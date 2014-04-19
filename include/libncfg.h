#ifndef __LIBNCFG_H__
#define __LIBNCFG_H__

#include <vector>
#include <string>

#include <net-device.h>

class LibNCFG{
    private:
        std::vector<NetDevice *> devices;

    public:
        LibNCFG();
        std::vector<NetDevice *> get_devices();
        NetDevice *get_device(std::string name);
        NetDevice *get_device(int if_id);

        NetDevice *create_soft_device(int type);
        NetDevice *create_soft_device(int type, std::string name);
        void destroy_soft_device(NetDevice *device);
        void destroy_soft_device(std::string name);
        void destroy_soft_device(int if_id);

        int define_callback(void *(*func)(int event, NetDevice *source), int event);
        void spin_callbacks();

        //add tables
};

#endif
