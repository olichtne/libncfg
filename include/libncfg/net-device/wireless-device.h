#ifndef __WIRELESS_DEVICE_H__
#define __WIRELESS_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device.h>

class WirelessDevice: public NetDevice{
    private:
        enum dev_type type = WIRELESS_DEVICE;
};

typedef WirelessDevice* create_wirelessdev();
typedef void destroy_wirelessdev(WirelessDevice*);

#endif
