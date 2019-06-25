#pragma once

#include <array>
#include <comm.hpp>
#include <rtos.hpp>

namespace r2d2::module_scheduler {
    class module_task_base_c {
    protected:
        comm_c comm;

        // All waitables that can trigger this module
        // to run
        std::array<rtos::waitable *, 4> waitables;

    public:
        module_task_base_c() : comm(), waitables() {
        }

        const std::array<rtos::waitable *, 4> &get_waitables() const {
            return waitables;
        }

        const base_comm_c &get_comm() const {
            return comm;
        }
    };
} // namespace r2d2::module_scheduler