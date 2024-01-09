#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include "Server.h"

int main()
{
	unsigned short port_num = 3333;

	try {

		Server srv;
		srv.start(port_num);
		std::this_thread::sleep_for(std::chrono::seconds(60));
		srv.stop();
	}

	catch (boost::system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
	}

	return 0;
}
