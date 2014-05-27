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

#ifndef __ETH_DEVICE_H__
#define __ETH_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/libncfg.h>
#include <libncfg/net-device.h>

class EthDevice: public NetDevice{
    private:
        static const int type = ETH_DEVICE;
    public:
        virtual HwAddr get_hw_addr()
            {throw MethodNotImplemented();}
        virtual int set_hw_addr(HwAddr hw_addr)
            {throw MethodNotImplemented();}

        virtual int get_arp()
            {throw MethodNotImplemented();}
        virtual int set_arp(bool val)
            {throw MethodNotImplemented();}

        virtual int get_multicast()
            {throw MethodNotImplemented();}
        virtual int set_multicast(bool val)
            {throw MethodNotImplemented();}

        virtual int get_dynamic()
            {throw MethodNotImplemented();}
        virtual int set_dynamic(bool val)
            {throw MethodNotImplemented();}

        virtual unsigned int get_mtu()
            {throw MethodNotImplemented();}
        virtual int set_mtu(unsigned int val)
            {throw MethodNotImplemented();}

        virtual unsigned int get_tx_queue_len()
            {throw MethodNotImplemented();}
        virtual int set_tx_queue_len(unsigned int val)
            {throw MethodNotImplemented();}

        virtual int set_master(NetDevice *master)
            {throw MethodNotImplemented();}
        virtual int set_master(int if_index)
            {throw MethodNotImplemented();}
        virtual int set_master(std::string name)
            {throw MethodNotImplemented();}
        virtual NetDevice *get_master()
            {throw MethodNotImplemented();}

        virtual const int get_device_type()
            {return EthDevice::type;}
};

typedef EthDevice* create_ethdev();
typedef void destroy_ethdev(EthDevice*);

#endif
