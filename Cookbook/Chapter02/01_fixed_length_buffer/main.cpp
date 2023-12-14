#include <boost/asio.hpp>

#include <iostream>
#include <string>

using namespace boost;


int main()
{

	const std::string buf = "Hello"; 

	//buf = "Hello";  

	asio::const_buffers_1 output_buf = asio::buffer(buf);

    std::cout << typeid(output_buf).name() << std::endl;
	

    for (size_t i = 0; i < output_buf.size(); ++i) {

        const char* data_ptr = static_cast<const char* >( output_buf.data() );

        std::cout <<  data_ptr[i] <<  " ";
    }


    std::cout << std::endl;

	return 0;

}
