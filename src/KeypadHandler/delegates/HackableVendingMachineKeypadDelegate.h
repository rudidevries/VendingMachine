#ifndef HackableVendingMachineKeypadDelegate_H
  #define HackableVendingMachineKeypadDelegate_H

  #include "VendingMachineKeypadDelegate.h"

  class HackableVendingMachineKeypadDelegate : public VendingMachineKeypadDelegate {
  public:
    HackableVendingMachineKeypadDelegate(VendingMachine *vendingMachine, Adafruit_LiquidCrystal *lcd);
    void handleEnteredCode(char *code);
  };

#endif
