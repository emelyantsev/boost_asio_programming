#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


void my_task()
{
    std::cout << "performing task" << std::endl;
    std::cout << boost::this_thread::get_id() << std::endl;
} 


int main() {

    boost::asio::thread_pool pool(4);

    while (true) {

        std::string command;

        std::cin >> command;

        if (command == "func") {

            boost::asio::post(pool, my_task);
        }
        else if (command == "lambda") {

            boost::asio::post(pool,
                []()
                {
                    std::cout << " performing lambda task" << std::endl;
                     std::cout << boost::this_thread::get_id() << std::endl;
                });
                }
        
        else if (command == "cancel") {

            break;
        }

    }

        // Wait for all tasks in the pool to complete.
    pool.join();


    return EXIT_SUCCESS;
}