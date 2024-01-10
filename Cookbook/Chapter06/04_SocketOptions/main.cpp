#include <boost/asio.hpp>
#include <iostream>

int main()
{
	try {

		boost::asio::io_context io_ctx;

		boost::asio::ip::tcp::socket sock( io_ctx, boost::asio::ip::tcp::v4() );

		boost::asio::socket_base::receive_buffer_size cur_buf_size;

		sock.get_option(cur_buf_size);

		std::cout << "Current receive buffer size is " 	<< cur_buf_size.value() << " bytes." << std::endl;

		boost::asio::socket_base::receive_buffer_size new_buf_size(cur_buf_size.value() * 2);

		sock.set_option(new_buf_size);

		std::cout << "New receive buferr size is " << new_buf_size.value() << " bytes." << std::endl;
	}
	catch (boost::system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

		return e.code().value();
	}

	return 0;
}
