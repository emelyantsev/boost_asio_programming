#include <unordered_map>
#include <string>
#include <iostream>
#include <optional>
#include <cassert>
#include <vector>

using map_t = std::unordered_map<std::string, int> ;


std::optional<int> get_age( const map_t& people_db, const std::string& name ) {

    auto it = people_db.find(name) ;

    if (it != people_db.end() ) {
        return it->second;
    }
    else {
        return std::nullopt;
    }
}


int main() {

    map_t people;

    people["Aleksey"] = 37;
    people["Mike"] = 3;

    for (const auto& [name, age] : people) {

        std::cout << name << " : " << age << std::endl;
    }


    {

        std::vector<std::string> names = {"Mike", "Sasha"} ;


        for (const auto& name : names) {


            auto res = get_age(people, name);

            std::cout << name << " " << res.value_or(-1) << std::endl;
        }



    }


    return 0;
}