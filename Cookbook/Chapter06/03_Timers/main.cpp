#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>

int main()
{
	boost::asio::io_context io_ctx;

	boost::asio::steady_timer t1(io_ctx);
	t1.expires_from_now(std::chrono::seconds(2));

	boost::asio::steady_timer t2(io_ctx);
	t2.expires_from_now(std::chrono::seconds(5));

	t1.async_wait( [&t2](boost::system::error_code ec) {

		if (ec.value() == 0) {

			std::cout << "Timer #1 has expired!" << std::endl;
		}
		else if (ec == boost::asio::error::operation_aborted) {

			std::cout << "Timer #1 has been cancelled!" << std::endl;
		}
		else {

			std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
		}

		t2.cancel();
	});

	t2.async_wait([](boost::system::error_code ec) {

		if (ec.value() == 0) {
			
            std::cout << "Timer #2 has expired!" << std::endl;
		}
		else if (ec == boost::asio::error::operation_aborted) {

			std::cout << "Timer #2 has been cancelled!" << std::endl;
		}
		else {
			
            std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
		}
	});

	io_ctx.run();

	return 0;
}