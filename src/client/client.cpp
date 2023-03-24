/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/CAPI/IR.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinOps.h>

#include <aiunite/client.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C" AIUResultCode AIUReadRegistry(const char *filename) {
  return AIU_FAILURE;
}

/******************************************************************************/
/*  CONTEXT MGMT                                                              */
/******************************************************************************/

struct _AIUContext {
  mlir::DialectRegistry _reg;
  mlir::MLIRContext *_d;
  mlir::Location _loc;

  mlir::DialectRegistry &getRegistry() {
    _reg.insert<mlir::func::FuncDialect>();
    return _reg;
  }

  _AIUContext() :
    _d(new mlir::MLIRContext(getRegistry())),
    _loc(mlir::UnknownLoc::get(_d))
  {
    _d->loadDialect<mlir::func::FuncDialect>();
    _d->loadDialect<mlir::tosa::TosaDialect>();
  }
  ~_AIUContext() {
    delete _d;
  }
};

extern "C" AIUResultCode AIUCreateContext(AIUContext *result) {
  assert(result != nullptr);
  *result = new _AIUContext;
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUDestroyContext(AIUContext context) {
  delete context;
  return AIU_SUCCESS;
}

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

struct _AIUModel {
  mlir::ModuleOp _module;
  mlir::func::FuncOp _d;
};

extern "C" AIUResultCode AIUCreateModel(AIUContext context, const char *name,
                                        AIUModel *result) {
  std::string modname = "module_";
  auto mod = mlir::ModuleOp::create(context->_loc, modname + name);
  mlir::OpBuilder b(mod);

  auto funcType = b.getFunctionType({}, {});
  auto func = b.create<mlir::func::FuncOp>(context->_loc, name, funcType);

  mlir::Block *block = func.addEntryBlock();
  b.setInsertionPointToStart(block);

  b.create<mlir::func::ReturnOp>(context->_loc);

  mod.push_back(func);

  *result = new _AIUModel{mod, func};
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUDestroyModel(AIUModel func) {
  delete func;
  return AIU_SUCCESS;
}

/* 1. Generator Spec */
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, const char *options,
                                           AIUModel *result) {
  return AIU_FAILURE;
}

/*
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, AIUOpType optype, ...,
AIUModel *result);
*/

/* 2. Builder Spec */
struct _AIUType {
  MlirType _type;
};

struct _AIUValue {
  MlirValue _value;
};

struct _AIUOperation {
  MlirOperation _operation;
};

/*  - types */
extern "C" AIUResultCode AIUMakeTensorType(AIUContext, int dims[],
                                           AIUType elemType, AIUType *result) {
  return AIU_FAILURE;
}

/*  - kernel func */
extern "C" AIUResultCode AIUAddParameter(AIUContext, AIUModel func,
                                         AIUType type, AIUValue *result) {
  return AIU_FAILURE;
}

/* 3. Clone Spec */
extern "C" AIUResultCode AIUCloneModel(AIUContext context, MlirOperation c_func,
                                       AIUModel *result) {
  auto func = llvm::dyn_cast<mlir::func::FuncOp>(*unwrap(c_func));

  std::string modname = "module_";
  auto mod =
      mlir::ModuleOp::create(context->_loc, (modname + func.getName()).str());

  mod.push_back(func.clone());

  // strip out locations
  // make large constants opaque

  *result = new _AIUModel{mod, func};
  return AIU_SUCCESS;
}

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

struct _AIURequest {
  // graph contains binaries
  mlir::ModuleOp _module;
  
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

extern "C" AIUResultCode
AIUSendModel(AIUModel kernel, AIURequestCode request_code, AIURequest *result) {

  const char *host = "0.0.0.0";
  const char *port = "8000";
  const char *target = "/foo.html";
  int version = 10;

  _AIURequest *request = new _AIURequest;
  try {

    request->init(host, port);

    std::string post_data;
    llvm::raw_string_ostream os(post_data);
    kernel->_module.print(os);

    std::cout << "Model: " << post_data << std::endl;

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, AIUNITE_VERSION_STR); // TOSA v1.0
    req.set(http::field::content_type, "text/tosa");
    req.set(http::field::content_version, "1.0");
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

struct _AIUSolution {};

extern "C" AIUResultCode AIURecvSolution(AIURequest request,
                                         AIUSolution *result) {

  AIUSolution solution = new _AIUSolution;

  try {
    // This buffer is used for reading and must be persisted

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(request->stream, request->buffer, res);
  
    // Write the message to standard out
    std::cout << res << std::endl;

    // Gracefully close the socket
    beast::error_code ec;
    request->stream.socket().shutdown(tcp::socket::shutdown_both, ec);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    delete solution;
    return AIU_FAILURE;
  }

  if (result)
    *result = solution;
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



