#ifndef StepperBoard_h
  #define StepperBoard_h

  #include "AccelStepper.h"

  #ifndef NUM_STEPPERS
    #define NUM_STEPPERS 4
  #endif

  class StepperBoard {
  public:
    static const char STEP_MODE_FULL = 0;
    static const char STEP_MODE_HALF = 1;
    static const char STEP_MODE_QUARTER = 2;
    static const char STEP_MODE_1_8 = 3;
    static const char STEP_MODE_1_16 = 4;
    static const char STEP_MODE_1_32 = 5;
    static const char STEPS_PER_ROTATION_FULL_STEP = 200;

    StepperBoard(uint8_t *stepPins, uint8_t *directionPins);
    void loop();

    long currentPosition(uint8_t stepperIndex);
    void resetCurrentPosition(uint8_t stepperIndex);
    void forward(uint8_t stepperIndex);
    bool movingForward(uint8_t stepperIndex);
    void backward(uint8_t stepperIndex);
    bool movingBackward(uint8_t stepperIndex);

    void stepMode(uint8_t stepperIndex, const char stepMode);
    char stepMode(uint8_t stepperIndex);

    // ENABLE LOW = ENABLE, HIGH = DISABLE
    void enable(uint8_t stepperIndex, bool set);
    bool enable(uint8_t stepperIndex);

    // RESET LOW = DISABLE, HIGH = ENABLE
    void reset(uint8_t stepperIndex, bool set);
    bool reset(uint8_t stepperIndex);

    // SLEEP LOW = SLEEP, HIGH = ACTIVE
    void sleep(uint8_t stepperIndex, bool set);
    bool sleep(uint8_t stepperIndex);

    void writeDriverConfiguration();
    void debugFlags(byte x);
    uint32_t _out;
  private:
    uint32_t _previousOut;
    uint16_t _stepsPerRotation[NUM_STEPPERS];
    AccelStepper _steppers[NUM_STEPPERS];

    void _m0(uint8_t stepperIndex, bool set);
    bool _m0(uint8_t stepperIndex);
    void _m1(uint8_t stepperIndex, bool set);
    bool _m1(uint8_t stepperIndex);
    void _m2(uint8_t stepperIndex, bool set);
    bool _m2(uint8_t stepperIndex);
    void _updateStepsPerRotation(uint8_t stepperIndex, const char stepMode);
    void _shiftOut(uint8_t val);
  };

#endif
