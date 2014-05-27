#ifndef __BONDING_DEVICE_H__
#define __BONDING_DEVICE_H__

#include <string>
#include <libncfg/common.h>
#include <libncfg/net-device.h>
#include <libncfg/net-device/l-aggr-device.h>

class BondingDevice: public LAggrDevice{
    private:
        static const int type = BOND_DEVICE;
    public:
        virtual int set_mode(int mode)
            {throw MethodNotImplemented();}
        virtual int set_mode(std::string mode)
            {throw MethodNotImplemented();}
        virtual int get_mode()
            {throw MethodNotImplemented();}
        virtual std::string get_mode_string()
            {throw MethodNotImplemented();}

        virtual int set_active_slave(NetDevice *dev)
            {throw MethodNotImplemented();}
        virtual NetDevice *get_active_slave()
            {throw MethodNotImplemented();}

        virtual int set_arp_interval(int interval)
            {throw MethodNotImplemented();}
        virtual int get_arp_interval()
            {throw MethodNotImplemented();}

        virtual int set_fail_over_mac(int val)
            {throw MethodNotImplemented();}
        virtual int set_fail_over_mac(std::string val)
            {throw MethodNotImplemented();}
        virtual int get_fail_over_mac()
            {throw MethodNotImplemented();}
        virtual std::string get_fail_over_mac_str()
            {throw MethodNotImplemented();}

        virtual int set_min_links(int val)
            {throw MethodNotImplemented();}
        virtual int get_min_links()
            {throw MethodNotImplemented();}

        virtual int set_primary(NetDevice *dev)
            {throw MethodNotImplemented();}
        virtual NetDevice *get_primary()
            {throw MethodNotImplemented();}

        virtual const int get_device_type()
            {return BondingDevice::type;}
};

typedef BondingDevice* create_bonddev();
typedef void destroy_bonddev(BondingDevice*);

#endif
