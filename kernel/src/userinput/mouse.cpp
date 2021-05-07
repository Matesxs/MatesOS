#include "mouse.h"

uint8_t MousePointer[] = {
  0b11111111, 0b11100000,
  0b11111111, 0b10000000,
  0b11111110, 0b00000000,
  0b11111100, 0b00000000,
  0b11111000, 0b00000000,
  0b11110000, 0b00000000,
  0b11100000, 0b00000000,
  0b11000000, 0b00000000,
  0b11000000, 0b00000000,
  0b10000000, 0b00000000,
  0b10000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
};

void MouseWait()
{
  uint64_t timeout = 100000;
  
  while (timeout--)
  {
    if ((ReadBus(0x64) & 0b10) == 0)
    {
      return;
    }
  }
}

void MouseWaitInput()
{
  uint64_t timeout = 100000;
  
  while (timeout--)
  {
    if (ReadBus(0x64) & 0b1)
    {
      return;
    }
  }
}

void MouseWrite(uint8_t value)
{
  MouseWait();
  WriteBus(0x64, 0xD4);
  MouseWait();
  WriteBus(0x60, value);
}

uint8_t MouseRead()
{
  MouseWaitInput();
  return ReadBus(0x60);
}

void InitPS2Mouse()
{
  WriteBus(0x64, 0xA8); // Enable aux device - mouse
  MouseWait();
  WriteBus(0x64, 0x20); // Keyboard controller - talking to mouse
  MouseWaitInput();

  uint8_t status = ReadBus(0x60);
  status |= 0b10;
  MouseWait();
  WriteBus(0x64, 0x60);
  MouseWait();
  WriteBus(0x60, status); // Setting compaq status byte

  MouseWrite(0xF6); // Setting defaults
  MouseRead();

  MouseWrite(0xF3); // Setting 200 sample rate
  MouseRead();
  MouseWrite(0xC8);
  MouseRead();

  MouseWrite(0xF4); // Enable data reporting
  MouseRead();
}

uint8_t mouseCycle = 0;
uint8_t mousePacket[4];
bool mousePacketReady = false;
void HandlePS2Mouse(uint8_t data)
{
  ProcessMousePacket();
  static bool skip = true;
  if (skip) { skip = false; return; }

  switch (mouseCycle)
  {
    case 0:
      if ((data & 0b00001000) == 0) break;
      mousePacket[0] = data;
      mouseCycle++;
      break;

    case 1:
      mousePacket[1] = data;
      mouseCycle++;
      break;

    case 2:
      mousePacket[2] = data;
      mousePacketReady = true;
      mouseCycle = 0;
      break;
  }
}

Point MousePosition;
Point MousePositionOld;
void ProcessMousePacket()
{
  if (!mousePacketReady) return;

  bool xNegative, yNegative, xOverflow, yOverflow;

  if (mousePacket[0] & PS2XSign)
  {
    xNegative = true;
  }
  else
  {
    xNegative = false;
  }

  if (mousePacket[0] & PS2YSign)
  {
    yNegative = true;
  }
  else
  {
    yNegative = false;
  }

  if (mousePacket[0] & PS2XOverflow)
  {
    xOverflow = true;
  }
  else
  {
    xOverflow = false;
  }

  if (mousePacket[0] & PS2YOverflow)
  {
    yOverflow = true;
  }
  else
  {
    yOverflow = false;
  }

  if (!xNegative)
  {
    MousePosition.X += mousePacket[1];
    if (xOverflow)
    {
      MousePosition.X += 255;
    }
  }
  else
  {
    mousePacket[1] = 256 - mousePacket[1];
    MousePosition.X -= mousePacket[1];
    if (xOverflow)
    {
      MousePosition.X -= 255;
    }
  }

  if (!yNegative)
  {
    MousePosition.Y -= mousePacket[2];
    if (yOverflow)
    {
      MousePosition.Y -= 255;
    }
  }
  else
  {
    mousePacket[2] = 256 - mousePacket[2];
    MousePosition.Y += mousePacket[2];
    if (yOverflow)
    {
      MousePosition.Y += 255;
    }
  }

  mousePacketReady = false;

  if (MousePosition.X < 0) MousePosition.X = 0;
  if (MousePosition.X > GlobalBasicRenderer.GetWidth() - 1) MousePosition.X = GlobalBasicRenderer.GetWidth() - 1;
  if (MousePosition.Y < 0) MousePosition.Y = 0;
  if (MousePosition.Y > GlobalBasicRenderer.GetHeight() - 1) MousePosition.Y = GlobalBasicRenderer.GetHeight() - 1;

  GlobalBasicRenderer.ClearMouseCursor(MousePointer, MousePositionOld);
  GlobalBasicRenderer.DrawOverlayMouseCursor(MousePointer, MousePosition, __CURSOR_COLOR);

  if (mousePacket[0] & PS2LeftButton)
  {
  }

  if (mousePacket[0] & PS2MiddleButton)
  {
  }

  if (mousePacket[0] & PS2RightButton)
  {
  }

  MousePositionOld = MousePosition;
}