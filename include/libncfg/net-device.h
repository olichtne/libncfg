#ifndef __NETDEVICE_H__
#define __NETDEVICE_H__

#include <string>
#include <vector>
#include <libncfg/common.h>

class NetDevice{
    public:
        virtual int get_if_id() {throw MethodNotImplemented();}

        virtual std::string get_name()
            {throw MethodNotImplemented();}
        virtual bool set_name(std::string name)
            {throw MethodNotImplemented();}

        virtual std::vector<IpAddr *> get_ip_addr_list()
            {throw MethodNotImplemented();}
        virtual bool add_ip_addr(IpAddr &addr)
            {throw MethodNotImplemented();}
        virtual bool del_ip_addr(IpAddr &addr)
            {throw MethodNotImplemented();}

        virtual int get_device_type()
            {throw MethodNotImplemented();}
};

typedef NetDevice* create_netdev();
typedef void destroy_netdev(NetDevice*);

#endif
