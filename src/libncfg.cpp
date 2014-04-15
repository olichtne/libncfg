#include <libncfg.h>
#include <iostream>

LibNCFG::LibNCFG()
{
    std::cout << "LibNCFG constructor!\n";
    //init - find and load dynamically loaded modules
}

std::vector<NetDevice *> LibNCFG::get_devices()
{
    //returns a vector of all network devices currently on the system
    return this->devices;
}

NetDevice * LibNCFG::get_device(std::string name)
{
    //returns the network device with the specified name
}

NetDevice * LibNCFG::get_device(int if_id)
{
    //returns the network device with the specified interface id
}

NetDevice * LibNCFG::create_soft_device(int type)
{
    //creates a device of the specified type, automatically generates a name
}

NetDevice * LibNCFG::create_soft_device(int type, std::string name)
{
    //creates a device of the specified type, with the specified name
}

void LibNCFG::destroy_soft_device(NetDevice *device)
{
    //destroys the device
}

void LibNCFG::destroy_soft_device(std::string name)
{
    //destroys the device
}

void LibNCFG::destroy_soft_device(int if_id)
{
    //destroys the device
}

int LibNCFG::define_callback(void *(*func)(int event, NetDevice *source),
                             int event)
{
    //registers a callback function that can be called when an event is catched
    //the events are only detected when the method spin_callbacks is called
}

void LibNCFG::spin_callbacks()
{
    //updates all devices and detects events, will call a callback function if defined
}
