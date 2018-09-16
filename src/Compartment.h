#ifndef Compartment_H
  #define Compartment_H

  #include "StepperBoard.h"

  class Compartment {
  public:
    Compartment(uint8_t number, unsigned int priceInCents);
    uint8_t number();
    unsigned int priceInCents();
  private:
    uint8_t _number;
    unsigned int _priceInCents;
  };
#endif
