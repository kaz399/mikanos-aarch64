#include <new>
#include <cerrno>


extern "C" int posix_memalign(void**, size_t, size_t) {
  return ENOMEM;
}

void __cxa_allocate_exception() {  }
void __cxa_throw() {  }
