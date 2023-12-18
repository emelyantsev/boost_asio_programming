#include <thread>
#include <chrono>
#include <iostream>

#include "AsyncTCPClient.h"


void handler(unsigned int request_id, const std::string& response, const boost::system::error_code& ec) 
{

    std::cout << "Thread id of handler " << std::this_thread::get_id() << std::endl;

	if (ec.value() == 0) {
		std::cout << "Request #" << request_id << " has completed. Response: " << response << std::endl;
	} else if (ec.value() == boost::asio::error::operation_aborted) {
		std::cout << "Request #" << request_id << " has been cancelled by the user." << std::endl;
	} else {
		std::cout << "Request #" << request_id << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << std::endl;
	}

	return;
}


int main()
{
	try {

        std::cout << "Main thread id " << std::this_thread::get_id() << std::endl;

		AsyncTCPClient client(4);

		client.emulateLongComputationOp(10, "127.0.0.1", 3333, handler, 1);

		std::this_thread::sleep_for(std::chrono::seconds(5));

		client.emulateLongComputationOp(11, "127.0.0.1", 3334, handler, 2);

		client.cancelRequest(2);

		std::this_thread::sleep_for(std::chrono::seconds(6));

		client.emulateLongComputationOp(12, "127.0.0.1", 3335, handler, 3);

		std::this_thread::sleep_for(std::chrono::seconds(15));

		client.close();
	}

	catch (boost::system::system_error& e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
};
