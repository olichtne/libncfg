#ifndef __WIFI_DEVICE_H__
#define __WIFI_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/wireless-device.h>

class WiFiDevice: public WirelessDevice{
    private:
        static const int type = WIFI_DEVICE;
    public:
        virtual const int get_device_type()
            {return WiFiDevice::type;}
};

typedef WiFiDevice* create_wifidev();
typedef void destroy_wifidev(WiFiDevice*);

#endif
