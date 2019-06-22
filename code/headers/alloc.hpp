#pragma once

#include <cstddef>
#include <stdint.h>

namespace r2d2::module_scheduler {
    template <size_t Size>
    class alloc_c {
        void *alloc(size_t n);
    };

    template <size_t Size = 64>
    class arena_alloc_c : public alloc_c<Size> {
    protected:
        uint8_t buffer[Size];
        uint8_t *pos;

    public:
        arena_alloc_c() : pos(buffer) {
        }

        void *alloc(size_t n) {
            size_t remaining = (buffer + Size) - pos;

            if (n > remaining) {
                return nullptr;
            }

            // this doesn't seem to work wit static_cast
            uint32_t alignment = uint_fast32_t(pos) & 3;
            if (alignment != 0) {
                pos += 4 - alignment;
            }

            uint8_t *to_return = pos;

            pos += n;

            return static_cast<void *>(to_return);
        }
    };
} // namespace r2d2::module_scheduler