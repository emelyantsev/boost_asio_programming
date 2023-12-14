#include <boost/predef.h> 

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501

#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO	
#endif
#endif

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace boost;

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {

		asio::ip::tcp::endpoint ep( asio::ip::address::from_string(raw_ip_address), port_num );

		boost::asio::io_context io_ctx;

		std::shared_ptr<asio::ip::tcp::socket> sock( new asio::ip::tcp::socket( io_ctx, ep.protocol() ) );

		sock->async_connect(ep, [sock](const boost::system::error_code& ec)
		{
			
			if (ec.value() != 0) {

				if (ec == asio::error::operation_aborted) {

					std::cout << "Operation cancelled!";
				}
				else {
					
                    std::cout << "Error occured!"
						<< " Error code = "
						<< ec.value()
						<< ". Message: "
						<< ec.message();
				}

				return;
			}

		});


		std::thread worker_thread( [&io_ctx](){
                try {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    io_ctx.run();
                }
                catch (system::system_error &e) {
                    std::cout << "Got error in worker thread!"
                        << " Error code = " << e.code()
                        << ". Message: " << e.what();
                }
		    }
        );

		

		sock->cancel();

		worker_thread.join();
	}
	catch (system::system_error& e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

		return e.code().value();
	}

	return 0;
}