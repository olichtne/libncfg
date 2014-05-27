#ifndef __WIRELESS_DEVICE_H__
#define __WIRELESS_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device.h>

class WirelessDevice: public NetDevice{
    private:
        static const int type = WIRELESS_DEVICE;
    public:
        virtual const int get_device_type()
            {return WirelessDevice::type;}
};

typedef WirelessDevice* create_wirelessdev();
typedef void destroy_wirelessdev(WirelessDevice*);

#endif
