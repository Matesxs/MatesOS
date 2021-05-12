#include "keyboard.h"

bool isLeftShiftPressed = false;
bool isRightShiftPressed = false;
void HandleKeyboard(uint8_t scancode)
{
  switch (scancode)
  {
    case LeftShift:
      isLeftShiftPressed = true;
      return;

    case LeftShift + 0x80:
      isLeftShiftPressed = false;
      return;

    case RightShift:
      isRightShiftPressed = true;
      return;

    case RightShift + 0x80:
      isRightShiftPressed = false;
      return;

    case Enter:
      // g_BasicRenderer.NewLine();
      return;

    case Spacebar:
      // g_BasicRenderer.PutChar(' ');
      return;

    case BackSpace:
      // g_BasicRenderer.ClearChar();
      return;
  }

  char ascii = QWERTYKeyboard::Translate(scancode, isLeftShiftPressed | isRightShiftPressed);
  if (ascii != 0)
  {
    // g_BasicRenderer.PutChar(ascii);
  }
}