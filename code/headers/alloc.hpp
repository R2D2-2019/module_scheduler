#pragma once

#include <cstddef>
#include <stdint.h>

namespace r2d2::module_scheduler {
    class alloc_c {
    public:
        // allocates a piece of memory that is large enough to fit a object of n
        // bytes and returns a pointer to this piece of memory
        virtual void *alloc(size_t n) = 0;

        // Creates a new object of type T that will be allocated to the memory
        // reserved by this allocator
        // All arguments for the constructor should be passed to this function.
        template <typename T, typename... Args>
        T &alloc_object(Args &&... args) {

            void *mem = alloc(sizeof(T));
            return *(new (mem) T(std::forward<Args>(args)...));
        }
    };

    template <size_t Size = 64>
    class arena_alloc_c : public alloc_c {
    protected:
        uint8_t buffer[Size];
        uint8_t *pos;

    public:
        arena_alloc_c() : pos(buffer) {
        }

        // allocates a piece of memory that is large enough to fit a object of n
        // bytes and returns a pointer to this piece of memory
        void *alloc(size_t n) override {
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