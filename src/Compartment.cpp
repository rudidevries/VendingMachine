#include "Compartment.h"

Compartment::Compartment(uint8_t number, unsigned int priceInCents) {
  _number = number;
  _priceInCents = priceInCents;
}

unsigned int Compartment::priceInCents() {
  return _priceInCents;
}

uint8_t Compartment::number() {
  return _number;
}
