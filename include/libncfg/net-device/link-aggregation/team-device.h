#ifndef __TEAM_DEVICE_H__
#define __TEAM_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/l-aggregation-device.h>

class TeamDevice: public LAggregationDevice{
    private:
        enum dev_type type = TEAM_DEVICE;
};

typedef TeamDevice* create_teamdev();
typedef void destroy_teamdev(TeamDevice*);

#endif
