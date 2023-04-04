/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

// #include <mlir/CAPI/IR.h>
// #include <mlir/Dialect/Func/IR/FuncOps.h>
// #include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinOps.h>

#include <aiunite/client.h>

#define AIU_CHECK_OBJECT(X) if (X != nullptr) ; \
  else return AIU_INVALID_OBJECT
#define AIU_GET_OBJECT(X) AIU_CHECK_OBJECT(X ## _);    \
  auto X = X ## _->_d

#define AIU_CHECK_RESULT(X) if (X != nullptr || *X != nullptr) ; \
  else return AIU_INVALID_RESULT_PTR


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
  // The io_context is required for all I/O
  net::io_context ioc;

  // These objects perform our I/O
  tcp::resolver resolver;
  beast::tcp_stream stream;
  beast::flat_buffer buffer;

  _AIURequest() : ioc(), resolver(ioc), stream(ioc) {}

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

  _AIURequest *request = new _AIURequest;
  try {

    request->init(host, port);

    const char *kernel_str;
    AIUPrintModel(kernel, &kernel_str);
    std::string post_data(kernel_str);

    std::string md5sum = getMD5SUM(post_data);
    
    std::cout << "Model: " << post_data << std::endl;

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, AIUNITE_VERSION_STR); // TOSA v1.0
    req.set(http::field::content_type, "text/tosa");
    req.set(http::field::content_version, "1.0");
    req.set(http::field::content_md5, md5sum);
    req.set(http::field::protocol_request, AIUGetRequestString(request_code));
    req.keep_alive();
    req.content_length(post_data.size());
    req.body() = post_data;

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

struct _AIUSolution {
  // graph contains binaries
  mlir::ModuleOp _module;
};

extern "C" AIUResultCode AIURecvSolution(AIURequest request_,
                                         AIUSolution *result_) {
  AIU_CHECK_OBJECT(request_);
  AIU_CHECK_RESULT(result_);

  // Declare a container to hold the response
  http::response<http::dynamic_body> res;

  try {
    // This buffer is used for reading and must be persisted

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

  

  *result_ = new _AIUSolution{};

  return AIU_SUCCESS;
}

/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/

/* -- Generated kernels and call graph */
extern "C" AIUResultCode AIUGetObject(AIUSolution solution, AIUModel kernel,
                                      AIUBinary *result) {
  return AIU_FAILURE;
}
/*   - EGraph */
/*   - Binary(s) */

/* -- Tuning space */



