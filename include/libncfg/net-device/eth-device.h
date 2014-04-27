#ifndef __ETH_DEVICE_H__
#define __ETH_DEVICE_H__

#include <libncfg/net-device.h>

class EthDevice: public NetDevice{
    public:
        virtual std::vector<char> get_hw_address()
            {throw MethodNotImplemented();}
        virtual bool set_hw_address(std::vector<char> hw_addr)
            {throw MethodNotImplemented();}
};

typedef EthDevice* create_ethdev();
typedef void destroy_ethdev(EthDevice*);

#endif
