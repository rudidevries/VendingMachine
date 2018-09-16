#ifndef KeypadHandlerDelegate_H
  #define KeypadHandlerDelegate_H

  class KeypadHandlerDelegate {
    public:
       virtual void handleDigit(char digit);
       virtual void handleEnteredCode(char *code);
       virtual void handleEnteringCode(char *code);
  };

#endif
