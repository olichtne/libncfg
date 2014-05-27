/* main.c - example application for the LibNCFG library
 * Copyright (C) 2014 Ondrej Lichtner <ondrej.lichtner@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <netinet/in.h>
#include <linux/if.h>
#include <libncfg/libncfg.h>
#include <libncfg/common.h>
#include <libncfg/net-device/eth-device.h>
#include <libncfg/net-device/link-aggregation/bonding-device.h>
#include <iostream>
#include <vector>

using namespace std;

void help(char *prog_name)
{
    cout << "USAGE: " << prog_name << " COMMAND [ARGS]" << endl;
    cout << "commands" << endl;
    cout << "help                               print this help" << endl;
    cout << "list                               list all detected devices"
         << endl;
    cout << "add-device type TYPE [name NAME]   creates a new device with "
         << "the specified type and optional name" << endl;
    cout << "del-device NAME                    deletes the specified device"
         << endl;
    cout << "add-addr dev NAME ADDRESS          adds an IP address to the "
         << "specified device" << endl;
    cout << "del-addr dev NAME ADDRESS          removes an IP address from "
         << "the device" << endl;
    cout << "set dev NAME {up|down}             sets the specified device up "
         << "or down" << endl;
    cout << "set dev NAME hwaddr ADDRESS        sets a new hardware address "
         << "of the specified device" << endl;
    cout << "set dev NAME master MNAME          makes the device named MNANE "
         << "the master of the device named NAME" << endl;
    cout << "set dev NAME nomaster              frees the specified device "
         << "if it is enslaved" << endl;
    cout << "add-slave dev NAME SLAVE_NAME      adds slave SLAVE_NAME to the "
         << "device NAME" << endl;
    cout << "del-slave dev NAME SLAVE_NAME      removes the slave SLAVE_NAME "
         << "from the device NAME" << endl;
}

void list_devices()
{
    LibNCFG l;
    for (auto i : l.get_devices()){
        EthDevice *dev = (EthDevice *)l.get_device(i->get_if_index(), ETH_DEVICE);

        cout << dev->get_if_index() << ": " << dev->get_name() << endl;
        switch (dev->get_state()){
            case IF_OPER_UP:
                cout << "\tstate: UP" << endl;
                break;
            case IF_OPER_DOWN:
                cout << "\tstate: DOWN" << endl;
                break;
            default:
            case IF_OPER_UNKNOWN:
                cout << "\tstate: UNKNOWN" << endl;
                break;
        }

        cout << "\thwaddr " << dev->get_hw_addr() << endl;
        cout << "\tmtu " << dev->get_mtu() << endl;

        for (auto j : dev->get_ip_addr_list())
            if (j.get_family_type() == AF_INET)
                cout << "\tinet " << j << "/" << j.get_prefix_len() << endl;
            else if (j.get_family_type() == AF_INET6)
                cout << "\tinet6 " << j << "/" << j.get_prefix_len() << endl;

        NetDevice *master = dev->get_master();
        if (master != NULL){
            cout << "\tmaster " << master->get_name() << endl;
        }
    }
    return;
}

void add_device(string type, string name=""){
    if (type.compare("bond") == 0){
        LibNCFG l;
        NetDevice *dev;
        if (name == "")
            dev = l.create_soft_device(BOND_DEVICE);
        else
            dev = l.create_soft_device(name, BOND_DEVICE);
        l.dealloc_device(dev);
    }else
        return;
}

void del_device(string name){
    LibNCFG l;
    NetDevice *dev = NULL;
    dev = l.get_device(name);
    if (dev != NULL){
        int res = 0;
        try{
            res = l.destroy_soft_device(dev);
            if (res != 0)
                cerr << "Failed to remove device " << name << ".\n";
        }catch (MethodNotImplemented e){
            cerr << "Can't remove device " << name << ".\n";
        }
        l.dealloc_device(dev);
    }else
        cerr << "No device named " << name << " found.\n";
}

void add_addr(string devname, string ipaddr){
    LibNCFG l;
    NetDevice *dev = NULL;
    IpAddr addr(ipaddr);
    dev = l.get_device(devname);
    if (dev != NULL){
        int res = 0;
        res = dev->add_ip_addr(addr);
        if (res != 0)
            cerr << "Failed to add address " << addr << "\n";
        l.dealloc_device(dev);
    }else
        cerr << "No device named " << devname << " found.\n";
}

void del_addr(string devname, string ipaddr){
    LibNCFG l;
    NetDevice *dev = NULL;
    IpAddr addr(ipaddr);
    dev = l.get_device(devname);
    if (dev != NULL){
        int res = 0;
        res = dev->del_ip_addr(addr);
        if (res != 0)
            cerr << "Failed to remove address " << addr << "\n";
        l.dealloc_device(dev);
    }else
        cerr << "No device named " << devname << " found.\n";
}

void set(int argc, char **argv){
    if (argc < 3 ||
            string(argv[0]).compare("dev")){
        cerr << "Invalid arguments!\n";
        help(argv[0]);
        return;
    }
    string devname = string(argv[1]);
    LibNCFG l;
    EthDevice *dev = NULL;
    dev = (EthDevice *)l.get_device(devname, ETH_DEVICE);
    if (dev == NULL){
        cerr << "No device named " << devname << " found.\n";
        return;
    }

    int res;
    if (string(argv[2]).compare("up") == 0){
        res = dev->set_up();
        if (res != 0)
            cerr << "Failed to set device " << devname << " up.\n";
        return;
    }else if (string(argv[2]).compare("down") == 0){
        res = dev->set_down();
        if (res != 0)
            cerr << "Failed to set device " << devname << " down.\n";
        return;
    }else if (string(argv[2]).compare("hwaddr") == 0){
        if (argc != 4){
            cerr << "Invalid set arguments!\n";
            return;
        }
        HwAddr addr(argv[3]);
        res = dev->set_hw_addr(addr);
        if (res != 0)
            cerr << "Failed to set new hwaddr for device " << devname << ".\n";
    }else if (string(argv[2]).compare("master") == 0){
        if (argc != 4){
            cerr << "Invalid set arguments!\n";
            return;
        }
        string master_name = string(argv[3]);
        res = dev->set_master(master_name);
        if (res != 0)
            cerr << "Failed to set master for device " << devname << ".\n";
    }else if (string(argv[2]).compare("nomaster") == 0){
        if (argc != 3){
            cerr << "Invalid set arguments!\n";
            return;
        }
        res = dev->set_master("");
        if (res != 0)
            cerr << "Failed to free device " << devname << ".\n";
    }else{
        cerr << "Unrecognized set option!\n";
        return;
    }
}

void add_slave(string devname, string slavename){
    LibNCFG l;
    EthDevice *dev = (EthDevice *)l.get_device(devname);
    if (dev == NULL){
        cerr << "No device " << devname << " found.\n";
        return;
    }else{
        l.dealloc_device(dev);
        dev = NULL;
    }

    dev = (EthDevice *)l.get_device(slavename, ETH_DEVICE);
    if (dev != NULL){
        dev->set_master(devname);
        l.dealloc_device(dev);
    }else{
        cerr << "No device " << slavename << " found.\n";
        return;
    }
}

void del_slave(string devname, string slavename){
    LibNCFG l;
    EthDevice *dev = (EthDevice *)l.get_device(devname);
    NetDevice *master = NULL;
    if (dev == NULL){
        cerr << "No device " << devname << " found.\n";
        return;
    }else{
        l.dealloc_device(dev);
        dev = NULL;
    }

    dev = (EthDevice *)l.get_device(slavename, ETH_DEVICE);
    if (dev != NULL){
        master = dev->get_master();
        if (master != NULL){
            if (master->get_name().compare(devname) == 0){
                dev->set_master("");
            }else{
                cerr << "Device " << devname << " is not the master of " << slavename << ".\n";
            }
            l.dealloc_device(master);
        }else{
            cerr << "Device " << slavename << " has no master.\n";
        }
        l.dealloc_device(dev);
    }else{
        cerr << "No device " << slavename << " found.\n";
        return;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2){
        cerr << "Not enough arguments!\n";
        help(argv[0]);
        return 1;
    }

    string command(argv[1]);
    if (command.compare("help") == 0){
        help(argv[0]);
    }else if (command.compare("list") == 0){
        list_devices();
    }else if (command.compare("add-device") == 0){
        if (argc == 4 &&
            string(argv[2]).compare("type") == 0)
            add_device(argv[3]);
        else if(argc == 6 &&
                string(argv[2]).compare("type") == 0 &&
                string(argv[4]).compare("name") == 0)
            add_device(argv[3], argv[5]);
        else{
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
    }else if (command == "del-device"){
        if (argc != 3){
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
        del_device(argv[2]);
    }else if (command == "add-addr"){
        if (argc != 5 &&
                string(argv[2]).compare("dev") != 0){
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
        add_addr(argv[3], argv[4]);
    }else if (command == "del-addr"){
        if (argc != 5 &&
                string(argv[2]).compare("dev") != 0){
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
        del_addr(argv[3], argv[4]);
    }else if (command == "set"){
        set(argc-2, argv+2);
    }else if (command == "add-slave"){
        if (argc != 5 &&
                string(argv[2]).compare("dev") != 0){
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
        add_slave(argv[3], argv[4]);
    }else if (command == "del-slave"){
        if (argc != 5 &&
                string(argv[2]).compare("dev") != 0){
            cerr << "Invalid arguments!\n";
            help(argv[0]);
            return 1;
        }
        del_slave(argv[3], argv[4]);
    }else{
        cerr << "Unknown command!\n";
        help(argv[1]);
        return 1;
    }
    return 0;
}
