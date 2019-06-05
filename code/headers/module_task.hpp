#pragma once

#include <alloc.hpp>
#include <array>
#include <base_module.hpp>
#include <rtos.hpp>

namespace r2d2::module_scheduler {
    template<int StackSize = 4096, int WaitableAllocSize = 128>
    class module_task_c : public rtos::task<StackSize> {
    protected:
        // The module we are wrapping
        base_module_c &module;

        // Allocater that contains the memory for the waitables.
        // This allows us to control the lifetimes of waitables
        // ourselfs.
        arena_alloc_c<WaitableAllocSize> allocator;

        // All waitables that can trigger this module
        // to run
        std::array<rtos::waitable*, 4> waitables;

    public:
        explicit module_task_c(base_module_c &module)
            : module(module) { }

        /**
         * Create a waitable that will be linked to this task.
         * All arguments after the 'this' argument (the first argument)
         * of the task should be passed.
         */ 
        template<typename T, typename ...Args>
        T &create_waitable(Args&&... args) {
            if (waitables.size() == 4) {
                // Too many waitables
                HWLIB_PANIC_WITH_LOCATION;
            }

            size_t pos = waitables.size();
            void *mem = allocator.alloc(sizeof(T));
            waitables[pos] = new (mem) T(this, std::forward<Args>(args)...);    

            return *(
                static_cast<T*>(waitables[pos])
            );
        }
        
        std::array<rtos::waitable*, 4> &get_waitables() {
            return waitables;
        }

        void main() override {
            this->wait();
            module.process();
        }
    };
}