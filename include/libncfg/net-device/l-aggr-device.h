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
