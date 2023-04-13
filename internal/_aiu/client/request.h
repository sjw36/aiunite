/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_REQUEST_H
#define _AIU_CLIENT_REQUEST_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

#include <vector>

struct _AIURequest {
  AIURequestCode request_code;
  std::vector<AIUDeviceRequest> device_reqs;

  // The io_context is required for all I/O
  _AIURequest(AIUModel model_, AIURequestCode code_);

  ~_AIURequest();

  void recvAll();

  size_t size() const { return device_reqs.size(); }
  AIUDeviceRequest get(size_t idx) const {
    return device_reqs[idx];
  }
};

#endif /* _AIU_CLIENT_REQUEST_H */
