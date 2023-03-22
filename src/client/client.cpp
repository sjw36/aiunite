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

extern "C"
AIUResult AIUReadRegistry(const char *filename) {
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


extern "C"
AIUResult
AIUCreateContext(AIUContext *result) {
  assert(result != nullptr);
  *result = new _AIUContext;
  return AIU_SUCCESS;
}

extern "C"
AIUResult
AIUDestroyContext(AIUContext context) {
  delete context;
  return AIU_SUCCESS;
}

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

struct _AIUFunc {
  mlir::ModuleOp _module;
  mlir::func::FuncOp _d;
};

extern "C"
AIUResult
AIUCreateFunc(AIUContext context, const char *name, AIUFunc *result) {
  auto mod = mlir::ModuleOp::create(context->_loc, name);
  mlir::OpBuilder b(mod);

  auto funcType = b.getFunctionType({}, {});
  auto func = b.create<mlir::func::FuncOp>(context->_loc, name, funcType);

  mlir::Block *block = func.addEntryBlock();
  b.setInsertionPointToStart(block);

  b.create<mlir::func::ReturnOp>(context->_loc);

  *result = new _AIUFunc{mod, func};
  return AIU_SUCCESS;
}

extern "C"
AIUResult
AIUDestroyFunc(AIUFunc func) {
  delete func;
  return AIU_SUCCESS;
}

/* 1. Generator Spec */
extern "C"
AIUResult AIUGenerateKernel(AIUContext, const char *options, AIUFunc *result) {
  return AIU_FAILURE;
}

/* 
extern "C" AIUResult AIUGenerateKernel(AIUContext, AIUOpType optype, ..., AIUFunc *result);
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
extern "C"
AIUResult AIUMakeTensorType(AIUContext, int dims[], AIUType elemType, AIUType *result) {
  return AIU_FAILURE;
}

/*  - kernel func */
extern "C"
AIUResult AIUAddFunctionParam(AIUContext, AIUFunc func, AIUType type, AIUValue *result) {
  return AIU_FAILURE;
}


/* 3. Clone Spec */
extern "C"
AIUResult AIUCloneFunc(AIUContext context, MlirOperation func, AIUFunc *result) {
  auto mod = mlir::ModuleOp::create(context->_loc, "foo");

  auto kernel = llvm::dyn_cast<mlir::func::FuncOp>(*unwrap(func));
  mod.push_back(kernel.clone());

  // strip out locations
  
  *result = new _AIUFunc{mod, kernel};
  return AIU_SUCCESS;
}

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

struct _AIUBinary {
};

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

  _AIURequest(const char *host, const char *port)
    : ioc(), resolver(ioc), stream(ioc) {
    // Look up the domain name
    auto const results = resolver.resolve(host, port);
    // handle error
    // Make the connection on the IP address we get from a lookup
    stream.connect(results);
  }

};


struct _AIUCBData {
  mlir::ModuleOp _module;
  bool _done;
};

static const char *s_url = "http://0.0.0.0:8000";
static const uint64_t s_timeout_ms = 1500;  // Connect timeout in milliseconds

extern "C"
AIUResult
AIUSubmitFunc(AIUContext context, AIUFunc kernel, AIUAction action, AIURequest *result) {
  kernel->_module.dump();

  const char *host = "0.0.0.0";
  const char *port = "8000";
  const char *target = "/foo.html";
  int version = 10;

  try {
    _AIURequest *request = new _AIURequest(host, port);
    *result = request;

    std::string post_data;
    llvm::raw_string_ostream os(post_data);
    kernel->_module.print(os);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::head, target, version, post_data};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING); // TOSA v1.0

    // Send the HTTP request to the remote host
    http::write(request->stream, req);

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
    
  } catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return AIU_FAILURE;
  }

  return AIU_SUCCESS;
}
// with provider?

extern "C"
AIUResult AIUReceiveSolution(AIUContext, AIUFunc kernel, AIUSolution *result) {
  return AIU_FAILURE;
}
  

/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/

/* -- Generated kernels and call graph */
extern "C"
AIUResult AIUGetObject(AIUSolution solution, AIUFunc kernel, AIUBinary *result) {
  return AIU_FAILURE;
}
/*   - EGraph */
/*   - Binary(s) */

/* -- Tuning space */



