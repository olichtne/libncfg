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

#ifndef __LIBNCFG_H__
#define __LIBNCFG_H__

#include <vector>
#include <string>
#include <map>

#include <libncfg/net-device.h>
#include <libncfg/table.h>

struct devclass_interface{
    create_netdev *create = NULL;
    destroy_netdev *destroy = NULL;
    dev_type type = NET_DEVICE;
    void *handle = NULL;
};

struct tableclass_interface{
    //TODO
};

const std::map<std::string, int> if_type_mapping = {
    {"netlink", IF_NETLINK},
    {"sysfs", IF_SYSFS},
    {"ioctl", IF_IOCTL}
};

class LibNCFG{
    private:
        //[if_type][class_type]
        std::map<int, std::map<int, struct devclass_interface> > devclass_mapping;
        std::map<int, std::map<int, struct tableclass_interface> > tableclass_mapping;
        std::vector<NetDevice *> devices;

        std::vector<int> if_type_order = {
            IF_NETLINK,
            IF_IOCTL,
            IF_SYSFS
        };
        std::map<int, callback_func> event_callbacks;

        struct mnl_socket *nl_sock;

        void nl_reload_devices();
        struct devclass_interface *get_dev_class(int dev_type);
        NetDevice *new_NetDevice(struct ifinfomsg *, int type = NET_DEVICE);
        NetDevice *new_NetDevice(int if_index, std::string name, int type = NET_DEVICE);
    public:
        LibNCFG();
        ~LibNCFG();
        std::vector<NetDevice *> get_devices(int if_type = -1);
        NetDevice *get_device(std::string name, int type = NET_DEVICE, int if_type = -1);
        NetDevice *get_device(int if_id, int type = NET_DEVICE, int if_type = -1);

        void dealloc_device(NetDevice *device);

        NetDevice *create_soft_device(int type, int if_type = -1);
        NetDevice *create_soft_device(std::string name, int type, int if_type = -1);
        int destroy_soft_device(NetDevice *device);
        int destroy_soft_device(std::string name);
        int destroy_soft_device(int if_id);

        int register_event(int type, callback_func func);
        int unregister_event(int type);
        void check_events();

        int set_if_type_order(std::vector<int> order); //TODO

        //add tables
        Table get_table(int table_type, std::string name, int if_type = -1);
};

#endif
