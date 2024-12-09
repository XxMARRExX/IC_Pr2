#include <sam.h> // Biblioteca para acceso directo a registros del SAMD21
#include "timers.h"

// Variables externas para flags
volatile int flag1 = 0;
volatile int flag2 = 0;

/**
 * @brief Inicializa el temporizador TCC0 para el sensor 1.
 */
void init_TCC0() {
  // Enable power management clock for TCC0
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

  // Configure GCLK for TCC0
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN |
                      GCLK_CLKCTRL_GEN_GCLK0 |
                      GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
  while (GCLK->STATUS.bit.SYNCBUSY);

  // Disable TCC0
  TCC0->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (TCC0->SYNCBUSY.bit.ENABLE);

  // Reset TCC0
  TCC0->CTRLA.reg = TCC_CTRLA_SWRST;
  while (TCC0->SYNCBUSY.bit.SWRST);

  // Configure TCC0 with maximum period
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1024 |
                    TCC_CTRLA_PRESCSYNC_PRESC;
  while (TCC0->SYNCBUSY.bit.ENABLE);

  // Set maximum period
  TCC0->PER.reg = 0xFFFFFFFF;
  while (TCC0->SYNCBUSY.bit.PER);

  // Enable TCC0
  TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (TCC0->SYNCBUSY.bit.ENABLE);
}


/**
 * @brief Establece el periodo de interrupción para TCC0.
 * 
 * @param ms Periodo en milisegundos.
 */
void set_tcc0_period(int ms) {
  uint32_t period = (uint32_t)(ms * (48000000 / 1024) / 1000);
  TCC0->PER.reg = period;
  while (TCC0->SYNCBUSY.bit.PER);
}


/**
 * @brief Habilita la interrupción de TCC0.
 */
void enable_tcc0_interrupt() {
  TCC0->INTENSET.reg = TCC_INTENSET_OVF;   // Enable overflow interrupt
  NVIC_EnableIRQ(TCC0_IRQn);
}


/**
 * @brief Deshabilita la interrupción de TCC0.
 */
void disable_tcc0_interrupt() {
  NVIC_DisableIRQ(TCC0_IRQn);
  TCC0->INTENCLR.reg = TCC_INTENCLR_OVF;  // Disable overflow interrupt
}


/**
 * @brief Inicializa los temporizadores TC4 y TC5 para el sensor 2.
 */
void init_TC4_TC5() {
  // Enable power management clock for TC4 and TC5
  PM->APBCMASK.reg |= PM_APBCMASK_TC4 | PM_APBCMASK_TC5;

  // Configure GCLK for TC4 and TC5
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN |
                      GCLK_CLKCTRL_GEN_GCLK0 |
                      GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  // Disable TC4 and TC5
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

  // Reset TC4 and TC5
  TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

  // Configure TC4 and TC5 for 32-bit counter mode
  TC4->COUNT32.CTRLA.reg = TC_CTRLA_MODE_COUNT32 |    // 32-bit mode
                             TC_CTRLA_PRESCALER_DIV1024 | // Same prescaler as TCC0
                             TC_CTRLA_WAVEGEN_NFRQ;      // Normal Frequency mode
    
  while (TC4->COUNT32.STATUS.bit.SYNCBUSY);

  // Set maximum period
  TC4->COUNT32.CC[0].reg = 0xFFFFFFFF;
  while (TC4->COUNT32.STATUS.bit.SYNCBUSY);

  // Enable TC4 (TC5 is part of TC4 in 32-bit mode)
  TC4->COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC4->COUNT32.STATUS.bit.SYNCBUSY);
}


/**
 * @brief Establece el periodo de interrupción para TC4/TC5.
 * 
 * @param ms Periodo en milisegundos.
 */
void set_tc4_tc5_period(int ms) {
  uint32_t period = (uint32_t)(ms * (48000000 / 1024) / 1000);
  TC4->COUNT32.CC[0].reg = period;
  while (TC4->COUNT32.STATUS.bit.SYNCBUSY);
}


/**
 * @brief Habilita la interrupción de TC4/TC5.
 */
void enable_tc4_tc5_interrupt() {
  TC4->COUNT32.INTENSET.reg = TC_INTENSET_MC0;   // Enable match compare interrupt
  NVIC_EnableIRQ(TC4_IRQn);
}

/**
 * @brief Deshabilita la interrupción de TC4/TC5.
 */
void disable_tc4_tc5_interrupt() {
  NVIC_DisableIRQ(TC4_IRQn);
  TC4->COUNT32.INTENCLR.reg = TC_INTENCLR_MC0;  // Disable match compare interrupt
}


/**
 * @brief Manejador de interrupciones para el temporizador TCC0.
 * 
 * Este manejador se activa cuando ocurre un desbordamiento en TCC0. 
 * Establece el flag `flag1` para indicar que el sensor 1 debe realizar una medición.
 */
void TCC0_Handler() {
  if (TCC0->INTFLAG.bit.OVF) {
      TCC0->INTFLAG.reg = TCC_INTFLAG_OVF;
      flag1 = 1;
  }
}


/**
 * @brief Manejador de interrupciones para el temporizador TC4.
 * 
 * Este manejador se activa cuando ocurre una comparación en TC4. 
 * Establece el flag `flag2` para indicar que el sensor 2 debe realizar una medición.
 */
void TC4_Handler() {
    if (TC4->COUNT32.INTFLAG.bit.MC0) {
        TC4->COUNT32.INTFLAG.bit.MC0 = 1;    // Clear the interrupt flag
        TC4->COUNT32.COUNT.reg = 0;          // Reset counter to zero
        while (TC4->COUNT32.STATUS.bit.SYNCBUSY); // Wait for sync
        flag2 = 1;
    }
}