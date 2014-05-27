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
