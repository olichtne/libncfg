#ifndef __LAGGR_DEVICE_H__
#define __LAGGR_DEVICE_H__

#include <vector>
#include <libncfg/common.h>
#include <libncfg/net-device.h>

class LAggrDevice: public NetDevice{
    private:
        static const int type = LAGGR_DEVICE;
    public:
        virtual int add_slave(NetDevice *dev)
            {throw MethodNotImplemented();}
        virtual int del_slave(NetDevice *dev)
            {throw MethodNotImplemented();}
        virtual std::vector<NetDevice *> get_slaves()
            {throw MethodNotImplemented();}

        virtual const int get_device_type()
            {return LAggrDevice::type;}
};

typedef LAggrDevice* create_laggrdev();
typedef void destroy_laggrdev(LAggrDevice*);

#endif
