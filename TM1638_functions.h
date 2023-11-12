#include "modded_functions.h"

const byte leds[8] = { 0xC1, 0xC3, 0xC5, 0xC7, 0xC9, 0xCB, 0xCD, 0xCF };
const byte displays[8] = { 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE };

void sendCommand(byte command, bool keepSTBLow = false) {
  digitalWrite(STB, LOW);
  shiftOutMod(DIO, CLK, LSBFIRST, command);
  if (!keepSTBLow) digitalWrite(STB, HIGH);
}

void reset() {
  sendCommand(0x40); // Set auto increment mode (useful for resetting LEDs and displays)
  sendCommand(0xC0, true); // Set starting address to 0
  for (int i = 0; i < 16; i++) sendCommand(0x00, !(i == 15));
}

void TM1638_Init(byte brightness = 0x0F) {
  // Activate TM1638 and set brightness of display
  // Usage: 0x8(0->F)
  //             ^-- brightness setting
  sendCommand(0x80 | brightness);
  // Resetting
  reset();
  // Set single addres mode
  sendCommand(0x44);
}

void setLED(int n_led, bool value) {
  // Send address
  sendCommand(leds[n_led], true);
  // Send value
  sendCommand(value);
}

void setDisplay(int n_display, byte value) {
  // Send address
  sendCommand(displays[n_display], true);
  // Send value
  sendCommand(value);
}

int readButtons()
{
  int buttons = 0;
  // Set read buttons mode
  sendCommand(0x42, true);
  // Set dio pin as input
  pinMode(DIO, INPUT);
  // Get 4 bytes and store pressed button(s)
  for (int i=0; i<4; i++)
  {
    int val = shiftInMod(DIO, CLK, LSBFIRST) << i;
    buttons |= val;
  }
  // Set dio pin as output and return pressed button(s)
  pinMode(DIO, OUTPUT);
  digitalWrite(STB, HIGH);

  return buttons;
}

void test() {
  #include "characters.h"
  // Send "3" to second display
  setDisplay(2, digits[3]);
  // Turn on LED7
  setLED(7, 1);
  // Read buttons
  int buttons = readButtons();
  for (int i = 0; i <= 7; i++) {
    setLED(i + 1, buttons & (1 << i));
    setDisplay(i + 1, buttons & (1 << i) ? 0xFF : 0);
  }
}
