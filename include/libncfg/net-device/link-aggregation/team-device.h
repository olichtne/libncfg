#ifndef __TEAM_DEVICE_H__
#define __TEAM_DEVICE_H__

#include <libncfg/net-device/l-aggregation-device.h>

class TeamDevice: public LAggregationDevice{
};

typedef TeamDevice* create_teamdev();
typedef void destroy_teamdev(TeamDevice*);

#endif
