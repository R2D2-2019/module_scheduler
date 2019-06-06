#include "hwlib.hpp"

#include <comm.hpp>
#include <module_task.hpp>
#include <rtos.hpp>

// some classes to test-compile:
class led_module : public r2d2::base_module_c {
public:
    led_module(r2d2::base_comm_c &comm, hwlib::pin_out &pin)
        : r2d2::base_module_c(comm) {
    }

    void process() override {
    }
};
class module : public r2d2::base_module_c {
public:
    module(r2d2::base_comm_c &comm) : r2d2::base_module_c(comm) {
    }

    void process() override {
    }
};

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    hwlib::target::pin_out led(hwlib::target::pins::led);
    auto task = r2d2::module_scheduler::module_task_c<led_module>(led);
    auto &flag = task.create_waitable<rtos::flag>("start_flag");

    auto task2 = r2d2::module_scheduler::module_task_c<module>();
    auto &flag2 = task2.create_waitable<rtos::flag>("start_flag");

    (void)flag;
    (void)flag2;

    rtos::run();
}