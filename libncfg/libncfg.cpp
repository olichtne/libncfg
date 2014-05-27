#include <dlfcn.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <regex>
#include <libncfg/libncfg.h>
#include <libncfg/common.h>
#include <libncfg/net-device.h>
#include <libncfg/net-device/eth-device.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <libmnl/libmnl.h>
#include <linux/if.h>
#include <linux/if_link.h>

#define LIB_PATH "libncfg/linux/"

using namespace std;

LibNCFG::LibNCFG()
{
    //init - find and load dynamically loaded modules
    struct dirent *if_entry;
    struct dirent *entry;
    DIR *lib_dir;
    DIR *if_dir;

    lib_dir = opendir(LIB_PATH);
    map<int, vector<string> > if_types;
    regex lib_regex(".*\\.so");
    if (lib_dir != NULL){
        while ((if_entry = readdir(lib_dir))){
            int if_type;
            try{
                if_type = if_type_mapping.at(string(if_entry->d_name));
            }catch (out_of_range e){
                if_type = IF_UNDEFINED;
            }
            if (if_type != IF_UNDEFINED){
                string if_path = LIB_PATH + string(if_entry->d_name);
                if_dir = opendir(if_path.c_str());
                if (if_dir != NULL){
                    while ((entry = readdir(if_dir))){
                        if (regex_match(entry->d_name, lib_regex)){
                            if_types[if_type].push_back(if_path + "/" + string(entry->d_name));
                        }
                    }
                }
            }
        }
    }else{
        cerr << "first dp null\n";
    }


    for (auto i : if_types){
        for (auto file : i.second){
            string tmp = file;
            tmp.erase(0, tmp.rfind("/")+1); //remove path
            tmp.erase(0, 3); //remove "lib"
            tmp.erase(tmp.length()-3); //remove ".so"

            string create_funcname = "create_" + tmp;
            string destroy_funcname = "destroy_" + tmp;
            string type_funcname = "type_" + tmp;

            devclass_interface dev_if;
            dev_if.handle = dlopen(file.c_str(), RTLD_NOW);
            if (dev_if.handle == NULL){
                cerr << dlerror() << endl;
                continue;
            }
            dev_if.create = (create_netdev*)dlsym(dev_if.handle,
                    create_funcname.c_str());
            dev_if.destroy = (destroy_netdev*)dlsym(dev_if.handle,
                    destroy_funcname.c_str());

            type_netdev *type_func = (type_netdev*)dlsym(dev_if.handle,
                    type_funcname.c_str());
            dev_if.type = type_func();

            if (this->devclass_mapping[i.first].count(type_func()) == 0)
                this->devclass_mapping[i.first][type_func()] = dev_if;
        }
    }

    this->nl_sock = mnl_socket_open(NETLINK_ROUTE);
    mnl_socket_bind(this->nl_sock, 0, MNL_SOCKET_AUTOPID);

    int sock_fd = mnl_socket_get_fd(this->nl_sock);
    fcntl(sock_fd, F_SETFL, O_NONBLOCK);
}

LibNCFG::~LibNCFG(){
    for (auto i : this->devices){
        this->dealloc_device(i);
    }
    this->devices.clear();

    for (auto i : this->devclass_mapping){
        for (auto j : i.second){
            dlclose(j.second.handle);
        }
        i.second.clear();
    }
    this->devclass_mapping.clear();

    mnl_socket_close(this->nl_sock);
}

void LibNCFG::nl_reload_devices(){
    struct nlmsghdr *nlh;
    struct rtgenmsg *rt;
    int ret;
    unsigned int seq, portid;
    unsigned char buf[MNL_SOCKET_BUFFER_SIZE];

    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type	= RTM_GETLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nlh->nlmsg_seq = seq = time(NULL);
    rt = (struct rtgenmsg*)mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg));
    rt->rtgen_family = AF_PACKET;
    portid = mnl_socket_get_portid(this->nl_sock);

    mnl_socket_sendto(this->nl_sock, nlh, nlh->nlmsg_len);

    bool done = false;
    while (!done){
        ret = mnl_socket_recvfrom(this->nl_sock, buf, sizeof(buf));
        if (ret <= 0){
            perror("mnl_socket_recvfrom");
            break;
        }else{
            struct nlmsghdr *hdr = (struct nlmsghdr *)buf;
            struct rtattr *attr = NULL;
            int payload_len = 0;
            struct ifinfomsg *ifinfo;
            struct nl_msg *msg;
            NetDevice *dev;
            string if_name;
            while(NLMSG_OK(hdr, ret) && !done){
                if_name = "";
                dev = NULL;
                switch (hdr->nlmsg_type){
                    case NLMSG_DONE:
                        done = true;
                        break;
                    case RTM_NEWLINK:
                        //payload_len = mnl_nlmsg_get_payload_len(hdr);
                        ifinfo = (struct ifinfomsg *)NLMSG_DATA(hdr);
                        //attr = IFLA_RTA(ifinfo);
                        //payload_len -= sizeof(ifinfo);
                        //while (RTA_OK(attr, payload_len)){
                            //switch (attr->rta_type){
                                //case IFLA_IFNAME:
                                    //if_name = string((char *)RTA_DATA(attr), RTA_PAYLOAD(attr));
                                    //break;
                                //default:
                                    //break;
                            //}
                            //attr = RTA_NEXT(attr, payload_len);
                        //}

                        dev = NULL;
                        for (auto i : this->devices){
                            if (i == NULL)
                                continue;
                            if (i->get_if_index() == ifinfo->ifi_index){
                                dev = i;
                                break;
                            }
                        }

                        if(dev == NULL){
                            dev = this->new_NetDevice(ifinfo);
                            this->devices.push_back(dev);
                        }
                        break;
                    default:
                        break;
                }
                hdr = NLMSG_NEXT(hdr, ret);
            }
        }
    }
    for (auto i : this->devices){
        if (i == NULL)
            continue;
    }
}

