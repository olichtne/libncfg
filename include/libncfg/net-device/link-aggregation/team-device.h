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

#ifndef __TEAM_DEVICE_H__
#define __TEAM_DEVICE_H__

#include <libncfg/common.h>
#include <libncfg/net-device/l-aggr-device.h>

class TeamDevice: public LAggrDevice{
    private:
        static const int type = TEAM_DEVICE;
    public:
        virtual const int get_device_type()
            {return TeamDevice::type;}
};

typedef TeamDevice* create_teamdev();
typedef void destroy_teamdev(TeamDevice*);

#endif
