#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

#include <button/module.hpp>
#include <controller/module.hpp>
#include <led/module.hpp>

namespace r2d2::module_scheduler {

    class led_task_c : public module_task_c<r2d2::led::module_c> {
    public:
        led_task_c(alloc_c &alloc)
            : module_task_c(alloc.alloc_object<hwlib::target::pin_out>(
                  hwlib::target::pins::led)) {
        }
    };

    class controller_task_c : public module_task_c<r2d2::controller::module_c> {
    public:
        controller_task_c(alloc_c &alloc) : module_task_c() {
        }
    };

    class button_task_c : public module_task_c<r2d2::button::module_c> {
    private:
        static hwlib::target::pin_in &
        allocate_pin_with_pullup(alloc_c &alloc, hwlib::target::pins pin) {
            auto &pin_in = alloc.alloc_object<hwlib::target::pin_in>(pin);
            pin_in.pullup_enable();
            return pin_in;
        }

    public:
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

    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();
    r2d2::module_scheduler::arena_alloc_c<1024> interface_allocator;

    r2d2::module_scheduler::led_task_c task(interface_allocator);
    can_task.add_task(&task);

    r2d2::module_scheduler::controller_task_c task2(interface_allocator);
    task2.create_waitable<rtos::clock>(100'000);
    can_task.add_task(&task2);

    r2d2::module_scheduler::button_task_c task3(interface_allocator);
    can_task.add_task(&task3);

    hwlib::cout << "pre rtos run" << hwlib::endl;
    rtos::run();
}