#include <boost/asio.hpp>
#include <iostream>


int main()
{
	boost::asio::ip::tcp::iostream stream("localhost", "3333");
	
    if (!stream) {
	
    	std::cout << "Error occurred! Error code = " << stream.error().value() << ". Message = " << stream.error().message() << std::endl;
		return -1;
	}

	stream << "Request1";
	stream << "Request2\n";
	stream.flush();

	std::cout << "Response: " << stream.rdbuf();

	return 0;
}
