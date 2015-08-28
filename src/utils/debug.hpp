#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "../main/params.hpp"

#if USE_DEBUG_PRINT
#define DEBUG_PRINT(x) (std::cout << (x) << std::endl)
#else 
#define DEBUG_PRINT(x)
#endif

#if USE_DEBUG_PRINT_VERBOSE
#define DEBUG_PRINT_VERBOSE(x) (std::cout << (x) << std::endl)
#else 
#define DEBUG_PRINT_VERBOSE(x)
#endif

#endif // DEBUG_HPP
