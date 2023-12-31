#include "Server.h"

#include <boost/asio.hpp>

#include <thread>
#include <iostream>

int main()
{

	unsigned short port_num = 3333;

	try {

		Server srv;

		srv.Start(port_num);


		std::this_thread::sleep_for(std::chrono::seconds(60));


		srv.Stop();

	}

	catch (boost::system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
	}

	return 0;
}
