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

#include <string>
#include <libncfg/common.h>
#include <libncfg/libncfg.h>
#include <libncfg/net-device.h>
#include <libncfg/net-device/link-aggregation/bonding-device.h>

class sysfsBondingDevice: public BondingDevice{
    private:
        std::string name;
        int if_index;
        LibNCFG *lib;
        NetDevice *parent_class;

        std::string dev_path;

        std::string state;

        std::vector<NetDevice *> slaves;

        int mode;
        std::string mode_str;
        int arp_interval;

        int fail_over_mac;
        std::string fail_over_mac_str;

        int min_links;
        NetDevice *active_slave;
        NetDevice *primary;

        int read_int_file(std::string filename);
        std::string read_file_line(std::string filename);
        int write_to_file(std::string filename, std::string value);
    protected:
        void initialize(int if_index, std::string name, LibNCFG *lib);
        int create();
        int destroy();
        void update();
    public:
        sysfsBondingDevice();
        ~sysfsBondingDevice();

        //NetDevice methods
        int get_if_index();

        std::string get_name();
        int set_name(std::string name);

        std::vector<IpAddr> get_ip_addr_list();
        int add_ip_addr(IpAddr &addr);
        int change_ip_addr(IpAddr &addr);
        int del_ip_addr(IpAddr &addr);

        bool is_up();
        bool is_down();
        int set_up();
        int set_down();
        int get_state();

        //LAggrDevice methods
        int add_slave(NetDevice *dev);
        int del_slave(NetDevice *dev);
        std::vector<NetDevice *> get_slaves();

        //BondingDevice methods
        int set_mode(int mode);
        int set_mode(std::string mode);
        int get_mode();
        std::string get_mode_string();

        int set_active_slave(NetDevice *dev);
        NetDevice *get_active_slave();

        int set_arp_interval(int interval);
        int get_arp_interval();

        int set_fail_over_mac(int val);
        int set_fail_over_mac(std::string val);
        int get_fail_over_mac();
        std::string get_fail_over_mac_str();

        int set_min_links(int val);
        int get_min_links();

        int set_primary(NetDevice *dev);
        NetDevice *get_primary();
};
