#ifndef __INFINIBAND_DEVICE_H__
#define __INFINIBAND_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device.h>

class InfiniBandDevice: public NetDevice{
    private:
        static const int type = INFINIBAND_DEVICE;
    public:
        virtual std::vector<char> get_hw_address()
            {throw MethodNotImplemented();}
        virtual bool set_hw_address(std::vector<char> hw_addr)
            {throw MethodNotImplemented();}

        virtual const int get_device_type()
            {return InfiniBandDevice::type;}
};

typedef InfiniBandDevice* create_ethdev();
typedef void destroy_ethdev(InfiniBandDevice*);

#endif
