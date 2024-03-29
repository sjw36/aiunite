/******************************************************************************/
/*  CPX:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/CAPI/IR.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinOps.h>

#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <aiunite/service.h>
#include <_aiu/support.h>
#include <_aiu/logger.h>

#include <algorithm>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

struct _AIURequest {
  _AIURequest(AIURequestCode rcode, const std::string &name,
              const std::string &ir, const std::string &md5)
      : code(rcode), model_name(name), md5(md5), context(registry) {
    context.loadDialect<mlir::tosa::TosaDialect, mlir::func::FuncDialect>();
    //,math::MathDialect, arith::ArithDialect>();

    auto mlir_buffer = llvm::MemoryBuffer::getMemBuffer(ir);
    llvm::SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(std::move(mlir_buffer), llvm::SMLoc());
    mlir::OwningOpRef<mlir::ModuleOp> moduleRef =
        mlir::parseSourceFile<mlir::ModuleOp>(sourceMgr, &context);
    if (moduleRef)
      module = moduleRef.release();
  }

  mlir::ModuleOp get() const { return module; }
  AIURequestCode getCode() const { return code; }
  const std::string &getMD5() const { return md5; }

private:
  AIURequestCode code;
  std::string model_name;
  std::string md5;
  mlir::DialectRegistry registry;
  mlir::MLIRContext context;
  mlir::ModuleOp module;
};

struct _AIUSolution {
  void setResult(AIUResponseCode rcode, mlir::ModuleOp module) {
    code = rcode;
    llvm::raw_string_ostream os(body);
    mlir::OpPrintingFlags flags;
    module.print(os, flags.enableDebugInfo().assumeVerified());
  }

  const std::string &get() const { return body; }
  AIUResponseCode getCode() const { return code; }

private:
  AIUResponseCode code;
  std::string body;
};

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path) {
  using beast::iequals;
  auto const ext = [&path] {
    auto const pos = path.rfind(".");
    if (pos == beast::string_view::npos)
      return beast::string_view{};
    return path.substr(pos);
  }();
  if (iequals(ext, ".tosa"))
    return "text/tosa";
  if (iequals(ext, ".tcp"))
    return "text/tcp";
  if (iequals(ext, ".php"))
    return "text/html";
  if (iequals(ext, ".css"))
    return "text/css";
  if (iequals(ext, ".txt"))
    return "text/plain";
  if (iequals(ext, ".js"))
    return "application/javascript";
  if (iequals(ext, ".json"))
    return "application/json";
  if (iequals(ext, ".xml"))
    return "application/xml";
  if (iequals(ext, ".swf"))
    return "application/x-shockwave-flash";
  if (iequals(ext, ".flv"))
    return "video/x-flv";
  if (iequals(ext, ".png"))
    return "image/png";
  if (iequals(ext, ".jpe"))
    return "image/jpeg";
  if (iequals(ext, ".jpeg"))
    return "image/jpeg";
  if (iequals(ext, ".jpg"))
    return "image/jpeg";
  if (iequals(ext, ".gif"))
    return "image/gif";
  if (iequals(ext, ".bmp"))
    return "image/bmp";
  if (iequals(ext, ".ico"))
    return "image/vnd.microsoft.icon";
  if (iequals(ext, ".tiff"))
    return "image/tiff";
  if (iequals(ext, ".tif"))
    return "image/tiff";
  if (iequals(ext, ".svg"))
    return "image/svg+xml";
  if (iequals(ext, ".svgz"))
    return "image/svg+xml";
  return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path) {
  if (base.empty())
    return std::string(path);
  std::string result(base);
#ifdef BOOST_MSVC
  char constexpr path_separator = '\\';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
  for (auto &c : result)
    if (c == '/')
      c = path_separator;
#else
  char constexpr path_separator = '/';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
#endif
  return result;
}

static http::response<http::string_body>
makeErrorResponse(http::status status_, uint32_t version,
                  const std::string &msg) {
  static std::string api_version = AIUNITE_VERSION_STR;
  http::response<http::string_body> res{status_, version};
  res.set(http::field::user_agent, api_version);
  res.set(http::field::server, api_version);
  res.set(http::field::content_type, "text/html");
  res.keep_alive(false); // keep_alive);
  res.body() = msg;
  res.prepare_payload();
  AIU_LOG_ERROR("RESPONSE: " << res);
  return res;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template <class Body, class Allocator, class Send>
void handle_request(beast::string_view doc_root, AIUCallBack callback,
                    http::request<Body, http::basic_fields<Allocator>> &&req,
                    Send &&send) {
  AIU_LOG_FUNC(handle_request);
  static std::string api_version = AIUNITE_VERSION_STR;

  AIU_LOG_DBG("REQUEST: " << req);

  // Returns a bad request response
  auto const bad_request = [&req](beast::string_view why) {
    return makeErrorResponse(http::status::bad_request, req.version(),
                             std::string(why));
  };

  // Returns a not found response
  auto const not_found = [&req](beast::string_view target) {
    std::string why =
        "The resource '" + std::string(target) + "' was not found.";
    return makeErrorResponse(http::status::not_found, req.version(), why);
  };

  // Returns a server error response
  auto const server_error = [&req](beast::string_view what) {
    std::string why = "An error occurred: '" + std::string(what) + "'";
    return makeErrorResponse(http::status::internal_server_error, req.version(),
                             why);
  };

  // Make sure we can handle the method
  if (req.method() != http::verb::get)
    return send(bad_request("Unsupported HTTP-method"));

  //////////////////////////////////////////////////////////////////////////
  // Process Request

  // TODO: verify versions

  std::string model_name = req.target().to_string();
  std::string req_body = req.body();

  // capture MD5sum
  // Q: Does it include size
  std::string md5 = req[http::field::content_md5].to_string();

  auto code = req[http::field::protocol_request];
  auto code_v = AIUGetRequestCode(code.to_string().c_str());

  _AIURequest aiu_req(code_v, model_name, req_body, md5);
  if (!aiu_req.get())
    return send(bad_request("Failed to load model"));

  _AIUSolution aiu_sol;

  {
    AIU_LOG_FUNC(ServiceCallback);
    AIUResponseCode res_code = (*callback)(&aiu_req, &aiu_sol);
    if (res_code != AIU_RESPONSE_SUCCESS || aiu_sol.getCode()) {
      const char *msg = "Generator failure";
      return send(server_error(msg));
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // Send Response

  http::response<http::string_body> res{http::status::ok, req.version()};
  res.set(http::field::user_agent, api_version);
  res.set(http::field::server, api_version);
  res.set(http::field::content_type, "text/xmodel");
  req.set(http::field::content_version, "1.0");
  res.content_length(aiu_sol.get().size());
  res.body() = aiu_sol.get();
  res.keep_alive(req.keep_alive());

  AIU_LOG_DBG("RESPONSE: " << res);
  return send(std::move(res));
}

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const *what) {
  // ssl::error::stream_truncated, also known as an SSL "short read",
  // indicates the peer closed the connection without performing the
  // required closing handshake (for example, Google does this to
  // improve performance). Generally this can be a security issue,
  // but if your communication protocol is self-terminated (as
  // it is with both HTTP and WebSocket) then you may simply
  // ignore the lack of close_notify.
  //
  // https://github.com/boostorg/beast/issues/38
  //
  // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
  //
  // When a short read would cut off the end of an HTTP message,
  // Beast returns the error beast::http::error::partial_message.
  // Therefore, if we see a short read here, it has occurred
  // after the message has been completed, so it is safe to ignore it.

  if (ec == net::ssl::error::stream_truncated)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

//------------------------------------------------------------------------------

// Echoes back all received WebSocket messages.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived> class websocket_session {
  // Access the derived class, this is part of
  // the Curiously Recurring Template Pattern idiom.
  Derived &derived() { return static_cast<Derived &>(*this); }

  beast::flat_buffer buffer_;

  // Start the asynchronous operation
  template <class Body, class Allocator>
  void do_accept(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    derived().ws().set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    derived().ws().set_option(
        websocket::stream_base::decorator([](websocket::response_type &res) {
          res.set(http::field::server,
                  std::string(AIUNITE_VERSION_STR) + " advanced-server-flex");
        }));

    // Accept the websocket handshake
    derived().ws().async_accept(
        req, beast::bind_front_handler(&websocket_session::on_accept,
                                       derived().shared_from_this()));
  }

  void on_accept(beast::error_code ec) {
    if (ec)
      return fail(ec, "accept");

    // Read a message
    do_read();
  }

  void do_read() {
    // Read a message into our buffer
    derived().ws().async_read(
        buffer_, beast::bind_front_handler(&websocket_session::on_read,
                                           derived().shared_from_this()));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the websocket_session was closed
    if (ec == websocket::error::closed)
      return;

    if (ec)
      fail(ec, "read");

    // Echo the message
    derived().ws().text(derived().ws().got_text());
    derived().ws().async_write(
        buffer_.data(),
        beast::bind_front_handler(&websocket_session::on_write,
                                  derived().shared_from_this()));
  }

  void on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return fail(ec, "write");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Do another read
    do_read();
  }

public:
  // Start the asynchronous operation
  template <class Body, class Allocator>
  void run(http::request<Body, http::basic_fields<Allocator>> req) {
    // Accept the WebSocket upgrade request
    do_accept(std::move(req));
  }
};

//------------------------------------------------------------------------------

// Handles a plain WebSocket connection
class plain_websocket_session
    : public websocket_session<plain_websocket_session>,
      public std::enable_shared_from_this<plain_websocket_session> {
  websocket::stream<beast::tcp_stream> ws_;

public:
  // Create the session
  explicit plain_websocket_session(beast::tcp_stream &&stream)
      : ws_(std::move(stream)) {}

  // Called by the base class
  websocket::stream<beast::tcp_stream> &ws() { return ws_; }
};

//------------------------------------------------------------------------------

// Handles an SSL WebSocket connection
class ssl_websocket_session
    : public websocket_session<ssl_websocket_session>,
      public std::enable_shared_from_this<ssl_websocket_session> {
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

public:
  // Create the ssl_websocket_session
  explicit ssl_websocket_session(beast::ssl_stream<beast::tcp_stream> &&stream)
      : ws_(std::move(stream)) {}

  // Called by the base class
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> &ws() { return ws_; }
};

//------------------------------------------------------------------------------

template <class Body, class Allocator>
void make_websocket_session(
    beast::tcp_stream stream,
    http::request<Body, http::basic_fields<Allocator>> req) {
  std::make_shared<plain_websocket_session>(std::move(stream))
      ->run(std::move(req));
}

template <class Body, class Allocator>
void make_websocket_session(
    beast::ssl_stream<beast::tcp_stream> stream,
    http::request<Body, http::basic_fields<Allocator>> req) {
  std::make_shared<ssl_websocket_session>(std::move(stream))
      ->run(std::move(req));
}

//------------------------------------------------------------------------------

// Handles an HTTP server connection.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived> class http_session {
  // Access the derived class, this is part of
  // the Curiously Recurring Template Pattern idiom.
  Derived &derived() { return static_cast<Derived &>(*this); }

  // This queue is used for HTTP pipelining.
  class queue {
    enum {
      // Maximum number of responses we will queue
      limit = 8
    };

    // The type-erased, saved work item
    struct work {
      virtual ~work() = default;
      virtual void operator()() = 0;
    };

    http_session &self_;
    std::vector<std::unique_ptr<work>> items_;

  public:
    explicit queue(http_session &self) : self_(self) {
      static_assert(limit > 0, "queue limit must be positive");
      items_.reserve(limit);
    }

    // Returns `true` if we have reached the queue limit
    bool is_full() const { return items_.size() >= limit; }

    // Called when a message finishes sending
    // Returns `true` if the caller should initiate a read
    bool on_write() {
      BOOST_ASSERT(!items_.empty());
      auto const was_full = is_full();
      items_.erase(items_.begin());
      if (!items_.empty())
        (*items_.front())();
      return was_full;
    }

    // Called by the HTTP handler to send a response.
    template <bool isRequest, class Body, class Fields>
    void operator()(http::message<isRequest, Body, Fields> &&msg) {
      // This holds a work item
      struct work_impl : work {
        http_session &self_;
        http::message<isRequest, Body, Fields> msg_;

        work_impl(http_session &self,
                  http::message<isRequest, Body, Fields> &&msg)
            : self_(self), msg_(std::move(msg)) {}

        void operator()() {
          http::async_write(
              self_.derived().stream(), msg_,
              beast::bind_front_handler(&http_session::on_write,
                                        self_.derived().shared_from_this(),
                                        msg_.need_eof()));
        }
      };

      // Allocate and store the work
      items_.push_back(boost::make_unique<work_impl>(self_, std::move(msg)));

      // If there was no previous work, start this one
      if (items_.size() == 1)
        (*items_.front())();
    }
  };

  std::shared_ptr<std::string const> doc_root_;
  queue queue_;

  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> parser_;

protected:
  beast::flat_buffer buffer_;
  AIUCallBack callback_;

public:
  // Construct the session
  http_session(beast::flat_buffer buffer,
               std::shared_ptr<std::string const> const &doc_root,
               AIUCallBack callback)
      : doc_root_(doc_root), queue_(*this), buffer_(std::move(buffer)),
        callback_(callback) {}

  void do_read() {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000);

    // Set the timeout.
    beast::get_lowest_layer(derived().stream())
        .expires_after(std::chrono::seconds(30));

    // Read a request using the parser-oriented interface
    http::async_read(derived().stream(), buffer_, *parser_,
                     beast::bind_front_handler(&http_session::on_read,
                                               derived().shared_from_this()));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
      return derived().do_eof();

    if (ec)
      return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if (websocket::is_upgrade(parser_->get())) {
      // Disable the timeout.
      // The websocket::stream uses its own timeout settings.
      beast::get_lowest_layer(derived().stream()).expires_never();

      // Create a websocket session, transferring ownership
      // of both the socket and the HTTP request.
      return make_websocket_session(derived().release_stream(),
                                    parser_->release());
    }

    // Send the response
    handle_request(*doc_root_, callback_, parser_->release(), queue_);

    // If we aren't at the queue limit, try to pipeline another request
    if (!queue_.is_full())
      do_read();
  }

  void on_write(bool close, beast::error_code ec,
                std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return fail(ec, "write");

    if (close) {
      // This means we should close the connection, usually because
      // the response indicated the "Connection: close" semantic.
      return derived().do_eof();
    }

    // Inform the queue that a write completed
    if (queue_.on_write()) {
      // Read another request
      do_read();
    }
  }
};

//------------------------------------------------------------------------------

// Handles a plain HTTP connection
class plain_http_session
    : public http_session<plain_http_session>,
      public std::enable_shared_from_this<plain_http_session> {
  beast::tcp_stream stream_;

public:
  // Create the session
  plain_http_session(beast::tcp_stream &&stream, beast::flat_buffer &&buffer,
                     std::shared_ptr<std::string const> const &doc_root,
                     AIUCallBack callback)
      : http_session<plain_http_session>(std::move(buffer), doc_root, callback),
        stream_(std::move(stream)) {}

  // Start the session
  void run() { this->do_read(); }

  // Called by the base class
  beast::tcp_stream &stream() { return stream_; }

  // Called by the base class
  beast::tcp_stream release_stream() { return std::move(stream_); }

  // Called by the base class
  void do_eof() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

// Handles an SSL HTTP connection
class ssl_http_session : public http_session<ssl_http_session>,
                         public std::enable_shared_from_this<ssl_http_session> {
  beast::ssl_stream<beast::tcp_stream> stream_;

public:
  // Create the http_session
  ssl_http_session(beast::tcp_stream &&stream, ssl::context &ctx,
                   beast::flat_buffer &&buffer,
                   std::shared_ptr<std::string const> const &doc_root,
                   AIUCallBack callback)
      : http_session<ssl_http_session>(std::move(buffer), doc_root, callback),
        stream_(std::move(stream), ctx) {}

  // Start the session
  void run() {
    // Set the timeout.
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    // Perform the SSL handshake
    // Note, this is the buffered version of the handshake.
    stream_.async_handshake(
        ssl::stream_base::server, buffer_.data(),
        beast::bind_front_handler(&ssl_http_session::on_handshake,
                                  shared_from_this()));
  }

  // Called by the base class
  beast::ssl_stream<beast::tcp_stream> &stream() { return stream_; }

  // Called by the base class
  beast::ssl_stream<beast::tcp_stream> release_stream() {
    return std::move(stream_);
  }

  // Called by the base class
  void do_eof() {
    // Set the timeout.
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    // Perform the SSL shutdown
    stream_.async_shutdown(beast::bind_front_handler(
        &ssl_http_session::on_shutdown, shared_from_this()));
  }

private:
  void on_handshake(beast::error_code ec, std::size_t bytes_used) {
    if (ec)
      return fail(ec, "handshake");

    // Consume the portion of the buffer used by the handshake
    buffer_.consume(bytes_used);

    do_read();
  }

  void on_shutdown(beast::error_code ec) {
    if (ec)
      return fail(ec, "shutdown");

    // At this point the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

// Detects SSL handshakes
class detect_session : public std::enable_shared_from_this<detect_session> {
  beast::tcp_stream stream_;
  ssl::context &ctx_;
  std::shared_ptr<std::string const> doc_root_;
  beast::flat_buffer buffer_;
  AIUCallBack callback_;

public:
  explicit detect_session(tcp::socket &&socket, ssl::context &ctx,
                          std::shared_ptr<std::string const> const &doc_root,
                          AIUCallBack callback)
      : stream_(std::move(socket)), ctx_(ctx), doc_root_(doc_root),
        callback_(callback) {}

  // Launch the detector
  void run() {
    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    beast::async_detect_ssl(
        stream_, buffer_,
        beast::bind_front_handler(&detect_session::on_detect,
                                  this->shared_from_this()));
  }

  void on_detect(beast::error_code ec, bool result) {
    if (ec)
      return fail(ec, "detect");

    if (result) {
      // Launch SSL session
      std::make_shared<ssl_http_session>(
          std::move(stream_), ctx_, std::move(buffer_), doc_root_, callback_)
          ->run();
      return;
    }

    // Launch plain session
    std::make_shared<plain_http_session>(std::move(stream_), std::move(buffer_),
                                         doc_root_, callback_)
        ->run();
  }
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
  net::io_context &ioc_;
  ssl::context &ctx_;
  tcp::acceptor acceptor_;
  std::shared_ptr<std::string const> doc_root_;
  AIUCallBack callback_;

public:
  listener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint,
           std::shared_ptr<std::string const> const &doc_root,
           AIUCallBack callback)
      : ioc_(ioc), ctx_(ctx), acceptor_(net::make_strand(ioc)),
        doc_root_(doc_root), callback_(callback) {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
      fail(ec, "open");
      return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
      fail(ec, "set_option");
      return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
      fail(ec, "bind");
      return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
      fail(ec, "listen");
      return;
    }
  }

  // Start accepting incoming connections
  void run() { do_accept(); }

private:
  void do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(&listener::on_accept, shared_from_this()));
  }

  void on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
      fail(ec, "accept");
    } else {
      // Create the detector http_session and run it
      std::make_shared<detect_session>(std::move(socket), ctx_, doc_root_,
                                       callback_)
          ->run();
    }

    // Accept another connection
    do_accept();
  }
};

//------------------------------------------------------------------------------

extern "C" AIUResultCode AIUCreateService(int port, AIUCallBack callback) {
  const char *host = "0.0.0.0";
  auto const address = net::ip::make_address(host);
  auto const doc_root = std::make_shared<std::string>(".");
  const int threads = 8;

  AIU_LOG_FUNC(AIUCreateService);
  AIU_LOG_DBG("Address: " << host);
  AIU_LOG_DBG("   Port: " << port);
  
  // The io_context is required for all I/O
  net::io_context ioc{threads};

  // The SSL context is required, and holds certificates
  ssl::context ctx{ssl::context::tlsv12};

  // This holds the self-signed certificate used by the server
  // load_server_certificate(ctx);

  // Create and launch a listening port
  unsigned short port_s = static_cast<unsigned short>(port);
  std::make_shared<listener>(ioc, ctx, tcp::endpoint{address, port_s}, doc_root,
                             callback)
      ->run();

  // Capture SIGINT and SIGTERM to perform a clean shutdown
  net::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait([&](beast::error_code const &, int) {
    // Stop the `io_context`. This will cause `run()`
    // to return immediately, eventually destroying the
    // `io_context` and all of the sockets in it.
    ioc.stop();
  });

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i > 0; --i)
    v.emplace_back([&ioc] { ioc.run(); });
  ioc.run();

  // (If we get here, it means we got a SIGINT or SIGTERM)

  // Block until all the threads exit
  for (auto &t : v)
    t.join();
  return AIU_FAILURE;
}

/******************************************************************************/
/*  WALK                                                               */
/******************************************************************************/

extern "C" const char *AIUGetMD5(AIURequest request_) {
  AIU_LOG_FUNC(AIUGetMD5);
  if (request_ == nullptr)
    return nullptr;
  return request_->getMD5().c_str();
}

extern "C" MlirModule AIUGetModule(AIURequest request_) {
  AIU_LOG_FUNC(AIUGetModule);
  assert(request_);
  return wrap(request_->get());
}

extern "C" AIURequestCode AIUGetRequestCode(AIURequest request_) {
  AIU_LOG_FUNC(AIUGetRequestCode);
  if (request_ == nullptr)
    return AIU_REQUEST_TUNE;
  return request_->getCode();
}

/******************************************************************************/
/*  SOLUTION RESPONSE                                                         */
/******************************************************************************/

extern "C" AIUResultCode AIUSendModule(AIUSolution solution_,
                                       MlirModule module_) {
  AIU_LOG_FUNC(AIUSendModule);
  AIU_CHECK_OBJECT(solution_);
  solution_->setResult(0, unwrap(module_));
  return AIU_SUCCESS;
}
