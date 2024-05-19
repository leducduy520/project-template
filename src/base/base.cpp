#include <iostream>

#include "base.hpp"

using namespace std;

base::base()
{
    if (csv.mmap("foo.csv"))
    {
        const auto header = csv.header();
        for (const auto row : csv)
        {
            for (const auto cell : row)
            {
                // Do something with cell value
                // std::string value;
                // cell.read_value(value);
            }
        }
    }
    cout << "Hello form base\n";
}
