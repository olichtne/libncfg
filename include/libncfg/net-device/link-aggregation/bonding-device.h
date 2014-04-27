#ifndef __BONDING_DEVICE_H__
#define __BONDING_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/l-aggregation-device.h>

class BondingDevice: public LAggregationDevice{
    private:
        enum dev_type type = BOND_DEVICE;
};

typedef BondingDevice* create_bonddev();
typedef void destroy_bonddev(BondingDevice*);

#endif
