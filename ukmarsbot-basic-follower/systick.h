#ifndef SYSTIC_H
#define SYSTICK_H
#include <stdint.h>

extern volatile uint32_t ticks;
void setupSystick(int frequency);

#endif