#include <iostream>
#include "base.hpp"
#include "yaml-cpp/yaml.h"
#include "pprint.hpp"
#include "wise_enum/wise_enum.h"
#include "wise_enum/compact_optional.h"

// Equivalent to enum Color {GREEN = 2, RED};
namespace my_lib {
WISE_ENUM((Color, uint8_t), (GREEN, 2), RED)
}

// Equivalent to enum class MoreColor : int64_t {BLUE, BLACK = 1};
WISE_ENUM_CLASS((MoreColor, int64_t), BLUE, (BLACK, 1LL))

base::base(/* args */)
{
    pprint::PrettyPrinter printer;
    printer.print(5);
    printer.print(3.14f);
    printer.print(2.718);
    printer.print(true);
    printer.print('x');
    printer.print("Hello, 世界");
    printer.print(nullptr);
}

base::~base()
{
  // Number of enumerations:
  static_assert(wise_enum::enumerators<my_lib::Color>::size == 2);
  std::cerr << "Number of enumerators: "
            << wise_enum::enumerators<my_lib::Color>::size << "\n";

  // Iterate over enums
  std::cerr << "Enum values and names:\n";
  for (auto element : wise_enum::enumerators<my_lib::Color>::range) {
    std::cerr << static_cast<int>(element.value) << " " << element.name << "\n";
  }
  std::cerr << "\n";

  // Convert any enum to a string
  std::cerr << wise_enum::to_string(my_lib::Color::RED) << "\n";

  // Convert any string to an optional enum
  auto green1 = wise_enum::from_string<my_lib::Color>("GREEN");
  auto green2 = wise_enum::from_string<my_lib::Color>("Greeeeeeen");

  assert(green1.value() == my_lib::Color::GREEN);
  assert(green2.value() == my_lib::Color::GREEN);
}

void base::dosomething()
{
    YAML::Emitter out;
    out << "Hello, World!\n";
    std::cout << "Here's the output YAML:\n" << out.c_str() << std::endl; // prints "Hello, World!"
}