struct devclass_interface *LibNCFG::get_dev_class(int type){
    struct devclass_interface *dev_class;
    for (auto i : if_type_order){
        try{
            dev_class = &(this->devclass_mapping[i].at(type));
            return dev_class;
        }catch (out_of_range e){}
    }
    return NULL;
}

NetDevice *LibNCFG::new_NetDevice(struct ifinfomsg *ifinfo, int type){
    //get class type
    struct devclass_interface *dev_class;
    dev_class = this->get_dev_class(type);
    if (dev_class == NULL)
        return NULL;
    //create new object

    NetDevice *dev;
    dev = dev_class->create();

    //initialize the object
    dev->initialize(ifinfo->ifi_index, "", this);
    dev->update();
    return dev;
}

NetDevice *LibNCFG::new_NetDevice(int if_index, string name, int type){
    //get class type
    struct devclass_interface *dev_class;
    dev_class = this->get_dev_class(type);
    if (dev_class == NULL)
        return NULL;
    //create new object

    NetDevice *dev;
    dev = dev_class->create();

    //initialize the object
    dev->initialize(if_index, name, this);
    dev->update();
    return dev;
}

std::vector<NetDevice *> LibNCFG::get_devices(int if_type)
{
    this->nl_reload_devices();
    return this->devices;
}

NetDevice *LibNCFG::get_device(std::string name, int type, int if_type)
{
    this->nl_reload_devices();
    NetDevice *dev = NULL;
    //search for the specified device
    for (auto i : this->devices){
        if (i->get_name() == name)
            dev = i;
    }

    if (dev != NULL){
        //create a new object for the user
        dev = this->new_NetDevice(dev->get_if_index(), dev->get_name(), type);
    }

    return dev;
}

NetDevice *LibNCFG::get_device(int if_id, int type, int if_type)
{
    this->nl_reload_devices();
    NetDevice *dev = NULL;
    //search for the specified device
    for (auto i : this->devices){
        if (i->get_if_index() == if_id)
            dev = i;
    }

    if (dev != NULL){
        //create a new object for the user
        dev = this->new_NetDevice(dev->get_if_index(), dev->get_name(), type);
    }

    return dev;
}

void LibNCFG::dealloc_device(NetDevice *device){
    if(device == NULL)
        return;
    int t = device->get_device_type();

    struct devclass_interface *dev_class;
    dev_class = this->get_dev_class(t);
    if (dev_class == NULL)
        return;

    dev_class->destroy(device);
}

NetDevice * LibNCFG::create_soft_device(int type, int if_type)
{
    //get class type
    struct devclass_interface *dev_class;
    dev_class = this->get_dev_class(type);
    if (dev_class == NULL)
        return NULL;

    //create new object
    NetDevice *dev;
    dev = dev_class->create();

    this->nl_reload_devices();
    string name = "";
    bool done = false;
    int i=0;
    while (!done){
        name = "dev_name" + to_string(i);
        done = true;
        for (auto i : this->devices){
            if (i->get_name() == name){
                done = false;
                break;
            }
        }
        ++i;
    }

    //initialize the object
    dev->initialize(-1, name, this);
    dev->create();
    this->nl_reload_devices();
    return dev;
}

NetDevice * LibNCFG::create_soft_device(std::string name, int type, int if_type)
{
    //get class type
    struct devclass_interface *dev_class;
    dev_class = this->get_dev_class(type);
    if (dev_class == NULL)
        return NULL;

    //create new object
    NetDevice *dev;
    dev = dev_class->create();

    //initialize the object
    dev->initialize(-1, name, this);
    dev->create();
    this->nl_reload_devices();
    return dev;
}

int LibNCFG::destroy_soft_device(NetDevice *device)
{
    if (device != NULL)
        return device->destroy();
    return -1;
}

int LibNCFG::destroy_soft_device(std::string name)
{
    for (auto i : this->devices){
        if (i->get_name() == name)
            return i->destroy();
    }
    return -1;
}

int LibNCFG::destroy_soft_device(int if_id)
{
    for (auto i : this->devices){
        if (i->get_if_index() == if_id)
            return i->destroy();
    }
    return -1;
}

int LibNCFG::register_event(int type,
                            callback_func func)
{
    //registers a callback function that can be called when an event is catched
    //the events are only detected when the method spin_callbacks is called
    return 0;
}

int unregister_event(int type)
{
    return 0;
}

void LibNCFG::check_events()
{
    //updates all devices and detects events, will call a callback function if defined
}
