#ifndef __NETDEVICE_H__
#define __NETDEVICE_H__

#include <string>
#include <common.h>

class NetDevice{
    public:
        virtual int get_if_id() = 0;

        virtual std::string get_name() = 0;
        virtual bool set_name(std::string name) = 0;

        virtual std::vector<char> get_hw_address() = 0;
        virtual bool set_hw_address(std::vector<char> hw_addr) = 0;

        virtual std::vector<IpAddr *> get_ip_addr_list() = 0;
        virtual bool add_ip_addr(IpAddr &addr) = 0;
        virtual bool del_ip_addr(IpAddr &addr) = 0;

        virtual int get_device_type() = 0;
}

#endif
