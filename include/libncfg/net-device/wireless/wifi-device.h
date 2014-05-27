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

#ifndef __WIFI_DEVICE_H__
#define __WIFI_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/wireless-device.h>

class WiFiDevice: public WirelessDevice{
    private:
        static const int type = WIFI_DEVICE;
    public:
        virtual const int get_device_type()
            {return WiFiDevice::type;}
};

typedef WiFiDevice* create_wifidev();
typedef void destroy_wifidev(WiFiDevice*);

#endif
