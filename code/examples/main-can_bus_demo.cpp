/*

This main is used to test the module-scheduler
For this programm to compile, the supplied Makefile must be used

This program makes the can-bus demo run on one arduino using the
module-scheduler

for the button, I/O pin 53 should be used, there is no external pullup resistor
needed. by pushing the button, the built-in led should turn on, by releasing the
button, the led should turn off.

*/

#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

// include the classes from the can-bus demo, located in the
// internal-communication folder
#include <button/module.hpp>
#include <controller/module.hpp>
#include <led/module.hpp>

namespace r2d2::module_scheduler {

    // this class initializes the led-module task with the arguments needed by
    // the module
    class led_task_c : public module_task_c<r2d2::led::module_c> {
    public:
        // first initialize the output pin, then initialize the module-task with
        // this pin
        // the output pin must be initialized using the allocator,
        // because the pin must be initialized before the module-task
        led_task_c(alloc_c &alloc)
            : module_task_c(alloc.alloc_object<hwlib::target::pin_out>(
                  hwlib::target::pins::led)) {
        }
    };

    // this class initializes the controller-module task with the arguments
    // needed by the module
    class controller_task_c : public module_task_c<r2d2::controller::module_c> {
    public:
        controller_task_c(alloc_c &alloc) : module_task_c() {
        }
    };

    // this class initializes the button-module task with the arguments
    // needed by the module
    class button_task_c : public module_task_c<r2d2::button::module_c> {
    private:
        /**
         * Create a hwlib::target::pin_in, enable the pullup resistor and return
         * the pin
         *
         */
        static hwlib::target::pin_in &
        allocate_pin_with_pullup(alloc_c &alloc, hwlib::target::pins pin) {
            auto &pin_in = alloc.alloc_object<hwlib::target::pin_in>(pin);
            pin_in.pullup_enable();
            return pin_in;
        }

    public:
        // first initialize the input pin, then initialize the module-task with
        // this pin
        // the input pin must be initialized using the allocator, because the
        // pin must be initialized before the module-task
        button_task_c(alloc_c &alloc)
            : module_task_c(
                  allocate_pin_with_pullup(alloc, hwlib::target::pins::d53)) {
        }
    };
} // namespace r2d2::module_scheduler

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

    // initialize the task for the led-module
    r2d2::module_scheduler::led_task_c task(interface_allocator);
    can_task.add_task(&task);

    // initialize the task for the controller-module
    r2d2::module_scheduler::controller_task_c task2(interface_allocator);
    // this module needs to be called every 1/10 second, to send frame-requests
    task2.create_waitable<rtos::clock>(100'000);
    can_task.add_task(&task2);

    // initialize the task for the button-module
    r2d2::module_scheduler::button_task_c task3(interface_allocator);
    can_task.add_task(&task3);

    // run rtos
    rtos::run();
}