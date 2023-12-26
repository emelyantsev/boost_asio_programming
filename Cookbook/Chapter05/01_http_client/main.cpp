#include <memory>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "HttpTools.h"


void handler(const HTTPRequest& request, const HTTPResponse& response, const boost::system::error_code& ec) ;


int main()
{
	try {

		HTTPClient client;

		std::shared_ptr<HTTPRequest> request_one = client.create_request(1);

		request_one->set_host("localhost");
		request_one->set_uri("/index.html");
		request_one->set_port(8000);
		request_one->set_callback(handler);
		request_one->execute();

		std::shared_ptr<HTTPRequest> request_two = client.create_request(2);

		request_two->set_host("localhost");
		request_two->set_uri("/echo.html");
		request_two->set_port(8000);
		request_two->set_callback(handler);
		request_two->execute();
	//	request_two->cancel();

		std::this_thread::sleep_for( std::chrono::seconds(15) );

		client.close();
	}
	catch ( boost::system::system_error& e ) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;
		return e.code().value();
	}

	return 0;
}


void handler(const HTTPRequest& request, const HTTPResponse& response, const boost::system::error_code& ec)
{
	if (ec.value() == 0) {

		std::cout << "Request #" << request.get_id() << " has completed. Response: " << response.get_response().rdbuf() << std::endl;
	}
	else if (ec == boost::asio::error::operation_aborted) {

		std::cout << "Request #" << request.get_id() << " has been cancelled by the user." << std::endl;
	}
	else {

		std::cout << "Request #" << request.get_id() << " failed! Error code = " << ec.value() << ". Error message = " << ec.message() << std::endl;
	}

	return;
}