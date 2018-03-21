#include <ma/strand.hpp>


int main(int, char**)
{
    boost::asio::io_service io_service;
    ma::strand strand{io_service};
    
    return 0;
}
