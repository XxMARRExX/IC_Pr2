#ifndef TIMERS_H
#define TIMERS_H

#include <sam.h> // Biblioteca para acceso directo a registros del SAMD21
 // Biblioteca para configurar temporizadores

// External variables for flags
extern volatile int flag1;
extern volatile int flag2;

// Function declarations
void init_timers();
void start_timer_1(int ms);
void stop_timer_1();
void start_timer_2(int ms);
void stop_timer_2();

#endif // TIMERS_H