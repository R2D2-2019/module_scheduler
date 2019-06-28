#pragma once

#include <hwlib.hpp>
#include <module_task.hpp>
#include <rtos.hpp>

#include <beetle.hpp>
#include <hardware_usart.hpp>

namespace r2d2::module_scheduler {
    // adapter to make the constructor compatible with the module-scheduler
    class beetle_module_c : public r2d2::moving_platform::beetle_c {
    public:
        beetle_module_c(r2d2::base_comm_c &comm,
                        r2d2::moving_platform::qik_2s12v10_c &qik,
                        hwlib::adc &enc0, hwlib::adc &enc1)
            : beetle_c(qik, comm, enc0, enc1) {
        }
    };

    class moving_platform_task_c : public module_task_c<beetle_module_c> {
    public:
        moving_platform_task_c(alloc_c &alloc)
            : module_task_c(
                  alloc.alloc_object<r2d2::moving_platform::qik_2s12v10_c>(
                      alloc.alloc_object<
                          r2d2::usart::hardware_usart_c<r2d2::usart::usart0>>(
                          9600),
                      alloc.alloc_object<hwlib::target::pin_out>(
                          hwlib::target::pins::d5)),
                  alloc.alloc_object<hwlib::target::pin_adc>(
                      hwlib::target::ad_pins::a0),
                  alloc.alloc_object<hwlib::target::pin_adc>(
                      hwlib::target::ad_pins::a1)) {
        }
    };
} // namespace r2d2::module_scheduler