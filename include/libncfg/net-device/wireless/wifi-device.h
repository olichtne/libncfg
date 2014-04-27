#ifndef __WIFI_DEVICE_H__
#define __WIFI_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/wireless-device.h>

class WiFiDevice: public WirelessDevice{
    private:
        enum dev_type type = WIFI_DEVICE;
};

typedef WiFiDevice* create_wifidev();
typedef void destroy_wifidev(WiFiDevice*);

#endif
