#define ASSERT_WITH_MSG(cond, msg) do \
{ if (!(cond)) { std::ostringstream str; str << msg; std::cerr << str.str(); std::abort(); } \
} while(0)

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <algorithm>
#include <iostream>
#include <istream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <map>
#include <numeric>
#include <memory>