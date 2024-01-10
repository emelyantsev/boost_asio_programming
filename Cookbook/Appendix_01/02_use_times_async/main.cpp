#include <iostream>
#include <boost/asio.hpp>

#include "tools.h"

void print(const boost::system::error_code& /*e*/)
{
  std::cout << "Hello, world!" << std::endl;
}

int main()
{
  boost::asio::io_context io;

  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

  boost::asio::steady_timer t2(io, boost::asio::chrono::seconds(1));
  
  t.async_wait(&print);

  t2.async_wait(&tools::print2);

  io.run();

  return 0;
}