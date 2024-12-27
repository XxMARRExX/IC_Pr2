#include "timers.h"
using namespace tc_lib::arduino_mkr;

// Variables externas para flags
volatile int flag1 = 0;
volatile int flag2 = 0;

int timer1_period_ms = 0;
int timer2_period_ms = 0;

struct ctx {
  ctx() { onoff = false; counter = 0; }
  bool onoff;
  uint32_t counter;
};

ctx action_ctx_timer1;
ctx action_ctx_timer2;

// Iniciar los timers
void init_timers(){
    //Por ahora nada anuque se espera que modifique el prescaler
}

//zona timer 1 (tc4)
void start_timer_1(int ms) {
    int period = ms*100000; // timer en 100000us = 1ms
    action_tc4.start(period, set_flag_action_timer1, &action_ctx_timer1);
}

void stop_timer_1() {
    action_tc4.stop();
}

void set_flag_action_timer1(void* a_ctx) {
  flag1 = 1;
}

//zona timer 2 (tc5)
void start_timer_2(int ms) {
    int period = ms*100000; // timer en 100000us = 1ms
    action_tc5.start(period, set_flag_action_timer2, &action_ctx_timer2);
}

void stop_timer_2() {
    action_tc5.stop();
}

void set_flag_action_timer2(void* a_ctx) {
  flag2 = 1;
}
