/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/IR/BuiltinOps.h>
#include <mlir/Parser/Parser.h>

#include <aiunite/client.h>
#include <_aiu/client/devices.h>
#include <_aiu/client/model.h>
#include <_aiu/client/request.h>
#include <_aiu/client/solution.h>
#include <_aiu/support.h>

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

#include <boost/algorithm/hex.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include <dirent.h>

#include <_aiu/logger.h>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum AIUStreamStatus {
    AIU_STREAM_STATUS_UP,
    AIU_STREAM_STATUS_DOWN,
    AIU_STREAM_STATUS_FAIL
};

enum AIURequestStatus {
    AIU_REQUEST_STATUS_INIT,
    AIU_REQUEST_STATUS_SENT,
    AIU_REQUEST_STATUS_RECV,
    AIU_REQUEST_STATUS_FAIL
};

static auto getStream(const std::string &host, const std::string &port,
                      AIUStreamStatus *status) {
  static boost::asio::io_context ioc;
  static boost::asio::ip::tcp::resolver resolver(ioc);

  *status = AIU_STREAM_STATUS_UP;
  
  beast::tcp_stream stream(ioc);
  AIU_LOG_INFO("AIUDeviceRequest::setup: " << host << ", " << port
               << std::endl);
#ifndef BOOST_NO_EXCEPTIONS
  try {
#endif // BOOST_NO_EXCEPTIONS
    auto const results = resolver.resolve(host.c_str(), port.c_str());

    // Make the connection on the IP address we get from a lookup
    stream.connect(results);

#ifndef BOOST_NO_EXCEPTIONS
  } catch (std::exception const &e) {
    AIU_LOG_ERROR("AIUDeviceRequest::setup: " << e.what());
    *status = AIU_STREAM_STATUS_FAIL;
  }
#else
  /// DO SOMETHING ELSE FOR ERRORs
#endif // BOOST_NO_EXCEPTIONS

  return stream;
}

using boost::uuids::detail::md5;

static std::string getMD5SUM(const std::string &data) {
  md5 hash;
  md5::digest_type digest;

  hash.process_bytes(data.data(), data.size());
  hash.get_digest(digest);
  const auto charDigest = reinterpret_cast<const char *>(&digest);
  std::string result;
  boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type),
                        std::back_inserter(result));

  AIU_LOG_INFO("MD5: " << result);
  return result;
}

struct _AIUDeviceRequest {
  _AIUDeviceRequest(AIUDevice _d)
      : device(_d),
        stream(getStream(device->getHost(), device->getPort(), &stream_status)),
        request_status(AIU_REQUEST_STATUS_INIT)
  {
    if (stream_status != AIU_STREAM_STATUS_UP) {
      request_status = AIU_REQUEST_STATUS_FAIL;
    }
  }

  ~_AIUDeviceRequest() {
    AIU_LOG_FUNC(~_AIUDeviceRequest);
  }

  AIUDevice getDevice() const {
    return device;
  }
  const std::string &getResult() const {
    return result;
  }
  AIURequestStatus getRequestStatus() const {
    return request_status;
  }

