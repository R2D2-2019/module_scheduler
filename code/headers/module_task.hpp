#pragma once

#include <alloc.hpp>
#include <array>
#include <module_task_base.hpp>
#include <rtos.hpp>

namespace r2d2::module_scheduler {
    template <typename ModuleType, int StackSize = 4096,
              int WaitableAllocSize = 128>
    class module_task_c : public rtos::task<StackSize>,
                          public module_task_base_c {
    private:
        size_t next_waitable_index = 0;

    protected:
        // The module we are wrapping
        ModuleType module;

        // Allocater that contains the memory for the waitables.
        // This allows us to control the lifetimes of waitables
        // ourselfs.
        arena_alloc_c<WaitableAllocSize> allocator;

    public:
        // initializes the module and it's task
        // the constructor for the module is called interaly, all but the first
        // constructor parameter must be passed to this constructor
        // this constructor assumes the first parameter of the module's
        // constructor is of the type base_comm_c. If this is not the case, a
        // adapter should be written.
        template <typename... Args>
        explicit module_task_c(Args &&... args)
            : rtos::task<StackSize>("task_module"),
              module_task_base_c(),
              module(comm, std::forward<Args>(args)...),
              allocator() {
            create_waitable<rtos::flag>("start_flag");
        }

        // Creates a waitable that will be linked to this task.
        // All arguments after the 'this' argument (the first argument)
        // of the task should be passed to this function.
        template <typename T, typename... Args>
        T &create_waitable(Args &&... args) {
            if (next_waitable_index == 4) {
                hwlib::cout << "Too many waitables!!" << hwlib::endl;
                // Too many waitables
                HWLIB_PANIC_WITH_LOCATION;
            }

            void *mem = allocator.alloc(sizeof(T));
            T *waitable;
            waitables[next_waitable_index] = waitable =
                new (mem) T(this, std::forward<Args>(args)...);
            next_waitable_index++;

            return *waitable;
        }

        // rtos task main
        void main() override {
            for (;;) {
                this->wait();
                module.process();
            }
        }
    };
} // namespace r2d2::module_scheduler