/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/IR/BuiltinOps.h>
#include <mlir/Parser/Parser.h>

#include <aiunite/client.h>
#include <aiunite/internal/model.h>
#include <aiunite/internal/solution.h>
#include <aiunite/internal/support.h>

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

#include <boost/algorithm/hex.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

struct _AIURequest {
  AIUModel model;
  
  // The io_context is required for all I/O
  net::io_context ioc;

  // These objects perform our I/O
  tcp::resolver resolver;
  beast::tcp_stream stream;
  beast::flat_buffer buffer;

  _AIURequest(AIUModel model_)
    : model(model_), ioc(), resolver(ioc), stream(ioc) {}

  void init(const char *host, const char *port) {
    // Look up the domain name
    auto const results = resolver.resolve(host, port);
    // handle error
    // Make the connection on the IP address we get from a lookup
    stream.connect(results);
  }
};

using boost::uuids::detail::md5;

std::string getMD5SUM(const std::string &data) {
  md5 hash;
  md5::digest_type digest;

  hash.process_bytes(data.data(), data.size());
  hash.get_digest(digest);
  const auto charDigest = reinterpret_cast<const char *>(&digest);
  std::string result;
  boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type),
                        std::back_inserter(result));
  return result;
}

extern "C" AIUResultCode
AIUSendModel(AIUModel kernel, AIURequestCode request_code, AIURequest *result) {

  const char *host = "0.0.0.0";
  const char *port = "8000";
  const char *target = "/foo.html";
  int version = 10;

  _AIURequest *request = new _AIURequest(kernel);
  try {

    request->init(host, port);

    const char *kernel_str;
    AIUPrintModel(kernel, &kernel_str);
    std::string data(kernel_str);

    std::string md5sum = getMD5SUM(data);
    
    std::cout << "Model: " << data << std::endl;

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, AIUNITE_VERSION_STR); // TOSA v1.0
    req.set(http::field::content_type, "text/tosa");
    req.set(http::field::content_version, "1.0");
    req.set(http::field::content_md5, md5sum);
    req.set(http::field::protocol_request, AIUGetRequestString(request_code));
    req.keep_alive();
    req.content_length(data.size());
    req.body() = data;

    // Send the HTTP request to the remote host
    http::write(request->stream, req);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    delete request;
    return AIU_FAILURE;
  }

  if (result)
    *result = request;

  return AIU_SUCCESS;
}
// with provider?

extern "C" AIUResultCode
AIURecvSolution(AIURequest request_, AIUSolution *result_) {
  AIU_CHECK_OBJECT(request_);
  AIU_CHECK_RESULT(result_);

  // Declare a container to hold the response
  http::response<http::dynamic_body> res;

  try {
    // Receive the HTTP response
    http::read(request_->stream, request_->buffer, res);
  
    // Write the message to standard out
    std::cout << res << std::endl;

    // Gracefully close the socket
    beast::error_code ec;
    request_->stream.socket().shutdown(tcp::socket::shutdown_both, ec);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return AIU_FAILURE;
  }

  std::string body = boost::beast::buffers_to_string(res.body().data());
  *result_ = new _AIUSolution(body);

  return AIU_SUCCESS;
}

