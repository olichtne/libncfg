#ifndef __WIMAX_DEVICE_H__
#define __WIMAX_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/wireless-device.h>

class WiMAXDevice: public WirelessDevice{
    private:
        static const int type = WIMAX_DEVICE;
    public:
        virtual const int get_device_type()
            {return WiMAXDevice::type;}
};

typedef WiMAXDevice* create_wimaxdev();
typedef void destroy_wimaxdev(WiMAXDevice*);

#endif
