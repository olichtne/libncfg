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

#ifndef __WIRELESS_DEVICE_H__
#define __WIRELESS_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device.h>

class WirelessDevice: public NetDevice{
    private:
        static const int type = WIRELESS_DEVICE;
    public:
        virtual const int get_device_type()
            {return WirelessDevice::type;}
};

typedef WirelessDevice* create_wirelessdev();
typedef void destroy_wirelessdev(WirelessDevice*);

#endif
