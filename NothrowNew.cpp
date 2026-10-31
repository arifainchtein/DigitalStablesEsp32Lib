//
// new (std::nothrow) that really doesn't throw (2026-10-31).
//
// The libstdc++ version calls the throwing operator new inside a try/catch. When the heap is
// empty, throwing std::bad_alloc needs memory for the exception object too; when that fails as
// well the C++ runtime calls std::terminate() and the ESP32 aborts. Annabelle v6 panicked that
// way in AsyncTCP tcp_accept() ("new (std::nothrow) AsyncClient") while the WiFi driver had taken
// nearly all of the 8-bit heap - instead of the connection being refused, the board restarted.
//
// These replacements go straight to malloc and return nullptr on failure, as the caller expects.
// They replace the libstdc++ ones for every sketch that uses this library (the linker takes a
// definition from an object file over the one in libstdc++). The matching nothrow deletes from
// libstdc++ call operator delete -> free(), so nothing else needs replacing.
//
#include <new>
#include <stdlib.h>

void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
  return malloc(size ? size : 1);
}

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept {
  return malloc(size ? size : 1);
}
