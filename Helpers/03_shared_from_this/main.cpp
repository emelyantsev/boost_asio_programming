#include <iostream>
#include <memory>
 
class Good : public std::enable_shared_from_this<Good>
{

public:

    std::shared_ptr<Good> getptr()
    {
        return shared_from_this();
    }
};
 

void testGood()
{
    std::shared_ptr<Good> good0 = std::make_shared<Good>();
    std::shared_ptr<Good> good1 = good0->getptr();
 
    std::cout << "good1.use_count() = " << good1.use_count() << '\n';
}

void misuseGood()
{
    try
    {
        Good not_so_good;
        std::shared_ptr<Good> gp1 = not_so_good.getptr();
    }
    catch (std::bad_weak_ptr& e)
    {
        std::cout << e.what() << '\n';
    }
}


class Best : public std::enable_shared_from_this<Best>
{
    struct Private {};
 
public:
    
    Best(Private) {}
 
    static std::shared_ptr<Best> create()
    {
        return std::make_shared<Best>(Private());
    }
 
    std::shared_ptr<Best> getptr()
    {
        return shared_from_this();
    }
};


void testBest()
{

    std::shared_ptr<Best> best0 = Best::create();
    std::shared_ptr<Best> best1 = best0->getptr();
    std::cout << "best1.use_count() = " << best1.use_count() << '\n';
 
    // Best stackBest; // <- Will not compile because Best::Best() is private.
}

struct Bad
{
    std::shared_ptr<Bad> getptr()
    {
        return std::shared_ptr<Bad>(this);
    }

    ~Bad() { std::cout << "Bad::~Bad() called\n"; }
};

void testBad()
{
    std::shared_ptr<Bad> bad0 = std::make_shared<Bad>();
    std::shared_ptr<Bad> bad1 = bad0->getptr();
    std::cout << "bad1.use_count() = " << bad1.use_count() << '\n';
}

 
int main()
{
    testGood();
    misuseGood();

    testBest();

    testBad();
}