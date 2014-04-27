#ifndef __LIBNCFG_COMMON__
#define __LIBNCFG_COMMON__

#include <string>
#include <exception>

class IpAddr{
    private:
        std::string addr;
    public:
        IpAddr();
};

class MethodNotImplemented : std::exception{
    const char *what() const noexcept {return "Method not implemented!\n";}
};

enum dev_type{
    NET_DEVICE,
    ETH_DEVICE,
    LAGGR_DEVICE,
    BOND_DEVICE,
    TEAM_DEVICE,
    WIRELESS_DEVICE,
    WIFI_DEVICE,
    WIMAX_DEVICE
};

#endif
