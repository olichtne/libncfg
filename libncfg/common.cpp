#include <iostream>
#include <iomanip>
#include <libncfg/common.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <regex>

IpAddr::IpAddr(){
    this->addr = NULL;
    this->family_type = -1;
    this->prefix_len = 0;
    this->flags = 0;
}

IpAddr::IpAddr(char *addr_bytes, unsigned int prefix_len, int type){
    this->addr = NULL;

    if(type == AF_INET){
        if (prefix_len > 32)
            throw "Invalid address prefix!";
        this->addr = new char[4];
        memcpy(this->addr, addr_bytes, 4);
    }else if (type == AF_INET6){
        if (prefix_len > 128)
            throw "Invalid address prefix!";
        this->addr = new char[16];
        memcpy(this->addr, addr_bytes, 16);
    }else
        throw "Invalid address type!";

    this->family_type = type;
    this->prefix_len = prefix_len;
    this->flags = 0;
}

IpAddr::IpAddr(std::string str_addr){
    int pos;
    if (str_addr.find(".") != std::string::npos)
        this->family_type = AF_INET;
    else if (str_addr.find(":") != std::string::npos)
        this->family_type = AF_INET6;
    else
        throw "Invalid address format";

    pos = str_addr.find("/");
    if (pos != std::string::npos){
        std::string prefix_str;
        prefix_str = str_addr.substr(pos+1);
        this->prefix_len = std::stoi(prefix_str);
        str_addr = str_addr.substr(0, pos);
    }else{
        if (this->family_type == AF_INET)
            this->prefix_len = 32;
        if (this->family_type == AF_INET6)
            this->prefix_len = 128;
    }

    if (this->family_type == AF_INET){
        this->addr = new char[4];
        memset(this->addr, 0, 4);
        uint8_t tmp;
        for (int i=0; i<4; ++i){
            tmp = (uint8_t)(std::stoi(str_addr) & 0xFF);
            this->addr[i] = tmp;
            pos = str_addr.find(".");
            if (pos != std::string::npos)
                str_addr = str_addr.substr(pos+1);
        }
    }else if (this->family_type == AF_INET6){
        this->addr = new char[16];
        memset(this->addr, 0, 16);
        unsigned int tmp;
        bool double_colon = -1;
        int i = 0;
        while(str_addr.size() > 0 && i<16){
            if (str_addr[0] != ':'){
                tmp = (unsigned int)(std::stoi(str_addr, NULL, 16));
                this->addr[i] = (tmp >> 8) & 0xFF;
                this->addr[i+1] = (tmp) & 0xFF;
            }else{
                double_colon = i;
                str_addr = str_addr.substr(1);
                break;
            }
            pos = str_addr.find(":");
            if (pos != std::string::npos)
                str_addr = str_addr.substr(pos+1);
            else
                str_addr = "";
            i += 2;
        }
        if (double_colon > -1){
            i = 15;
            while(str_addr.size() > 0 && i>=double_colon){
                pos = str_addr.rfind(":");
                std::string num = "";
                if (pos != std::string::npos){
                    num = str_addr.substr(pos+1);
                    str_addr = str_addr.substr(0, pos);
                }else{
                    num = str_addr;
                    str_addr = "";
                }
                tmp = (unsigned int)(std::stoi(num, NULL, 16));
                this->addr[i-1] = (tmp >> 8) & 0xFF;
                this->addr[i] = (tmp) & 0xFF;

                i -= 2;
            }
            if (str_addr.size() != 0)
                throw "Invalid address format";
        }
    }

    this->flags = 0;
}

IpAddr::IpAddr(const IpAddr& other){
    this->addr = NULL;
    if(other.family_type == AF_INET){
        if (other.prefix_len > 32)
            throw "Invalid address prefix!";
        this->addr = new char[4];
        memcpy(this->addr, other.addr, 4);
    }else if (other.family_type == AF_INET6){
        if (other.prefix_len > 128)
            throw "Invalid address prefix!";
        this->addr = new char[16];
        memcpy(this->addr, other.addr, 16);
    }else
        throw "Invalid address type!";

    this->family_type = other.family_type;
    this->prefix_len = other.prefix_len;
    this->flags = other.flags;
}

IpAddr& IpAddr::operator=(const IpAddr& other){
    this->addr = NULL;
    if(other.family_type == AF_INET){
        if (other.prefix_len > 32)
            throw "Invalid address prefix!";
        this->addr = new char[4];
        memcpy(this->addr, other.addr, 4);
    }else if (other.family_type == AF_INET6){
        if (other.prefix_len > 128)
            throw "Invalid address prefix!";
        this->addr = new char[16];
        memcpy(this->addr, other.addr, 16);
    }else
        throw "Invalid address type!";

    this->family_type = other.family_type;
    this->prefix_len = other.prefix_len;
    this->flags = other.flags;
}

