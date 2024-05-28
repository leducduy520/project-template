#pragma once


#include "IniReader.h"
#include "csv2/reader.hpp"
#include "csv2/writer.hpp"

class base
{
private:
    /* data */
    csv2::Reader<csv2::delimiter<','>,
                 csv2::quote_character<'"'>,
                 csv2::first_row_is_header<true>,
                 csv2::trim_policy::trim_whitespace>
        csv;
    INIReader iniRd{""};

public:
    base(/* args */);
    ~base() = default;
};