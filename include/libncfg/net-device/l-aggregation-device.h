#ifndef __LAGGR_DEVICE_H__
#define __LAGGR_DEVICE_H__

#include <libncfg/net-device.h>

class LAggregationDevice: public NetDevice{
};

typedef LAggregationDevice* create_laggrdev();
typedef void destroy_laggrdev(LAggregationDevice*);

#endif
