#include "VendingMachineKeypadDelegate.h"

VendingMachineKeypadDelegate::VendingMachineKeypadDelegate(VendingMachine *vendingMachine, Adafruit_LiquidCrystal *lcd) {
  _vendingMachine = vendingMachine;
  _lcd = lcd;
}

void VendingMachineKeypadDelegate::handleDigit(char digit) {
  _vendingMachine->selectCompartment(digit);
}

void VendingMachineKeypadDelegate::handleEnteredCode(char *code) {
  if (strcmp("361447", code) == 0) {
    _vendingMachine->state = administrator;
  }
}

void VendingMachineKeypadDelegate::handleEnteringCode(char *code) {
  _lcd->setCursor(0, 1);
  _lcd->print("       CODE: ");
  _lcd->setCursor(0, 2);

  char line[21];
  sprintf(line, "%-20s", code);

  _lcd->print(line);
}
