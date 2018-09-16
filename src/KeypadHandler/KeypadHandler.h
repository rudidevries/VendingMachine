#ifndef KeyHandler_H
  #define KeyHandler_H

  #include "Keypad.h"
  #include "KeypadHandlerDelegate.h"

  class KeypadHandler : public Keypad {
  public:
    KeypadHandler(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols);
    void setDelegate(KeypadHandlerDelegate *delegate);
    void loop();
  private:
    KeypadHandlerDelegate *_delegate;
    bool _collectInput = false;
    char _input[16];
    byte _inputChars = 0;
    void _processDigit(const char keypressed);
    void _processCode();
  };

#endif
