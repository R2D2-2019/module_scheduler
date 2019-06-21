#include <hwlib.hpp>
#include <rtos.hpp>

#include <can_bus_pending_task.hpp>
#include <module_task.hpp>

// some classes to test-compile:
class led_module : public r2d2::base_module_c {
public:
    led_module(r2d2::base_comm_c &comm, hwlib::pin_out &pin)
        : r2d2::base_module_c(comm) {
    }

    void process() override {
        hwlib::cout << "led-module" << hwlib::endl;
    }
};
class module : public r2d2::base_module_c {
public:
    module(r2d2::base_comm_c &comm) : r2d2::base_module_c(comm) {
    }

    void process() override {
        hwlib::cout << "module" << hwlib::endl;
    }
};

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();

    hwlib::target::pin_out led(hwlib::target::pins::led);
    auto task = r2d2::module_scheduler::module_task_c<led_module>(led);
    can_task.add_task(&task);

    // auto task2 = r2d2::module_scheduler::module_task_c<module>();
    // can_task.add_task(&task2);

    rtos::run();
}