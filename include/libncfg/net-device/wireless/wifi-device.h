#ifndef __WIFI_DEVICE_H__
#define __WIFI_DEVICE_H__

#include <libncfg/net-device/wireless-device.h>

class WiFiDevice: public WirelessDevice{
};

typedef WiFiDevice* create_wifidev();
typedef void destroy_wifidev(WiFiDevice*);

#endif
