#ifndef VendingMachineKeypadDelegate_H
  #define VendingMachineKeypadDelegate_H

  #include "../../VendingMachine.h"
  #include "../../KeypadHandler/KeypadHandlerDelegate.h"
  #include "Adafruit_LiquidCrystal.h"

  class VendingMachineKeypadDelegate : public KeypadHandlerDelegate {
  public:
    VendingMachineKeypadDelegate(VendingMachine *vendingMachine, Adafruit_LiquidCrystal *lcd);
    void handleDigit(char digit);
    void handleEnteredCode(char *code);
    void handleEnteringCode(char *code);
  private:
    VendingMachine *_vendingMachine;
    Adafruit_LiquidCrystal *_lcd;
  };

#endif
