#ifndef __WIMAX_DEVICE_H__
#define __WIMAX_DEVICE_H__

#include <libncfg/net-device/wireless-device.h>

class WiMAXDevice: public WirelessDevice{
};

typedef WiMAXDevice* create_wimaxdev();
typedef void destroy_wimaxdev(WiMAXDevice*);

#endif