IpAddr::~IpAddr(){
    if (this->addr != NULL)
        delete[] this->addr;
    this->addr = NULL;
}

unsigned int IpAddr::get_prefix_len(){
    return this->prefix_len;
}

int IpAddr::get_family_type(){
    return this->family_type;
}

char *IpAddr::get_addr(){
    return this->addr;
}

int IpAddr::get_flags(){
    return this->flags;
}

int IpAddr::get_length(){
    if (this->family_type == AF_INET)
        return 4;
    else if (this->family_type == AF_INET6)
        return 16;
    else
        return 0;
}

std::ostream& operator<<(std::ostream& os, const IpAddr& addr)
{
    std::ios_base::fmtflags flags = os.flags();
    switch (addr.family_type){
        case AF_INET:
            for (int i=0; i<4; ++i){
                os << (((unsigned int)addr.addr[i]) & 0xFF);
                if (i<3)
                    os << ".";
            }
            break;
        case AF_INET6:
            os << std::hex << std::setfill('0');
            for (int i=0; i<16; i += 2){
                os << std::setw(2) << (((unsigned int)addr.addr[i]) & 0xFF);
                os << std::setw(2) << (((unsigned int)addr.addr[i+1]) & 0xFF);
                if (i<14)
                    os << ":";
            }
            os << std::dec;
            break;
        default:
            break;
    }
    os.flags(flags);
    return os;
}

/*************************/
/* HwAddr implementation */
/*************************/

HwAddr::HwAddr(){
    this->addr = NULL;
    this->type = -1;
    this->len = 0;
}

HwAddr::HwAddr(char *addr_bytes, unsigned int len, int type){
    this->addr = NULL;

    if(type == MAC_ADDR){
        if (len != 6)
            throw "Invalid address length!";
        this->addr = new char[len];
        memset(this->addr, 0, len);
        memcpy(this->addr, addr_bytes, len);
    }else
        throw "Invalid address type!";

    this->type = type;
    this->len = len;
}

HwAddr::HwAddr(std::string str_addr){
    if (str_addr.find(":") != std::string::npos)
        this->type = MAC_ADDR;
    else
        throw "Invalid address format";

    if (this->type == MAC_ADDR)
        this->len = 6;

    if (this->type == MAC_ADDR){
        this->addr = new char[6];
        memset(this->addr, 0, 6);
        unsigned int tmp;
        int pos;
        for(int i=0; i<6; ++i){
            tmp = (unsigned int)(std::stoi(str_addr, NULL, 16));
            this->addr[i] = tmp & 0xFF;
            pos = str_addr.find(":");
            if (pos != std::string::npos)
                str_addr = str_addr.substr(pos+1);
            else
                str_addr = "";
        }
    }
}

HwAddr::HwAddr(const HwAddr& other){
    this->addr = NULL;
    if(other.type == MAC_ADDR){
        if (other.len != 6)
            throw "Invalid address length!";
        this->addr = new char[other.len];
        memcpy(this->addr, other.addr, other.len);
    }else
        throw "Invalid address type!";

    this->type = other.type;
    this->len = other.len;
}

HwAddr& HwAddr::operator=(const HwAddr& other){
    this->addr = NULL;
    if(other.type == MAC_ADDR){
        if (other.len != 6)
            throw "Invalid address length!";
        this->addr = new char[other.len];
        memcpy(this->addr, other.addr, other.len);
    }else
        throw "Invalid address type!";

    this->type = other.type;
    this->len = other.len;
}

HwAddr::~HwAddr(){
    if (this->addr != NULL)
        delete[] this->addr;
    this->addr = NULL;
}

unsigned int HwAddr::get_length(){
    return this->len;
}

int HwAddr::get_type(){
    return this->type;
}

char *HwAddr::get_addr(){
    return this->addr;
}

std::ostream& operator<<(std::ostream& os, const HwAddr& addr)
{
    std::ios_base::fmtflags flags = os.flags();
    for (int i=0; i<addr.len; ++i){
        os << std::hex << std::setfill('0');
        os << std::setw(2) << (((unsigned int)addr.addr[i]) & 0xFF);
        if (i<addr.len-1)
            os << ":";
    }
    os.flags(flags);
    return os;
}
