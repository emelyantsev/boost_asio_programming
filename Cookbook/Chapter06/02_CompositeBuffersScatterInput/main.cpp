#include <boost/asio.hpp>
#include <iostream>


void fill_buff(boost::asio::mutable_buffer& buff, const std::string& str) {

	for (int i = 0; i < buff.size() && i < str.size(); ++i ) {

		char* ptr_data = static_cast<char*>( buff.data() );
		ptr_data[i] = str[i];
	}
} 

template<class Buffer>
void print(const Buffer& buf) {

	for (int i = 0; i < buf.size() ; ++i) {

		const char* data_ptr = static_cast<const char*>( buf.data() );
		std::cout << data_ptr[i] << " " ;
	}

	std::cout << std::endl;
} 


int main()
{
	char part1[6];
	char part2[3];
	char part3[7];

    std::vector<boost::asio::mutable_buffer> composite_buffer;

	composite_buffer.push_back(boost::asio::mutable_buffer(part1, sizeof(part1) ) );
	composite_buffer.push_back(boost::asio::mutable_buffer(part2, sizeof(part2) ) );
	composite_buffer.push_back(boost::asio::mutable_buffer(part3, sizeof(part3) ) );

	std::vector<std::string> messages = {"hello", "my", "friend!"} ;

	int i = 0;

	for (auto& buf : composite_buffer) {

		fill_buff(buf, messages[i++] ) ;
	}

	for (auto& buf : composite_buffer) {

		print( buf ) ;
	}

	return 0;
}
