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
