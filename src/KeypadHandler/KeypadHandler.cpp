#include "KeypadHandler.h"

KeypadHandler::KeypadHandler(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) : Keypad(userKeymap, row, col, numRows, numCols) {

}

void KeypadHandler::setDelegate(KeypadHandlerDelegate *delegate) {
  _delegate = delegate;
}

void KeypadHandler::loop() {
  char keypressed = getKey();

  switch (keypressed) {
    case NO_KEY:
      break;
    case '*':
      _collectInput = true;
      break;
    case '#':
      _processCode();
      break;
    default:
      // TODO move logic to code handler.
      if (_collectInput) {
        if (_inputChars < 15) {
          _input[_inputChars++] = keypressed;
          if (_delegate) _delegate->handleEnteringCode(_input);
        }
        else {
          _processCode();
        }
      }
      else {
        _processDigit(keypressed);
      }
      break;
  }

}

void KeypadHandler::_processCode() {
  _input[_inputChars++] = '\0';

  if (_delegate) _delegate->handleEnteredCode(_input);

  memset(_input, 0, sizeof _input);
  _inputChars = 0;
  _collectInput = false;
}

void KeypadHandler::_processDigit(const char keypressed) {
  if (_delegate) _delegate->handleDigit(keypressed);
}
