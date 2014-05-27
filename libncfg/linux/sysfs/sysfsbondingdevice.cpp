#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <regex>

#include <netinet/in.h>
#include <linux/if.h>

#include <libncfg/common.h>
#include "sysfsbondingdevice.h"

extern "C" sysfsBondingDevice *create_sysfsbondingdevice()
{
    return new sysfsBondingDevice();
}

extern "C" void destroy_sysfsbondingdevice(sysfsBondingDevice *dev)
{
    delete dev;
}

extern "C" dev_type type_sysfsbondingdevice()
{
    return BOND_DEVICE;
}

sysfsBondingDevice::sysfsBondingDevice()
{
    this->active_slave = NULL;
    this->primary = NULL;
    this->name = "";
    this->if_index = -1;
    this->lib = NULL;
    this->parent_class = NULL;
}

sysfsBondingDevice::~sysfsBondingDevice()
{
    if (this->parent_class != NULL)
        this->lib->dealloc_device(this->parent_class);
    if (this->active_slave != NULL)
        this->lib->dealloc_device(this->active_slave);
    if (this->primary != NULL)
        this->lib->dealloc_device(this->primary);
    return;
}

void sysfsBondingDevice::initialize(int if_index, std::string name, LibNCFG *lib)
{
    this->name = name;
    this->lib = lib;
    this->dev_path = "/sys/class/net/" + this->name + "/";

    this->parent_class = this->lib->get_device(name, NET_DEVICE);
}

int sysfsBondingDevice::create()
{
    FILE *bonding_masters = NULL;
    bonding_masters = fopen("/sys/class/net/bonding_masters", "w");
    if (bonding_masters == NULL)
        return -1;

    int res = fprintf(bonding_masters, "+%s", this->name.c_str());

    fclose(bonding_masters);
    if (res > -1)
        return 0;
    else
        return -1;
}

int sysfsBondingDevice::destroy()
{
    FILE *bonding_masters = NULL;
    bonding_masters = fopen("/sys/class/net/bonding_masters", "w");
    if (bonding_masters == NULL)
        return -1;

    int res = fprintf(bonding_masters, "-%s", this->name.c_str());

    fclose(bonding_masters);
    if (res > -1)
        return 0;
    else
        return -1;
}

int sysfsBondingDevice::read_int_file(std::string filename)
{
    std::string filepath = this->dev_path + filename;
    std::ifstream f(filepath);
    std::string line;

    std::getline(f, line);

    return std::stoi(line);
}

std::string sysfsBondingDevice::read_file_line(std::string filename)
{
    std::string filepath = this->dev_path + filename;
    std::ifstream f(filepath);
    std::string line;

    std::getline(f, line);
    return line;
}

int sysfsBondingDevice::write_to_file(std::string filename, std::string value)
{
    std::string filepath = this->dev_path + filename;
    std::ofstream f(filepath);

    f << value;
    return 0;
}

void sysfsBondingDevice::update()
{
    this->name = this->parent_class->get_name();
    this->dev_path = "/sys/class/net/" + this->name + "/";
    this->if_index = this->read_int_file("ifindex");
    this->arp_interval = this->read_int_file("bonding/arp_interval");
    this->min_links = this->read_int_file("bonding/min_links");

    std::regex reg_str_int("^([^ ]*) (\\d*)$");
    std::smatch sm;
    std::string tmp_line;

    tmp_line = this->read_file_line("bonding/mode");
    std::regex_match(tmp_line, sm, reg_str_int);
    this->mode_str = sm[1];
    this->mode = std::stoi(sm[2]);

    tmp_line = this->read_file_line("bonding/fail_over_mac");
    std::regex_match(tmp_line, sm, reg_str_int);
    this->fail_over_mac_str = sm[1];
    this->fail_over_mac = std::stoi(sm[2]);

    tmp_line = this->read_file_line("bonding/active_slave");
    if (tmp_line.size() > 0){
        if (this->active_slave != NULL)
            this->lib->dealloc_device(this->active_slave);
        this->active_slave = this->lib->get_device(tmp_line);
    }

    tmp_line = this->read_file_line("bonding/primary");
    if (tmp_line.size() > 0){
        if (this->primary != NULL)
            this->lib->dealloc_device(this->primary);
        this->primary = this->lib->get_device(tmp_line);
    }

    tmp_line = this->read_file_line("bonding/slaves");
    for (auto i : this->slaves)
        this->lib->dealloc_device(i);
    this->slaves.clear();
    int space_pos;
    std::string slave_name = "";
    while (tmp_line.size() > 0){
        space_pos = tmp_line.find(" ");
        slave_name = tmp_line.substr(0, space_pos);
        this->slaves.push_back(this->lib->get_device(slave_name));
        if (space_pos != std::string::npos)
            tmp_line = tmp_line.substr(space_pos+1);
        else
            tmp_line = "";
    }
}

