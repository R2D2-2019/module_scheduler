#pragma once

#include <module_task_base.hpp>

namespace r2d2::module_scheduler {
    // this task checks the communication bus for every module and unlocks the
    // task when there are frames ready to be processed
    template <size_t Size>
    class can_bus_pending_task_c : public rtos::task<256> {
    private:
        std::array<module_task_base_c *, Size> tasks;
        size_t next_task_index = 0;

    public:
        // adds a module to the list of tasks
        // this task will loop through this list to check for frames for every
        // module
        void add_task(module_task_base_c *task) {
            if (next_task_index == Size) {
                // // Too many tasks
                HWLIB_PANIC_WITH_LOCATION;
            }
            tasks[next_task_index] = task;
            next_task_index++;
        }

        // rtos task main
        void main() override {
            for (;;) {
                for (unsigned int i = 0; i < next_task_index; i++) {
                    module_task_base_c *task = tasks[i];
                    if (task != nullptr && task->get_comm().has_data()) {
                        // check for data on the can bus
                        static_cast<rtos::flag *>(task->get_waitables()[0])
                            ->set(); // set the flag to unlock the task
                    }
                }
                hwlib::wait_ms(50);
            }
        }
    };
} // namespace r2d2::module_scheduler