  AIUResultCode send(AIURequestCode code, const std::string &data) {
    if (request_status != AIU_REQUEST_STATUS_INIT) {
      return request_status != AIU_REQUEST_STATUS_FAIL ? AIU_SUCCESS : AIU_FAILURE;
    }
    AIUResultCode status = AIU_FAILURE;
    if (stream_status == AIU_STREAM_STATUS_UP) {
      std::string md5sum = getMD5SUM(data);

      const char *target = "/foo";
      int version = 10;
#ifndef BOOST_NO_EXCEPTIONS
      try {
#endif // BOOST_NO_EXCEPTIONS
        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, device->getHost());
        req.set(http::field::user_agent, AIUNITE_VERSION_STR); // TOSA v1.0
        req.set(http::field::content_type, "text/tosa");
        req.set(http::field::content_version, "1.0");
        req.set(http::field::content_md5, md5sum);
        req.set(http::field::protocol_request, AIUGetRequestString(code));
        // req.keep_alive();
        req.content_length(data.size());
        req.body() = data;

        AIU_LOG_INFO("AIUDeviceRequest::send: " << req);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        request_status = AIU_REQUEST_STATUS_SENT;
        status = AIU_SUCCESS;
#ifndef BOOST_NO_EXCEPTIONS
      } catch (std::exception const &e) {
        AIU_LOG_ERROR("AIUDeviceRequest::send: " << e.what());
        request_status = AIU_REQUEST_STATUS_FAIL;
      }
#endif // BOOST_NO_EXCEPTIONS
    }
    return status;
  }

  AIUResultCode recv() {
    if (request_status > AIU_REQUEST_STATUS_SENT) {
      return request_status != AIU_REQUEST_STATUS_FAIL ? AIU_SUCCESS : AIU_FAILURE;
    }
    AIUResultCode status = AIU_FAILURE;
    if (stream_status == AIU_STREAM_STATUS_UP) {
      // Declare a container to hold the response
      http::response<http::dynamic_body> res;

#ifndef BOOST_NO_EXCEPTIONS
      try {
#endif // BOOST_NO_EXCEPTIONS
        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Write the message to standard out
        AIU_LOG_DBG("AIUDeviceRequest::recv: " << res);

        // TODO: defer until all comms are complete
        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        request_status = AIU_REQUEST_STATUS_RECV;
        status = AIU_SUCCESS;
#ifndef BOOST_NO_EXCEPTIONS
      } catch (std::exception const &e) {
        AIU_LOG_ERROR("AIUDeviceRequest::recv: " << e.what());
        request_status = AIU_REQUEST_STATUS_FAIL;
        return status;
      }
#endif // BOOST_NO_EXCEPTIONS

      result = boost::beast::buffers_to_string(res.body().data());
    }

    return status;
  }

private:
  AIUDevice device;
  // The io_context is required for all I/O
  beast::tcp_stream stream;
  beast::flat_buffer buffer;
  std::string result;
  AIUStreamStatus stream_status;
  AIURequestStatus request_status;
};

///////////////////////////////////////////////////////////////////////////////
_AIUDevice::_AIUDevice(const std::string &_p, const std::string &_h,
                       const std::string &_n, AIUDeviceType _t)
    : port(_p), host(_h), name(_n), type(_t) {}

AIUDeviceRequest _AIUDevice::sendRequest(AIURequestCode code,
                                         const std::string &body) {
  auto *result = new _AIUDeviceRequest(this);
  result->send(code, body);
  return result;
}

#include <vector>


#ifndef BOOST_NO_EXCEPTIONS
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;
#endif // BOOST_NO_EXCEPTIONS

///////////////////////////////////////////////////////////////////////////////
////  Device Management
AIUDevices::AIUDevices() {
  device_vec.reserve(8);
  // add non-device
  device_vec.push_back(new _AIUDevice("", "", "", AIU_DEVICE_TYPE_NONE));
  const char *etc_path = "/etc/aiunite";
  AIU_LOG_INFO("AIUDevices in: " << etc_path);
  // read /etc/aiunite and add kg for each file
#if 0 // disable boost filesystem
  //#ifndef BOOST_NO_EXCEPTIONS
  for (auto fpath : directory_iterator(etc_path)) {
    if (is_regular_file(fpath)) {
      std::string port, host, name, device_type;
      ifstream file(fpath.path());
      file >> port >> host >> name >> device_type;
      AIU_LOG_INFO("AIUDevice: " << host << ", " << port << ", " << name);
      device_vec.push_back(
          new _AIUDevice(port, host, name, AIU_DEVICE_TYPE_GPU));
    }
  }
#else
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(etc_path)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      std::string port, host, name, device_type;
      ifstream file(ent->d_name);
      file >> port >> host >> name >> device_type;
      AIU_LOG_INFO("AIUDevice: " << host << ", " << port << ", " << name);
      device_vec.push_back(
          new _AIUDevice(port, host, name, AIU_DEVICE_TYPE_GPU));
    }
    closedir (dir);
  }
