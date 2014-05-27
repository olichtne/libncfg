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

#include <libncfg/common.h>
#include <libncfg/libncfg.h>
#include <libncfg/net-device.h>

class NetlinkNetDevice: public NetDevice{
    private:
        LibNCFG *lib;

        std::string name;
        int if_index;
        int state;
        std::vector<IpAddr> ip_addresses;

        struct mnl_socket *nl_sock;

        int nl_set_flags(short flags);
        int nl_clr_flags(short flags);
        int ip_operation(IpAddr &addr, int msg_flags, int operation);
    protected:
        void initialize(int if_index, std::string name, LibNCFG *lib);
        int destroy();
        void update();
    public:
        NetlinkNetDevice();
        ~NetlinkNetDevice();

        bool is_up();
        bool is_down();
        int set_up();
        int set_down();
        int get_state();

        int get_if_index();

        std::string get_name();
        int set_name(std::string name);

        std::vector<IpAddr> get_ip_addr_list();
        int add_ip_addr(IpAddr &addr);
        int change_ip_addr(IpAddr &addr);
        int del_ip_addr(IpAddr &addr);
};
