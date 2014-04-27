#ifndef __WIRELESS_DEVICE_H__
#define __WIRELESS_DEVICE_H__

#include <libncfg/net-device.h>

class WirelessDevice: public NetDevice{
};

typedef WirelessDevice* create_wirelessdev();
typedef void destroy_wirelessdev(WirelessDevice*);

#endif
