#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

#include <moving_platform_task.hpp>

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();
    r2d2::module_scheduler::arena_alloc_c<1024> interface_allocator;

    auto task =
        r2d2::module_scheduler::moving_platform_task_c(interface_allocator);
    can_task.add_task(&task);

    rtos::run();
}