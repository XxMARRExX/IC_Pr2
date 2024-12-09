#ifndef TIMERS_H
#define TIMERS_H

#include <sam.h> // Biblioteca para acceso directo a registros del SAMD21

// External variables for flags
extern volatile int flag1;
extern volatile int flag2;

// Function declarations
void init_TCC0();
void set_tcc0_period(int ms);
void enable_tcc0_interrupt();
void disable_tcc0_interrupt();
void init_TC4_TC5();
void set_tc4_tc5_period(int ms);
void enable_tc4_tc5_interrupt();
void disable_tc4_tc5_interrupt();
void TCC0_Handler();
void TC4_Handler();

#endif // TIMERS_H