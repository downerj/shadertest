#ifndef DEBUG_HXX
#define DEBUG_HXX

#ifdef DEBUG
#include <iostream>
#define DEBUG_LOG(x) \
  do { \
    std::cout << x << '\n'; \
  } while (0);
#endif // DEBUG

#endif // DEBUG_HXX
