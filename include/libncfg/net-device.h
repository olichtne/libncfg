/* Copyright (C) 2014 Ondrej Lichtner <ondrej.lichtner@gmail.com>
 *
 * This file is part of the LibNCFG library.
 *
 * The LibNCFG library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The LibNCFG library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the LibNCFG library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NETDEVICE_H__
#define __NETDEVICE_H__

#include <string>
#include <vector>
#include <libncfg/common.h>

class NetDevice{
        static const int type = NET_DEVICE;
    protected:
        virtual void initialize(int if_index, std::string name, class LibNCFG* lib)
            {throw MethodNotImplemented();}
        virtual int create()
            {throw MethodNotImplemented();}
        virtual int destroy()
            {throw MethodNotImplemented();}
        virtual void update()
            {throw MethodNotImplemented();}
    public:
        virtual int get_if_index()
            {throw MethodNotImplemented();}
        virtual std::vector<Event> get_events(bool clear_event_queue=true)
            {throw MethodNotImplemented();}
        virtual const int get_device_type()
            {return NetDevice::type;}

        virtual std::string get_name()
            {throw MethodNotImplemented();}
        virtual int set_name(std::string name)
            {throw MethodNotImplemented();}

        virtual std::vector<IpAddr> get_ip_addr_list()
            {throw MethodNotImplemented();}
        virtual int add_ip_addr(IpAddr &addr)
            {throw MethodNotImplemented();}
        virtual int change_ip_addr(IpAddr &addr)
            {throw MethodNotImplemented();}
        virtual int del_ip_addr(IpAddr &addr)
            {throw MethodNotImplemented();}

        virtual bool is_up()
            {throw MethodNotImplemented();}
        virtual bool is_down()
            {throw MethodNotImplemented();}
        virtual int set_up()
            {throw MethodNotImplemented();}
        virtual int set_down()
            {throw MethodNotImplemented();}
        virtual int get_state()
            {throw MethodNotImplemented();}

        friend class LibNCFG;
};

typedef NetDevice* create_netdev();
typedef void destroy_netdev(NetDevice*);
typedef dev_type type_netdev();

#endif
