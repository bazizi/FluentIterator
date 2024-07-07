#include "FluentIterator.hpp"

#include <iostream>
#include <list>

int main()
{
    {
        FluentIterator<std::vector<int>> v = {1, 2, 3, 4, 5, 6};

        auto result = v.map([](auto const &item)
                            { return item * 20; })
                          .print() // 20, 40, 60, 80, 100, 120
                          .filter([](auto const &item)
                                  { return item % 3 != 0; })
                          .print() // 20, 40, 80, 100
                          .map([](auto const &item)
                               { return item / 4; })
                          .print() // 5, 10, 20, 25
                          .filter([](auto const &item)
                                  { return item % 4 == 0; })
                          .print() // 20
            ;

        std::cout << std::endl;
    }

    {
        FluentIterator<std::list<int>> v = {1, 2, 3, 4, 5, 6};

        auto result = v.map([](auto const &item)
                            { return item * 20; })
                          .print() // 20, 40, 60, 80, 100, 120
                          .filter([](auto const &item)
                                  { return item % 3 != 0; })
                          .print() // 20, 40, 80, 100
                          .map([](auto const &item)
                               { return item / 4; })
                          .print() // 5, 10, 20, 25
                          .filter([](auto const &item)
                                  { return item % 4 == 0; })
                          .print() // 20
            ;
    }
}
