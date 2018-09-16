#include "StepperBoard.h"
#include "Arduino.h"
#include <avr/io.h>

#define PULSE_WIDTH_USEC      1
#define OUT_MASK              0x80000000

#define STEPPER_DATA_WIDTH    8

// BIT POSITIONS FOR STEPPER DATA FLAGS
#define FLAG_OUT_ENABLE       0
#define FLAG_OUT_M0           1
#define FLAG_OUT_M1           2
#define FLAG_OUT_M2           3
#define FLAG_OUT_RESET        4
#define FLAG_OUT_SLEEP        5

#define SPEED 1600.0

#define _flag(flag, set, stepper) \
  if (set) {\
    _out |= (OUT_MASK >> ((stepper * STEPPER_DATA_WIDTH) + flag));\
  }\
  else {\
    _out &= ~(OUT_MASK >> ((stepper * STEPPER_DATA_WIDTH) + flag));\
  }

#define _outFlagged(flag, stepper) (_out & (OUT_MASK >> ((stepper * STEPPER_DATA_WIDTH) + flag)))

// PIN / PORT Values:
// STEPPER_OUT_DATA              23   PA1
// STEPPER_OUT_CLOCK             25   PA3
// STEPPER_OUT_LATCH             27   PA5
// STEPPER_OUT_OE                29   PA7
StepperBoard::StepperBoard(uint8_t *stepPins, uint8_t *directionPins) {
  _out = 0;
  _previousOut = 0;

  // Init parallel out shift register
  DDRA |= (1 << DDA3) | (1 << DDA1) | (1 << DDA5) | (1 << DDA7);

  PORTA &= ~(1 << PA3); // clock
  PORTA |= (1 << PA5); // latch
  PORTA &= ~(1 << PA7); // oe

  for (uint8_t stepperIndex = 0; stepperIndex < NUM_STEPPERS; stepperIndex++) {
    _steppers[stepperIndex] = AccelStepper(AccelStepper::DRIVER, stepPins[stepperIndex], directionPins[stepperIndex]);
    _steppers[stepperIndex].setMaxSpeed(SPEED);
    stepMode(stepperIndex, STEP_MODE_FULL);
    enable(stepperIndex, true);
    sleep(stepperIndex, false);
    reset(stepperIndex, true);
  }
  writeDriverConfiguration();
}

void StepperBoard::loop() {
  for (uint8_t i = 0; i < NUM_STEPPERS; i++) {
    if(!enable(i)) {
      _steppers[i].runSpeed();
    }
  }
}

long StepperBoard::currentPosition(uint8_t stepperIndex) {
  return _steppers[stepperIndex].currentPosition();
}

void StepperBoard::resetCurrentPosition(uint8_t stepperIndex) {
  float speed = _steppers[stepperIndex].speed();
  _steppers[stepperIndex].setCurrentPosition(0);
  _steppers[stepperIndex].setSpeed(speed);
}

void StepperBoard::forward(uint8_t stepperIndex) {
  _steppers[stepperIndex].setSpeed(SPEED);
}

bool StepperBoard::movingForward(uint8_t stepperIndex) {
  return _steppers[stepperIndex].speed() < 0;
}

void StepperBoard::backward(uint8_t stepperIndex) {
  _steppers[stepperIndex].setSpeed(-SPEED);
}

bool StepperBoard::movingBackward(uint8_t stepperIndex) {
  return _steppers[stepperIndex].speed() > 0;
}

void StepperBoard::enable(uint8_t stepperIndex, bool set) {
  _flag(FLAG_OUT_ENABLE, set, stepperIndex);
}

bool StepperBoard::enable(uint8_t stepperIndex)  {
  return _outFlagged(FLAG_OUT_ENABLE, stepperIndex);
}

void StepperBoard::stepMode(uint8_t stepperIndex, const char stepMode) {
  _m0(stepperIndex, (stepMode & 1) > 0);
  _m1(stepperIndex, (stepMode & 2) > 0);
  _m2(stepperIndex, (stepMode & 4) > 0);

  _updateStepsPerRotation(stepperIndex, stepMode);
}

void StepperBoard::_updateStepsPerRotation(uint8_t stepperIndex, const char stepMode) {
  switch(stepMode) {
    case STEP_MODE_FULL:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP;
      break;
    case STEP_MODE_HALF:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP * 2;
      break;
    case STEP_MODE_QUARTER:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP * 4;
      break;
    case STEP_MODE_1_8:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP * 8;
      break;
    case STEP_MODE_1_16:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP * 16;
      break;
    case STEP_MODE_1_32:
      _stepsPerRotation[stepperIndex] = STEPS_PER_ROTATION_FULL_STEP * 32;
      break;
  }
}

char StepperBoard::stepMode(uint8_t stepperIndex) {
  char x = 0;
  x += (_m0(stepperIndex) ? 1 : 0);
  x += (_m1(stepperIndex) ? 2 : 0);
  x += (_m2(stepperIndex) ? 4 : 0);
  return x;
}

void StepperBoard::_m0(uint8_t stepperIndex, bool set) {
  _flag(FLAG_OUT_M0, set, stepperIndex);
}

bool StepperBoard::_m0(uint8_t stepperIndex) {
  return _outFlagged(FLAG_OUT_M0, stepperIndex);
}

void StepperBoard::_m1(uint8_t stepperIndex, bool set) {
  _flag(FLAG_OUT_M1, set, stepperIndex);
}

bool StepperBoard::_m1(uint8_t stepperIndex) {
  return _outFlagged(FLAG_OUT_M1, stepperIndex);
}

void StepperBoard::_m2(uint8_t stepperIndex, bool set) {
  _flag(FLAG_OUT_M2, set, stepperIndex);
}

bool StepperBoard::_m2(uint8_t stepperIndex) {
  return _outFlagged(FLAG_OUT_M2, stepperIndex);
}

void StepperBoard::reset(uint8_t stepperIndex, bool set) {
    _flag(FLAG_OUT_RESET, set, stepperIndex);
}

bool StepperBoard::reset(uint8_t stepperIndex) {
  return _outFlagged(FLAG_OUT_RESET, stepperIndex);
}

void StepperBoard::sleep(uint8_t stepperIndex, bool set) {
  _flag(FLAG_OUT_SLEEP, set, stepperIndex);
}

bool StepperBoard::sleep(uint8_t stepperIndex) {
  return _outFlagged(FLAG_OUT_SLEEP, stepperIndex);
}

void StepperBoard::writeDriverConfiguration() {
  // Only write when something changed.
  if (_out != _previousOut) {
    PORTA &= ~(1 << PA5);
    delayMicroseconds(PULSE_WIDTH_USEC);

    Serial.println(_out, BIN);

    for (int i = 0; i < NUM_STEPPERS; i++) {
      _shiftOut((_out >> (i * STEPPER_DATA_WIDTH)) & 0xFF);
    }

    PORTA |= (1 << PA5);
    _previousOut = _out;
  }
}

void StepperBoard::_shiftOut(uint8_t val) {
  for (uint8_t i = 0; i < 8; i++)  {
    uint8_t b = !!(val & (1 << i));
    if (b) {
      PORTA |= (1 << PA1);
    }
    else {
      PORTA &= ~(1 << PA1);
    }

    PORTA |= (1 << PA3);
    delayMicroseconds(PULSE_WIDTH_USEC);
    PORTA &= ~(1 << PA3);
  }
}

void StepperBoard::debugFlags(byte x) {
  for(byte mask = 0x80; mask; mask >>= 1){
    if(mask & x)
      Serial.print('1');
    else
      Serial.print('0');
  }
  Serial.println();
}
