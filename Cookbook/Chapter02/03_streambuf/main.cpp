#include <boost/asio.hpp>

#include <iostream>


using namespace boost;


int main()
{

	asio::streambuf buf;

	std::ostream output(&buf);

	output << "Message1\nMessage2";

	std::istream input(&buf);

	std::string message1;

    std::cout << buf.size() << std::endl;

	std::getline(input, message1);

    std::cout << buf.size() << std::endl;

    output << "Message3\nMessage4";

    std::cout << buf.size() << std::endl;

	return 0;
}
