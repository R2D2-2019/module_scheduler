#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

#include <moving_platform_task.hpp>

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    // initialize the controller task, that checks for frames on the
    // communication bus and set the flag for the modules that have to run
    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();
    // this allocator is used to initialize hardware interfaces and I/O pins
    // this is needed because the interfaces need to be initialized before the
    // module-task
    r2d2::module_scheduler::arena_alloc_c<1024> interface_allocator;

    // initialize the task for the beetle
    auto task =
        r2d2::module_scheduler::moving_platform_task_c(interface_allocator);
    can_task.add_task(&task);

    // run rtos
    rtos::run();
}