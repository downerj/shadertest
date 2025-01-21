#ifndef DEBUG_HXX
#define DEBUG_HXX

#ifdef _DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#include <iostream>
#define LOG(x) do { std::cout << x; } while (false);
#define LOG_ERROR(x) do { std::cerr << x; } while (false);
#else
#define LOG(x)
#define LOG_ERROR(x)
#endif

#endif // DEBUG_HXX