#endif // BOOST_NO_EXCEPTIONS
}

size_t AIUDevices::size() const { return device_vec.size() - 1; }

AIUDevice AIUDevices::get(size_t idx) const {
  idx++;
  if (idx < device_vec.size())
    return const_cast<AIUDevice>(device_vec[idx]);
  return const_cast<AIUDevice>(device_vec[0]);
}

AIUDevice AIUDevices::lookup(AIUDeviceType t) const {
  for (size_t i = 1; i < device_vec.size(); ++i) {
    if (device_vec[i]->getType() == t)
      return const_cast<AIUDevice>(device_vec[i]);
  }
  return const_cast<AIUDevice>(device_vec[0]);
}

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

_AIURequest::_AIURequest(AIUModel model_, AIURequestCode code_) : request_code(code_) {
  auto &model_str = model_->print();
  auto devs = AIUDevices::get();

  device_reqs.reserve(devs.size());
  solutions.reserve(devs.size());

  AIU_LOG_DBG("AIURequest: " << model_str);
  for (size_t i = 0; i < devs.size(); ++i) {
    device_reqs.push_back(devs.get(i)->sendRequest(code_, model_str));
    solutions.push_back(nullptr);
  }
}

_AIURequest::~_AIURequest() {
  for (auto req : device_reqs) {
    delete req;
  }
}

AIUSolution _AIURequest::recv(size_t index) {
  assert(index < device_reqs.size());
  if (solutions[index] == nullptr) {
    if (device_reqs[index]->recv() == AIU_SUCCESS) {
      auto devs = AIUDevices::get();
      solutions[index] = new _AIUSolution(devs.get(index), device_reqs[index]->getResult());
    }
  }
  return solutions[index];
}

AIUSolution _AIURequest::recv(AIUDevice dev) {
  for (size_t i = 0; i < device_reqs.size(); ++i) {
    auto req = device_reqs[i];
    if (req->getDevice() == dev)
      return recv(i);
  }
  return nullptr;
}

void _AIURequest::recvAll() {
  for (size_t i = 0; i < device_reqs.size(); ++i)
    recv(i);
}

/******************************************************************************/
/*  SUBMIT REQUEST, GET SOLUTIONS                                             */
/******************************************************************************/
extern "C" AIUResultCode AIUSendModel(AIUModel model_, AIURequestCode code_,
                                      AIURequest *result_) {
  AIU_LOG_FUNC(AIUSendModel);
  AIU_CHECK_OBJECT(model_);
  AIU_CHECK_RESULT(result_);

  *result_ = new _AIURequest(model_, code_);
  return AIU_SUCCESS;
}


extern "C" AIUResultCode AIUGetRequestCount(AIURequest request_, int64_t *result_) {
  AIU_LOG_FUNC(AIUGetRequestCount);
  AIU_CHECK_OBJECT(request_);
  AIU_CHECK_RESULT(result_);
  
  *result_ = request_->size();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetRequestStatus(AIURequest request_, int64_t *count_) {
  AIU_LOG_FUNC(AIUGetRequestStatus);

  AIU_CHECK_OBJECT(request_);
  //request_->query();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIURecvSolutions(AIURequest request_) {
  AIU_LOG_FUNC(AIUGetSolutions);

  AIU_CHECK_OBJECT(request_);
  request_->recvAll();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetSolution(AIURequest request_, size_t index_,
                                         AIUSolution *result_) {
  AIU_LOG_FUNC(AIUGetSolution);

  AIU_CHECK_OBJECT(request_);
  *result_ = request_->recv(index_);

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetSolutionFromDevice(AIURequest request_,
                                                  AIUDevice device_,
                                                  AIUSolution *result_) {
  AIU_LOG_FUNC(AIUGetSolution);

  AIU_CHECK_OBJECT(request_);
  *result_ = request_->recv(device_);

  return AIU_SUCCESS;
}
