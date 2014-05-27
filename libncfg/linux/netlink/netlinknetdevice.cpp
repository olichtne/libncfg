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

#include <iostream>
#include <libmnl/libmnl.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <cstring>

#include <libncfg/common.h>
#include "netlinknetdevice.h"

extern "C" NetlinkNetDevice *create_netlinknetdevice()
{
    return new NetlinkNetDevice();
}

extern "C" void destroy_netlinknetdevice(NetlinkNetDevice *dev)
{
    delete dev;
}

extern "C" dev_type type_netlinknetdevice()
{
    return NET_DEVICE;
}

NetlinkNetDevice::NetlinkNetDevice()
{
    this->nl_sock = mnl_socket_open(NETLINK_ROUTE);
    mnl_socket_bind(this->nl_sock, 0, MNL_SOCKET_AUTOPID);

    int sock_fd = mnl_socket_get_fd(this->nl_sock);
    fcntl(sock_fd, F_SETFL, O_NONBLOCK);
}

NetlinkNetDevice::~NetlinkNetDevice()
{
    mnl_socket_close(this->nl_sock);
}

void NetlinkNetDevice::initialize(int if_index, std::string name, LibNCFG *lib)
{
    this->if_index = if_index;
    this->lib = lib;
    this->update();
}

int NetlinkNetDevice::destroy()
{
    struct nlmsghdr *nlh = NULL;
    struct ifinfomsg *ifinfo;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_DELLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlh->nlmsg_seq = time(NULL);

    ifinfo = (struct ifinfomsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
    ifinfo->ifi_index = this->if_index;

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

void NetlinkNetDevice::update()
{
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

        attr = IFLA_RTA(ifinfo);
        payload_len -= sizeof(ifinfo);
        while (RTA_OK(attr, payload_len)){
            switch (attr->rta_type){
                case IFLA_IFNAME:
                    this->name = std::string((char *)RTA_DATA(attr));
                    break;
                case IFLA_OPERSTATE:
                    this->state = *((uint8_t *)RTA_DATA(attr));
                    break;
                default:
                    break;
            }
            attr = RTA_NEXT(attr, payload_len);
        }
    }

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_GETADDR;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nlh->nlmsg_seq = time(NULL);

    struct ifaddrmsg *ifaddr;
    ifaddr = (struct ifaddrmsg*)mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifaddrmsg));
    ifaddr->ifa_index = this->if_index;
    ifaddr->ifa_flags = 0;

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    this->ip_addresses.clear();
    bool done = false;
    while (!done){
        memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);
        ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
        if (ret <= 0){
            perror("mnl_socket_recvfrom");
            return;
        }else{
            nlh = (struct nlmsghdr *)buf;
            while (NLMSG_OK(nlh, ret)){
                switch(nlh->nlmsg_type){
                    case RTM_NEWADDR:
                        ifaddr = (struct ifaddrmsg*)NLMSG_DATA(nlh);

                        if (ifaddr->ifa_index != this->if_index)
                            break;

                        payload_len = mnl_nlmsg_get_payload_len(nlh);
                        attr = (struct rtattr*)((char *)ifaddr + NLMSG_ALIGN(sizeof(struct ifaddrmsg)));
                        while (RTA_OK(attr, payload_len)){
                            switch (attr->rta_type){
                                case IFA_ADDRESS:
                                    this->ip_addresses.push_back(IpAddr((char*)RTA_DATA(attr),
                                                                 ifaddr->ifa_prefixlen,
                                                                 ifaddr->ifa_family));
                                    break;
                                default:
                                    break;
                            }

                            attr = RTA_NEXT(attr, payload_len);
                        }
                        break;
                    case NLMSG_DONE:
                        done = true;
                        break;
                    default:
                        break;
                }
                nlh = NLMSG_NEXT(nlh, ret);
            }
        }
    }
    return;
}

bool NetlinkNetDevice::is_up(){
    this->update();
    return this->state == IF_OPER_UP;
}

bool NetlinkNetDevice::is_down(){
    this->update();
    return this->state == IF_OPER_DOWN;
}

int NetlinkNetDevice::get_state(){
    return this->state;
}

int NetlinkNetDevice::nl_set_flags(short flags){
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

int NetlinkNetDevice::nl_clr_flags(short flags){
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

int NetlinkNetDevice::set_up(){
    return this->nl_set_flags(IFF_UP);
}

int NetlinkNetDevice::set_down(){
    return this->nl_clr_flags(IFF_UP);
}

int NetlinkNetDevice::get_if_index(){
    this->update();
    return this->if_index;
}

std::string NetlinkNetDevice::get_name(){
    this->update();
    return this->name;
}

int NetlinkNetDevice::set_name(std::string new_name){
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

    mnl_attr_put(nlh, IFLA_IFNAME, new_name.size(), new_name.c_str());

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

std::vector<IpAddr> NetlinkNetDevice::get_ip_addr_list(){
    this->update();
    return this->ip_addresses;
}

int NetlinkNetDevice::ip_operation(IpAddr &addr,
        int msg_flags,
        int operation)
{
    struct nlmsghdr *nlh = NULL;
    struct ifaddrmsg *addrmsg;
    int ret;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];
    int payload_len = 0;

    memset(buf, 0, MNL_SOCKET_BUFFER_SIZE);

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= operation;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | msg_flags;
    nlh->nlmsg_seq = time(NULL);

    addrmsg = (struct ifaddrmsg*) mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifaddrmsg));
    addrmsg->ifa_index = this->if_index;
    addrmsg->ifa_family = addr.get_family_type();
    addrmsg->ifa_prefixlen = addr.get_prefix_len();
    addrmsg->ifa_flags = addr.get_flags();

    mnl_attr_put(nlh, IFA_LOCAL, addr.get_length(), (char *)addr.get_addr());

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

int NetlinkNetDevice::add_ip_addr(IpAddr &addr){
    int msg_flags = NLM_F_CREATE | NLM_F_EXCL;
    return this->ip_operation(addr, msg_flags, RTM_NEWADDR);
}

int NetlinkNetDevice::change_ip_addr(IpAddr &addr){
    int msg_flags = NLM_F_REPLACE;
    return this->ip_operation(addr, msg_flags, RTM_NEWADDR);
}

int NetlinkNetDevice::del_ip_addr(IpAddr &addr){
    return this->ip_operation(addr, 0, RTM_DELADDR);
}
