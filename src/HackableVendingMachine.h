#ifndef HackableVendingMachine_h
  #define HackableVendingMachine_h

  #include "MqttVendingMachine.h"

  // Adds functionality to the vending machine to manipulate it and
  // bypass the normal payment system.
  class HackableVendingMachine : public MqttVendingMachine {
    public:
      HackableVendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, ELClientMqtt *mqtt, uint8_t numCompartments, Compartment **Compartments);
  };

#endif
