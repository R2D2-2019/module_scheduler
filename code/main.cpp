#include "hwlib.hpp"

#include <rtos.hpp>
#include <comm.hpp>
#include <module_task.hpp>

class module : public r2d2::base_module_c {
public:
    module(r2d2::base_comm_c& comm) 
      : r2d2::base_module_c(comm) {}

    void process() override {}
};

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(1000);
    
    r2d2::comm_c comm;
    module mod(comm);

    r2d2::module_scheduler::module_task_c task(mod);
    auto& flag = task.create_waitable<rtos::flag>("start_flag");

    (void) flag;
    
    rtos::run();
}