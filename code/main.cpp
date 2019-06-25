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
    template <typename T, typename... Args>
    static T *allocate(Args &&... args) {
        static uint8_t alloc[64];
        static uint8_t *pos = alloc;

        uint32_t alignment = uint_fast32_t(pos) & 3;
        if (alignment != 0) {
            pos += 4 - alignment;
        }

        void *addr = static_cast<void *>(pos);
        pos += sizeof(T);
        return new (addr) T(std::forward<Args>(args)...);
    }

public:
    led_task_c()
        : module_task_c(
              *allocate<hwlib::target::pin_out>(hwlib::target::pins::led)) {
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
    template <typename T, typename... Args>
    static T *allocate(Args &&... args) {
        static uint8_t alloc[64];
        static uint8_t *pos = alloc;

        uint32_t alignment = uint_fast32_t(pos) & 3;
        if (alignment != 0) {
            pos += 4 - alignment;
        }

        void *addr = static_cast<void *>(pos);
        pos += sizeof(T);
        return new (addr) T(std::forward<Args>(args)...);
    }

    hwlib::target::pin_in *button;

public:
    button_task_c()
        : module_task_c(
              *allocate<hwlib::target::pin_in>(hwlib::target::pins::d53)) {
        // button.pullup_enable();
    }
};

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);

    auto can_task = r2d2::module_scheduler::can_bus_pending_task_c<32>();

    // hwlib::target::pin_out *led =
    //     new hwlib::target::pin_out(hwlib::target::pins::led);
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