int sysfsBondingDevice::get_if_index()
{
    this->update();
    return this->if_index;
}

std::string sysfsBondingDevice::get_name()
{
    this->update();
    return this->name;
}

int sysfsBondingDevice::set_name(std::string name)
{
    int res = -1;
    if (this->parent_class != NULL)
        res = this->parent_class->set_name(name);
    if (res == 0)
        this->name = name;
    return res;
}


std::vector<IpAddr> sysfsBondingDevice::get_ip_addr_list()
{
    if (this->parent_class != NULL)
        return this->parent_class->get_ip_addr_list();
    return std::vector<IpAddr>();
}

int sysfsBondingDevice::add_ip_addr(IpAddr &addr)
{
    if (this->parent_class != NULL)
        return this->parent_class->add_ip_addr(addr);
    return -1;
}

int sysfsBondingDevice::change_ip_addr(IpAddr &addr)
{
    if (this->parent_class != NULL)
        return this->parent_class->change_ip_addr(addr);
    return -1;
}

int sysfsBondingDevice::del_ip_addr(IpAddr &addr)
{
    if (this->parent_class != NULL)
        return this->parent_class->del_ip_addr(addr);
    return -1;
}

bool sysfsBondingDevice::is_up()
{
    this->update();
    if (this->state.compare("up") == 0)
        return true;
}

bool sysfsBondingDevice::is_down()
{
    this->update();
    if (this->state.compare("down") == 0)
        return true;
}

int sysfsBondingDevice::set_up()
{
    if (this->parent_class != NULL)
        return this->parent_class->set_up();
    return -1;
}

int sysfsBondingDevice::set_down()
{
    if (this->parent_class != NULL)
        return this->parent_class->set_down();
    return -1;
}

int sysfsBondingDevice::get_state()
{
    this->update();
    if (this->state.compare("up"))
        return IF_OPER_UP;
    else if (this->state.compare("down"))
        return IF_OPER_DOWN;
    else
        return IF_OPER_UNKNOWN;
}

int sysfsBondingDevice::add_slave(NetDevice *dev)
{
    if (dev == NULL)
        return -1;
    std::string devname = dev->get_name();
    this->write_to_file("bonding/slaves", "+" + devname);
    return 0;
}

int sysfsBondingDevice::del_slave(NetDevice *dev)
{
    if (dev == NULL)
        return -1;
    std::string devname = dev->get_name();
    this->write_to_file("bonding/slaves", "-" + devname);
    return 0;
}

std::vector<NetDevice *> sysfsBondingDevice::get_slaves()
{
    this->update();
    return this->slaves;
}

int sysfsBondingDevice::set_mode(int mode)
{
    this->write_to_file("bonding/mode", std::to_string(mode));
    return 0;
}

int sysfsBondingDevice::set_mode(std::string mode)
{
    this->write_to_file("bonding/mode", mode);
    return 0;
}

int sysfsBondingDevice::get_mode()
{
    this->update();
    return this->mode;
}

std::string sysfsBondingDevice::get_mode_string()
{
    this->update();
    return this->mode_str;
}

int sysfsBondingDevice::set_active_slave(NetDevice *dev)
{
    if (dev == NULL)
        return -1;
    std::string devname = dev->get_name();
    this->write_to_file("bonding/active_slave", devname);
    return 0;
}

NetDevice *sysfsBondingDevice::get_active_slave()
{
    this->update();
    return this->active_slave;
}

int sysfsBondingDevice::set_arp_interval(int interval)
{
    this->write_to_file("bonding/arp_interval", std::to_string(interval));
    return 0;
}

int sysfsBondingDevice::get_arp_interval()
{
    this->update();
    return this->arp_interval;
}

int sysfsBondingDevice::set_fail_over_mac(int val)
{
    this->write_to_file("bonding/fail_over_mac", std::to_string(val));
    return 0;
}

int sysfsBondingDevice::set_fail_over_mac(std::string val)
{
    this->write_to_file("bonding/fail_over_mac", val);
    return 0;
}

int sysfsBondingDevice::get_fail_over_mac()
{
    this->update();
    return this->fail_over_mac;
}

std::string sysfsBondingDevice::get_fail_over_mac_str()
{
    this->update();
    return this->fail_over_mac_str;
}

int sysfsBondingDevice::set_min_links(int val)
{
    this->write_to_file("bonding/min_links", std::to_string(val));
    return 0;
}

int sysfsBondingDevice::get_min_links()
{
    this->update();
    return this->min_links;
}

int sysfsBondingDevice::set_primary(NetDevice *dev)
{
    if (dev == NULL)
        return -1;
    std::string devname = dev->get_name();
    this->write_to_file("bonding/primary", devname);
    return 0;
}

NetDevice *sysfsBondingDevice::get_primary()
{
    this->update();
    return this->primary;
}
