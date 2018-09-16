#include "HackableVendingMachineKeypadDelegate.h"

HackableVendingMachineKeypadDelegate::HackableVendingMachineKeypadDelegate(VendingMachine *vendingMachine, Adafruit_LiquidCrystal *lcd) : VendingMachineKeypadDelegate(vendingMachine, lcd) {

}

void HackableVendingMachineKeypadDelegate::handleEnteredCode(char *code) {
  VendingMachineKeypadDelegate::handleEnteredCode(code);
  // TODO handle own.
}
