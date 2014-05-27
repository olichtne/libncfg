#include <iostream>
#include <libmnl/libmnl.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <cstring>

#include <libncfg/common.h>
#include "netlinkethdevice.h"

extern "C" NetlinkEthDevice *create_netlinkethdevice(){
    return new NetlinkEthDevice();
}

extern "C" void destroy_netlinkethdevice(NetlinkEthDevice *dev){
    delete dev;
}

extern "C" dev_type type_netlinkethdevice(){
    return ETH_DEVICE;
}

NetlinkEthDevice::NetlinkEthDevice(){
    this->nl_sock = mnl_socket_open(NETLINK_ROUTE);
    mnl_socket_bind(this->nl_sock, 0, MNL_SOCKET_AUTOPID);

    int sock_fd = mnl_socket_get_fd(this->nl_sock);
    fcntl(sock_fd, F_SETFL, O_NONBLOCK);

    this->master = NULL;
    this->parent_class = NULL;
    this->lib = NULL;
}

NetlinkEthDevice::~NetlinkEthDevice(){
    mnl_socket_close(this->nl_sock);
    if (this->lib != NULL)
        this->lib->dealloc_device(this->parent_class);
    this->parent_class = NULL;
}

void NetlinkEthDevice::initialize(int if_index, std::string name, LibNCFG *lib){
    this->if_index = if_index;
    this->lib = lib;
    this->update();

    this->parent_class = this->lib->get_device(if_index, NET_DEVICE, IF_NETLINK);
}

void NetlinkEthDevice::update(){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    struct rtattr *attr = NULL;
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_GETLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*)mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == RTM_NEWLINK){
        payload_len = mnl_nlmsg_get_payload_len(nlh);
        ifinfo = (struct ifinfomsg *)NLMSG_DATA(nlh);

        this->flags = ifinfo->ifi_flags;

        attr = IFLA_RTA(ifinfo);
        payload_len -= sizeof(ifinfo);

        bool master_set = false;
        while (RTA_OK(attr, payload_len)){
            switch (attr->rta_type){
                case IFLA_IFNAME:
                    this->name = std::string((char *)RTA_DATA(attr));
                    break;
                case IFLA_OPERSTATE:
                    this->state = *((uint8_t *)RTA_DATA(attr));
                    break;
                case IFLA_ADDRESS:
                    this->hw_address = HwAddr((char*)RTA_DATA(attr), RTA_PAYLOAD(attr), MAC_ADDR);
                    break;
                case IFLA_MTU:
                    this->mtu = mnl_attr_get_u32((struct nlattr *)attr);
                    break;
                case IFLA_TXQLEN:
                    this->tx_queue_len = mnl_attr_get_u32((struct nlattr *)attr);
                    break;
                case IFLA_MASTER:
                    if(this->master != NULL)
                        this->lib->dealloc_device(this->master);
                    this->master = this->lib->get_device(mnl_attr_get_u32((struct nlattr *)attr));
                    master_set = true;
                    break;
                default:
                    break;
            }
            attr = RTA_NEXT(attr, payload_len);
        }
        if (!master_set){
            if (this->master != NULL)
                this->lib->dealloc_device(this->master);
            this->master = NULL;
        }
    }
    return;
}

int NetlinkEthDevice::get_if_index(){
    this->update();
    return this->if_index;
}

std::string NetlinkEthDevice::get_name(){
    this->update();
    return this->name;
}

int NetlinkEthDevice::set_name(std::string new_name){
    return this->parent_class->set_name(new_name);
}

std::vector<IpAddr> NetlinkEthDevice::get_ip_addr_list(){
    return this->parent_class->get_ip_addr_list();
}

int NetlinkEthDevice::add_ip_addr(IpAddr &addr){
    return this->parent_class->add_ip_addr(addr);
}

int NetlinkEthDevice::change_ip_addr(IpAddr &addr){
    return this->parent_class->change_ip_addr(addr);
}

