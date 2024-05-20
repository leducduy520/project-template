#include "base.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>

base::base(/* args */)
{
}

base::~base()
{
}

void base::dosomething()
{
    YAML::Emitter out;
    out << "Hello, World!";
    std::cout << "Here's the output YAML:\n" << out.c_str(); // prints "Hello, World!"
}