#define STB 4
#define CLK 16
#define DIO 17

#include "TM1638_functions.h"
#include "characters.h"

/*

  --- COMMANDS ---
    i) Activate board and set brightness - 0x8(0->F)
      arg: none
    ii) Single Address Mode              - 0x44
      arg: 0xC(4-bit addr), 0x(8-bit data)
    iii) Address Auto Increment Mode     - 0x40
      arg: 0xC(start addr), 0x(data1), 0x(data2)
    iv) Read Buttons                     - 0x42
      arg: none

  --- ADDRESSES (left to right) ---
    LED: 0xC1, 0xC3, 0xC5, 0xC7, 0xC9, 0xCB, 0xCD, 0xCF
    DISP: 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE

  --- 7 Segment Display ---
        A
      F   B
        G
      E   C
        D   Dp

    0xDpGFEDCBA

  --- Buttons ---
    +----------+---+---+---+----+---+---+---+----+
    | Byte/Bit | 7 | 6 | 5 | 4  | 3 | 2 | 1 | 0  |
    +----------+---+---+---+----+---+---+---+----+
    |        1 | x | x | x | S5 | x | x | x | S1 |
    |        2 | x | x | x | S6 | x | x | x | S2 |
    |        3 | x | x | x | S7 | x | x | x | S3 |
    |        4 | x | x | x | S8 | x | x | x | S4 |
    +----------+---+---+---+----+---+---+---+----+
    Example: button S5
    Byte 1: 0x10
    Byte 2: 0x00
    Byte 3: 0x00
    Byte 4: 0x00

*/

int brightness = 0;
int button_states = 0b00000000;
bool screen_state = 1;

byte getBit(int number, int bitPos) {
  return !!(number & (1 << bitPos));
}

void flipBit(int *number, int bitPos) {
  *number ^= (1 << bitPos);
}

void setup() {
  pinMode(STB, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  TM1638_Init();
  setDisplay(0, 0x76);setDisplay(1, 0x79);setDisplay(2, 0x38);setDisplay(3, 0x38);setDisplay(4, 0x3F);
  setLED(brightness, 1);
}

void loop() {
  int buttons = readButtons();

  if (getBit(buttons, 7) != getBit(button_states, 7)) {
    flipBit(&button_states, 7);
    brightness += getBit(buttons, 7);
    if (brightness > 7) brightness = 7;
    setLED(brightness, 1);
  }
  if (getBit(buttons, 6) != getBit(button_states, 6)) {
    flipBit(&button_states, 6);
    brightness -= getBit(buttons, 6);
    if (brightness < 0) brightness = 0;
    setLED(brightness + 1, 0);
  }
  if (getBit(buttons, 5) != getBit(button_states, 5)) {
    if (getBit(buttons, 5) == 1) screen_state = !screen_state; // Only on button press and not on button release
    flipBit(&button_states, 5);
  }
  
  sendCommand(screen_state ? 0x80 | (brightness + 0x8) : 0x80);
}
