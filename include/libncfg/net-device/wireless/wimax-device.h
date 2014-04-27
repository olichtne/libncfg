#ifndef __WIMAX_DEVICE_H__
#define __WIMAX_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/wireless-device.h>

class WiMAXDevice: public WirelessDevice{
    private:
        enum dev_type type = WIMAX_DEVICE;
};

typedef WiMAXDevice* create_wimaxdev();
typedef void destroy_wimaxdev(WiMAXDevice*);

#endif
