#ifndef __ETH_DEVICE_H__
#define __ETH_DEVICE_H__

#include <net-device.h>

class EthDevice: public NetDevice{
    public:
        virtual std::vector<char> get_hw_address() = 0;
        virtual bool set_hw_address(std::vector<char> hw_addr) = 0;
};

#endif
