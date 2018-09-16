#include "VendingMachine.h"

#define DISP_LINE_TITLE   "      Yacht.nl      "
#define DISP_LINE_EMPTY   "                    "
#define DISP_LINE_SELECT  " Maak uw keuze[1-4] "
#define DISP_LINE_WAIT    "  Een moment A.U.B. "
#define DISP_LINE_INVALID "   Onjuiste keuze   "

#define STATE_UPD_CREDIT                  B00000001
#define STATE_UPD_INVALID_SELECTION       B00000010
#define STATE_UPD_IDLE                    B00000100
#define STATE_UPD_DISPENSE                B00001000
#define STATE_UPD_INSUFFICIENT_CREDIT     B00010000
#define STATE_DISPENSING                  B00100000

VendingMachine::VendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, uint8_t numCompartments, Compartment **compartments):
  _ledControl(ledPin, 2), _stepperBoard(new uint8_t[4]{33, 39, 45, 51}, new uint8_t[4]{31, 37, 43, 49}) {

  _lcd = lcd;

  state = operational;
  _cents = 0;
  _numCompartments = numCompartments;
  _updateState = 0;
  _stepperTargetPosition = 0;

  // TODO read from some type of storage
  _compartments = compartments;

  _configureSteppers();
  _configureLedControl();
  _stateIdle();
}

void VendingMachine::_configureSteppers() {
  for (uint8_t i = 0; i < 4; i++) {
    _stepperBoard.stepMode(i, StepperBoard::STEP_MODE_1_16);
  }
  _stepperBoard.writeDriverConfiguration();
}

void VendingMachine::_configureLedControl() {
  _ledControl.setSpotlightColor(0, 0, 255, 0);
  _ledControl.setSpotlightColor(1, 255, 0, 0);
}

void VendingMachine::loop() {
  if (_stateUpdateTimeout != 0 && _stateUpdateTimeout < millis()) {
    _stateUpdateTimeout = 0;
    _updateState |= _delayedState;
    _delayedState = 0;
  }

  if (_updateState & STATE_UPD_DISPENSE) {
    _stateDispense();
  }
  else if (_updateState & STATE_UPD_IDLE) {
    _stateIdle();
  }
  else if (_updateState & STATE_UPD_INVALID_SELECTION) {
    _stateInvalidSelection();
  }
  else if (_updateState & STATE_UPD_INSUFFICIENT_CREDIT) {
    _stateInsufficientCredit();
  }
  else if (_updateState & STATE_UPD_CREDIT) {
    _stateAddedCredit();
  }
  else if (_updateState & STATE_DISPENSING) {
    _moveStepper();
  }

  _ledControl.loop();
}

void VendingMachine::_moveStepper() {
  if (_stepperTargetPosition > 0) {
    if (_stepperBoard.currentPosition(_activeStepper) < _stepperTargetPosition) {
      _stepperBoard.loop();
    }
    else {
      _stepperTargetPosition = 0;
      _stepperBoard.enable(_activeStepper, true);
      _stepperBoard.sleep(_activeStepper, false);
      _stepperBoard.writeDriverConfiguration();
    }
  }
}

void VendingMachine::tenCentPulse() {
  _cents += 10;
  _updateState |= STATE_UPD_CREDIT;
}

unsigned int VendingMachine::getCentsRemaining() {
  return _cents;
}

void VendingMachine::selectCompartment(const char input) {
  Serial.print("Select compartment: ");
  Serial.println(input);
  _selectedCompartment = _compartmentFromChar(input);
  if (!_selectedCompartment) {
    _updateState |= STATE_UPD_INVALID_SELECTION;
    _delayedStateUpdate(STATE_UPD_IDLE, 3000);
    return;
  }

  if (!_sufficientCredit(_selectedCompartment)) {
    _stateInsufficientCredit();
    _delayedStateUpdate(STATE_UPD_IDLE, 3000);
    return;
  }

  _cents -= _selectedCompartment->priceInCents();
  _updateState |= STATE_UPD_CREDIT;
  _updateState |= STATE_UPD_DISPENSE;
  _delayedStateUpdate(STATE_UPD_IDLE, 3000);
}

void VendingMachine::resetDisplay() {
  _lcd->clear();
  _stateIdle();
  _stateAddedCredit();
}

Compartment * VendingMachine::_compartmentFromChar(const char number) {
  int converted = (number - '0');
  return _compartmentFromInt(converted);
}

Compartment * VendingMachine::_compartmentFromInt(const int number) {
  if (number < 1 || number > 4) {
    return NULL;
  }
  return _compartments[number-1];
}

bool VendingMachine::_sufficientCredit(Compartment *compartment) {
  return compartment->priceInCents() <= _cents;
}

void VendingMachine::_stateDispense() {
  Serial.println("_stateDispense");
    _updateState &= ~STATE_UPD_DISPENSE;
    if (!_selectedCompartment) {
      return;
    }

    _ledControl.spotlight(_selectedCompartment->number(), 0, 3000);

    _lcd->setCursor(0, 2);
    _lcd->print(DISP_LINE_WAIT);
    _lcd->setCursor(0, 3);
    _lcd->print(DISP_LINE_EMPTY);

    // Calculate end position
    _activeStepper = _selectedCompartment->number() - 1;
    _stepperTargetPosition = _stepperBoard.currentPosition(_activeStepper) + 3200;
    // Enable stepper on stepperboard
    _stepperBoard.enable(_activeStepper, false);
    _stepperBoard.sleep(_activeStepper, true);
    // Set stepper to move forward.
    _stepperBoard.forward(_activeStepper);
    _stepperBoard.writeDriverConfiguration();
    // Update state to wait for the end position to be reached.
    _updateState |= STATE_DISPENSING;
}

void VendingMachine::_stateIdle() {
  _updateState &= ~STATE_UPD_IDLE;
  Serial.println("_stateIdle");
  _ledControl.idle();
  _lcd->setCursor(0, 0);
  _lcd->print(DISP_LINE_TITLE);
  _lcd->setCursor(0, 2);
  _lcd->print(DISP_LINE_SELECT);

  _stateAddedCredit();
}

void VendingMachine::_stateInsufficientCredit() {
  Serial.println("_stateInsufficientCredit");

  _updateState &= ~STATE_UPD_INSUFFICIENT_CREDIT;
  if (!_selectedCompartment) {
    return;
  }

  _ledControl.spotlight(_selectedCompartment->number(), 1, 3000);

  _lcd->setCursor(0, 3);
  _lcd->print("Prijs: EUR "); // TODO display euro symbol
  char line[9];
  dtostrf(0.01 * _selectedCompartment->priceInCents(), 3, 2, line);
  _lcd->print(line);
}

void VendingMachine::_stateAddedCredit() {
  Serial.println("_stateAddedCredit");
  _updateState &= ~STATE_UPD_CREDIT;

  _lcd->setCursor(0, 3);
  if (_cents > 0) {
    _lcd->print("Saldo: EUR "); // TODO display euro symbol

    char line[9];
    dtostrf(0.01 * _cents, 3, 2, line);
    _lcd->print(line);
  }
  else {
    _lcd->print(DISP_LINE_EMPTY);
  }
}

void VendingMachine::_stateInvalidSelection() {
  _updateState &= ~STATE_UPD_INVALID_SELECTION;

  Serial.println("_stateInvalidSelection");
  _lcd->setCursor(0, 3);
  _lcd->print(DISP_LINE_INVALID);
}

void VendingMachine::_delayedStateUpdate(byte state, unsigned long delay) {
  _delayedState |= state;
  _stateUpdateTimeout = millis() + delay;
}
