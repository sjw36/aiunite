/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_DEVICES_H
#define _AIU_CLIENT_DEVICES_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

#include <list>
#include <string>
#include <vector>

struct _AIUDevice {

  _AIUDevice(const std::string &_p, const std::string &_h,
             const std::string &_n, AIUDeviceType _t);

  AIUDeviceType getType() const { return type; }
  const std::string &getPort() const { return port; }
  const std::string &getHost() const { return host; }
  const std::string &getName() const { return name; }

  AIUDeviceRequest sendRequest(AIURequestCode code, const std::string &body);

private:
  std::string port;
  std::string host;
  std::string name;
  AIUDeviceType type;

  std::list<AIUDeviceRequest> requests;
};

class AIUDevices {
  std::vector<AIUDevice> device_vec;
  AIUDevices();

public:
  static const AIUDevices &get() {
    static AIUDevices devices_s;
    return devices_s;
  }

  size_t size() const;
  AIUDevice get(size_t idx) const;
  AIUDevice lookup(AIUDeviceType t) const;
};

#endif /*  _AIU_CLIENT_DEVICES_H */
