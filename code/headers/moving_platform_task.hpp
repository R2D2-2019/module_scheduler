#pragma once

#include <hwlib.hpp>
#include <module_task.hpp>
#include <rtos.hpp>

#include <beetle.hpp>
#include <hardware_usart.hpp>

namespace r2d2::module_scheduler {
    // adapter to make the constructor compatible with the module-scheduler
    // this is needed because the module-task expects the base_comm to be the
    // first parameter of the constructor, but the beetle constructor expects
    // the base_comm as the second parameter.
    // this class does nothing but switching those two parameters around
    class beetle_module_c : public r2d2::moving_platform::beetle_c {
    public:
        beetle_module_c(r2d2::base_comm_c &comm,
                        r2d2::moving_platform::qik_2s12v10_c &qik,
                        hwlib::adc &enc0, hwlib::adc &enc1)
            : beetle_c(qik, comm, enc0, enc1) {
        }
    };

    // this class initializes the task for the mopving-platform module with the
    // arguments needed by the module
    class moving_platform_task_c : public module_task_c<beetle_module_c> {
    public:
        // fitst initalzie the required hardware interfaces, then initialize the
        // module-task with these hardware interfaces the hardware interfaces
        // must be initialized using the allocator, because they must be
        // initialized before the module-task
        moving_platform_task_c(alloc_c &alloc)
            // the module needs a r2d2::moving_platform::qik_2s12v10_c object
            // and two hwlib::target::pin_adc objects
            : module_task_c(
                  alloc.alloc_object<r2d2::moving_platform::qik_2s12v10_c>(
                      // the qik_2s12v10_c needs a hardware-usart and a
                      // hwlib::target::pin_out
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