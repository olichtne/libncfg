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
