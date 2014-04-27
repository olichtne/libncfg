#ifndef __LAGGR_DEVICE_H__
#define __LAGGR_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device.h>

class LAggregationDevice: public NetDevice{
    private:
        enum dev_type type = LAGGR_DEVICE;
};

typedef LAggregationDevice* create_laggrdev();
typedef void destroy_laggrdev(LAggregationDevice*);

#endif
