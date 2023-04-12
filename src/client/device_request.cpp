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

#include <_aiu/logger.h>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

static auto getStream(const char *host, const char *port) {
  static boost::asio::io_context ioc;
  static boost::asio::ip::tcp::resolver resolver(ioc);

  beast::tcp_stream stream(ioc);
  AIU_LOG_INFO << "AIUDeviceRequest::setup: " << host << ", " << port
               << std::endl;
  try {
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    stream.connect(results);
  } catch (std::exception const &e) {
    AIU_LOG_ERROR << "AIUDeviceRequest::setup: " << e.what();
  }

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

  AIU_LOG_INFO << "MD5: " << result;
  return result;
}

struct _AIUDeviceRequest {
  AIUDevice device;
  // The io_context is required for all I/O
  beast::tcp_stream stream;
  beast::flat_buffer buffer;
  std::string result;
  AIUResultCode status;

  _AIUDeviceRequest(AIUDevice _d)
      : device(_d),
        stream(getStream(device->getHost().c_str(), device->getPort().c_str())),
        status(AIU_SUCCESS) {
    // if (!stream)
    //   status = AIU_FAILURE;
  }

  ~_AIUDeviceRequest() {
    AIU_LOG_FUNC(~_AIUDeviceRequest);
  }

  AIUResultCode send(AIURequestCode code, const std::string &data) {
    if (status == AIU_SUCCESS) {
      std::string md5sum = getMD5SUM(data);

      const char *target = "/foo";
      int version = 10;
      try {
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

        AIU_LOG_INFO << "AIUDeviceRequest::send: " << req;

        // Send the HTTP request to the remote host
        http::write(stream, req);

      } catch (std::exception const &e) {
        AIU_LOG_ERROR << "AIUDeviceRequest::send: " << e.what();
        status = AIU_FAILURE;
      }
    }
    return status;
  }

  AIUResultCode recv() {
    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    try {
      // Receive the HTTP response
      http::read(stream, buffer, res);

      // Write the message to standard out
      AIU_LOG_DBG << "AIUDeviceRequest::recv: " << res;

      // TODO: defer until all comms are complete
      // Gracefully close the socket
      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    } catch (std::exception const &e) {
      AIU_LOG_ERROR << "AIUDeviceRequest::recv: " << e.what();
      return AIU_FAILURE;
    }

    std::string body = boost::beast::buffers_to_string(res.body().data());

    return AIU_SUCCESS;
  }
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

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
////  Device Management
AIUDevices::AIUDevices() {
  device_vec.reserve(8);
  // add non-device
  device_vec.push_back(new _AIUDevice("", "", "", AIU_DEVICE_TYPE_NONE));
  const char *etc_path = "/etc/aiunite";
  AIU_LOG_INFO << "AIUDevices in: " << etc_path;
  // read /etc/aiunite and add kg for each file
  for (auto fpath : directory_iterator(etc_path)) {
    if (is_regular_file(fpath)) {
      std::string port, host, name, device_type;
      ifstream file(fpath.path());
      file >> port >> host >> name >> device_type;
      AIU_LOG_INFO << "AIUDevice: " << host << ", " << port << ", " << name;
      device_vec.push_back(
          new _AIUDevice(port, host, name, AIU_DEVICE_TYPE_GPU));
    }
  }
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

struct _AIURequest {
  AIURequestCode request_code;
  std::list<AIUDeviceRequest> device_reqs;

  // The io_context is required for all I/O
  _AIURequest(AIUModel model_, AIURequestCode code_) : request_code(code_) {
    auto &model_str = model_->print();
    auto devs = AIUDevices::get();

    AIU_LOG_DBG << "AIURequest: " << model_str;
    for (size_t i = 0; i < devs.size(); ++i) {
      device_reqs.push_back(devs.get(i)->sendRequest(code_, model_str));
    }
  }

  ~_AIURequest() {
    for (auto req : device_reqs) {
      delete req;
    }
  }

  void recvAll() {
    for (auto req : device_reqs) {
      req->recv();
    }
  }
};

extern "C" AIUResultCode AIUSendModel(AIUModel model_, AIURequestCode code_,
                                      AIURequest *result_) {
  AIU_LOG_FUNC(AIUSendModel);
  AIU_CHECK_OBJECT(model_);
  AIU_CHECK_RESULT(result_);

  *result_ = new _AIURequest(model_, code_);
  return AIU_SUCCESS;
}
// with provider?

extern "C" AIUResultCode AIUGetRequestStatus(AIURequest request_, int64_t *count_) {
  AIU_LOG_FUNC(AIURecvSolution);

  AIU_CHECK_OBJECT(request_);
  //request_->query();

  return AIU_SUCCESS;
}
extern "C" AIUResultCode AIURecvSolutions(AIURequest request_) {
  AIU_LOG_FUNC(AIURecvSolutions);

  AIU_CHECK_OBJECT(request_);
  request_->recvAll();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIURecvSolution(AIURequest request_, AIUSolution *result_) {
  AIU_LOG_FUNC(AIURecvSolution);

  AIU_CHECK_OBJECT(request_);
  request_->recvAll();

  return AIU_SUCCESS;
}

