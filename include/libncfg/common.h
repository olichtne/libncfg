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

#ifndef __LIBNCFG_COMMON__
#define __LIBNCFG_COMMON__

#include <string>
#include <exception>
#include <iostream>

class IpAddr{
    private:
        char *addr;
        unsigned int prefix_len;
        int family_type;
        int flags;
    public:
        IpAddr();
        IpAddr(std::string str_addr);
        IpAddr(char *addr_bytes, unsigned int prefix_len, int type);
        IpAddr(const IpAddr &other);
        IpAddr& operator=(const IpAddr &other);
        ~IpAddr();

        unsigned int get_prefix_len();
        int set_prefix_len(unsigned int prefix_len); //TODO
        int get_family_type();
        int set_family_type(int family); //TODO
        char *get_addr();
        int set_addr(std::string str_addr); //TODO
        int set_addr(char *addr_bytes, int type); //TODO
        int get_length();
        int get_flags();
        int set_flags(int flags); //TODO
        int clear_flags(); //TODO
        int set_flag(int flag); //TODO
        int clear_flag(int flag); //TODO

        friend std::ostream& operator<<(std::ostream& os, const IpAddr& addr);
};

class HwAddr{
    private:
        char *addr;
        unsigned int len;
        int type;
    public:
        HwAddr();
        HwAddr(std::string);
        HwAddr(char *addr_bytes, unsigned int len, int type);
        HwAddr(const HwAddr& other);
        HwAddr& operator=(const HwAddr &other);
        ~HwAddr();

        unsigned int get_length();
        int get_type();
        int set_type(int type); //TODO
        char *get_addr();
        int set_addr(std::string str_addr); //TODO
        int set_addr(char *addr_bytes, unsigned int len, int type); //TODO

        friend std::ostream& operator<<(std::ostream& os, const HwAddr& addr);
};

class Event{
    //TODO event class
};

class MethodNotImplemented : std::exception{
    const char *what() const noexcept {return "Method not implemented!\n";}
};

enum class_type{
    DEV_CLASS = 0x100,
    TABLE_CLASS = 0x200
};

enum dev_type{
    NET_DEVICE = DEV_CLASS,
    ETH_DEVICE,
    INFINIBAND_DEVICE,
    LAGGR_DEVICE,
    BOND_DEVICE,
    TEAM_DEVICE,
    WIRELESS_DEVICE,
    WIFI_DEVICE,
    WIMAX_DEVICE
};

enum table_type{
    TABLE = TABLE_CLASS
};

enum if_type{
    IF_UNDEFINED,
    IF_NETLINK,
    IF_IOCTL,
    IF_SYSFS
};

enum hw_addr_type{
    MAC_ADDR,
};

enum event_type{
    NEW_DEVICE,
};

typedef void *(*callback_func)(int event, class NetDevice *source);

#endif
