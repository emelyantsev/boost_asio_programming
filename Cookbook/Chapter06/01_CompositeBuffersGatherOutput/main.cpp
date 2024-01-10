#include <boost/asio.hpp>
#include <iostream>
#include <cstring>

int main()
{
	const char* part1 = "Hello ";
	const char* part2 = "my ";
	const char* part3 = "friend!";

    std::vector<boost::asio::const_buffer> composite_buffer;

	composite_buffer.push_back(boost::asio::const_buffer(part1, strlen(part1) ) );
	composite_buffer.push_back(boost::asio::const_buffer(part2, strlen(part2) ) );
	composite_buffer.push_back(boost::asio::const_buffer(part3, strlen(part3) ) );

	for (const auto& buf : composite_buffer) {

		for (int i = 0; i < buf.size() ; ++i) {

			const char* data_ptr = static_cast<const char*>( buf.data() );
			std::cout << data_ptr[i] << " " ;
		}

		std::cout << std::endl;
	}

	return 0;
}