int NetlinkEthDevice::del_ip_addr(IpAddr &addr){
    return this->parent_class->del_ip_addr(addr);
}

bool NetlinkEthDevice::is_up()
{
    return this->parent_class->is_up();
}

bool NetlinkEthDevice::is_down()
{
    return this->parent_class->is_down();
}

int NetlinkEthDevice::set_up()
{
    return this->parent_class->set_up();
}

int NetlinkEthDevice::set_down()
{
    return this->parent_class->set_down();
}

int NetlinkEthDevice::get_state()
{
    return this->parent_class->get_state();
}

HwAddr NetlinkEthDevice::get_hw_addr(){
    this->update();
    return this->hw_address;
}

int NetlinkEthDevice::set_hw_addr(HwAddr hw_addr){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;

    mnl_attr_put(nlh, IFLA_ADDRESS, hw_addr.get_length(), hw_addr.get_addr());

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return 0;
}

int NetlinkEthDevice::nl_set_flags(short flags){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;
    ifinfo->ifi_flags |= flags;

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return -1;
}

int NetlinkEthDevice::nl_clr_flags(short flags){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;
    ifinfo->ifi_flags &= ~flags;

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return -1;
}

int NetlinkEthDevice::get_arp(){
    this->update();
    return !(this->flags & IFF_NOARP);
}

int NetlinkEthDevice::set_arp(bool val){
    if (val)
        return this->nl_clr_flags(IFF_NOARP);
    else
        return this->nl_set_flags(IFF_NOARP);
}

int NetlinkEthDevice::get_multicast(){
    this->update();
    return this->flags & IFF_MULTICAST;
}

int NetlinkEthDevice::set_multicast(bool val){
    if (val)
        return this->nl_set_flags(IFF_MULTICAST);
    else
        return this->nl_clr_flags(IFF_MULTICAST);
}

int NetlinkEthDevice::get_dynamic(){
    this->update();
    return this->flags & IFF_DYNAMIC;
}

int NetlinkEthDevice::set_dynamic(bool val){
    if (val)
        return this->nl_set_flags(IFF_DYNAMIC);
    else
        return this->nl_clr_flags(IFF_DYNAMIC);
}

unsigned int NetlinkEthDevice::get_mtu(){
    this->update();
    return this->mtu;
}

int NetlinkEthDevice::set_mtu(unsigned int val){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;

    mnl_attr_put(nlh, IFLA_MTU, sizeof(unsigned int), &val);

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return 0;
}

unsigned int NetlinkEthDevice::get_tx_queue_len(){
    this->update();
    return this->tx_queue_len;
}

int NetlinkEthDevice::set_tx_queue_len(unsigned int val){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;

    mnl_attr_put(nlh, IFLA_TXQLEN, sizeof(unsigned int), &val);

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return 0;
}

int NetlinkEthDevice::set_master(NetDevice *master){
    int master_if_id = 0;
    if (master != NULL)
        master_if_id = master->get_if_index();
    return this->set_master(master_if_id);
}

int NetlinkEthDevice::set_master(int if_index){
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;
    ifinfo->ifi_change = 0xFFFFFFFF;

    mnl_attr_put(nlh, IFLA_MASTER, sizeof(int), &if_index);

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
    if (ret <= 0){
        perror("mnl_socket_recvfrom");
        return -1;
    }

    nlh = (struct nlmsghdr *)buf;
    if(NLMSG_OK(nlh, ret) && nlh->nlmsg_type == NLMSG_ERROR){
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        return err->error;
    }
    return 0;
}

int NetlinkEthDevice::set_master(std::string name){
    int res = -1;
    NetDevice *master = this->lib->get_device(name);
    if (master != NULL){
        res = this->set_master(master->get_if_index());
        this->lib->dealloc_device(master);
    }else{
        res = this->set_master(0);
    }
    return res;
}

NetDevice *NetlinkEthDevice::get_master(){
    this->update();
    return this->master;
}
