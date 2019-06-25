#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

#include <button_module.hpp>
#include <controler_module.hpp>
#include <led_module.hpp>

class led_task_c
    : public r2d2::module_scheduler::module_task_c<r2d2::led::module_c> {
private:
    hwlib::target::pin_out led;

public:
    led_task_c() : module_task_c(led), led(hwlib::target::pins::led) {
        hwlib::cout
            << sizeof(
                   r2d2::module_scheduler::module_task_c<r2d2::led::module_c>)
            << hwlib::endl;
    }
};

class controller_task_c
    : public r2d2::module_scheduler::module_task_c<r2d2::controller::module_c> {

public:
    controller_task_c() : module_task_c() {
    }
};

class button_task_c
    : public r2d2::module_scheduler::module_task_c<r2d2::button::module_c> {
private:
    hwlib::target::pin_in button;

public:
    button_task_c() : module_task_c(button), button(hwlib::target::pins::d53) {
        button.pullup_enable();
    }
};

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();

    // hwlib::target::pin_out led(hwlib::target::pins::led);
    led_task_c task;
    can_task.add_task(&task);

    controller_task_c task2;
    task2.create_waitable<rtos::clock>(100'000);
    can_task.add_task(&task2);

    hwlib::target::pin_in button(hwlib::target::pins::d53);
    button.pullup_enable();
    auto task3 =
        r2d2::module_scheduler::module_task_c<r2d2::button::module_c>(button);
    can_task.add_task(&task3);

    hwlib::cout << "pre rtos run" << hwlib::endl;
    rtos::run();
}