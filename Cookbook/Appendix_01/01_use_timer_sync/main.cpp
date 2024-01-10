
#include <iostream>
#include <boost/asio.hpp>

#define WAITING_PERIOD 2

int main()
{
  boost::asio::io_context io;

  boost::asio::steady_timer t( io, boost::asio::chrono::seconds( WAITING_PERIOD  ) );
  
  t.wait();

  std::cout << "I've been waiting for " << WAITING_PERIOD << " seconds!" << std::endl;

  return 0;
}