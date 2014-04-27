#ifndef __NETDEVICE_H__
#define __NETDEVICE_H__

#include <string>
#include <vector>
#include <libncfg/common.h>

class NetDevice{
    private:
        enum dev_type type = NET_DEVICE;
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

        virtual enum dev_type get_device_type() final
            {return this->type;}
};

typedef NetDevice* create_netdev();
typedef void destroy_netdev(NetDevice*);

#endif
