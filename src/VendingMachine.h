#ifndef VendingMachine_h
  #define VendingMachine_h

  #include "Adafruit_LiquidCrystal.h"
  #include "Compartment.h"
  #include "LedControl.h"
  #include "StepperBoard.h"

  enum State {operational, administrator};

  class VendingMachine {
  protected:
      Adafruit_LiquidCrystal *_lcd;
      LedControl _ledControl;

      StepperBoard _stepperBoard;
      Compartment ** _compartments;
      uint8_t _numCompartments;
      unsigned int _cents;
      byte _updateState;
      byte _delayedState;
      unsigned long _stateUpdateTimeout;
      Compartment *_selectedCompartment;

      Compartment * _compartmentFromChar(const char number);
      Compartment * _compartmentFromInt(const int number);

      void _configureSteppers();
      void _configureLedControl();

      bool _sufficientCredit(Compartment *Compartment);

      void _delayedStateUpdate(byte state, unsigned long delay);
      void _stateIdle();
      void _stateInvalidSelection();
      void _stateInsufficientCredit();
      void _stateDispense();
      void _stateAddedCredit();

      void _moveStepper();
  public:
    State state;
    long _stepperTargetPosition;
    uint8_t _activeStepper;

    VendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, uint8_t numCompartments, Compartment **Compartments);

    void tenCentPulse();
    unsigned int getCentsRemaining();
    void resetDisplay();
    void selectCompartment(const char input);

    void loop();
  };
#endif
