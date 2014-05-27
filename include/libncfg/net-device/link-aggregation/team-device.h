#ifndef __TEAM_DEVICE_H__
#define __TEAM_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/l-aggr-device.h>

class TeamDevice: public LAggrDevice{
    private:
        static const int type = TEAM_DEVICE;
    public:
        virtual const int get_device_type()
            {return TeamDevice::type;}
};

typedef TeamDevice* create_teamdev();
typedef void destroy_teamdev(TeamDevice*);

#endif
