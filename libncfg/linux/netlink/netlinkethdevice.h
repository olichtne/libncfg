#include <libncfg/common.h>
#include <libncfg/libncfg.h>
#include <libncfg/net-device.h>
#include <libncfg/net-device/eth-device.h>

class NetlinkEthDevice: public EthDevice{
    private:
        LibNCFG *lib;
        NetDevice *parent_class;

        enum dev_type type;
        std::string name;
        int if_index;
        int state;

        HwAddr hw_address;
        short flags;
        unsigned int mtu;
        unsigned int tx_queue_len;
        NetDevice *master;

        struct mnl_socket *nl_sock;

        int nl_set_flags(short flags);
        int nl_clr_flags(short flags);
    protected:
        void initialize(int if_index, std::string name, LibNCFG *lib);
        void update();
    public:
        NetlinkEthDevice();
        ~NetlinkEthDevice();

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

        //EthDevice methods
        HwAddr get_hw_addr();
        int set_hw_addr(HwAddr hw_addr);

        int get_arp();
        int set_arp(bool val);

        int get_multicast();
        int set_multicast(bool val);

        int get_dynamic();
        int set_dynamic(bool val);

        unsigned int get_mtu();
        int set_mtu(unsigned int val);

        unsigned int get_tx_queue_len();
        int set_tx_queue_len(unsigned int val);

        int set_master(NetDevice *master);
        int set_master(int if_index);
        int set_master(std::string name);
        NetDevice *get_master();
};
