/*

  Mods for Arduino IDE's built-in functions shiftIn() and shiftOut() are required for this controller
  because the ESP32 operates too fast

*/

byte shiftInMod(byte dataPin, byte clockPin, byte bitOrder, byte clockType = 1, short clockDelay_us = 1) {
  byte value = 0;

	for (int i = 0; i < 8; i++) {
		digitalWrite(clockPin, (clockType ? LOW : HIGH)); // Send clock signal
    delayMicroseconds(clockDelay_us); // Add necessary delay for the ESP32
    // Read the bit from the data pin and store it into value
		if (bitOrder == LSBFIRST)
			value |= digitalRead(dataPin) << i;
		else
			value |= digitalRead(dataPin) << (7 - i);
    // Send clock signal again and repeat until 8 bits are read
		digitalWrite(clockPin, (clockType ? HIGH : LOW));
	}

	return value;
}

void shiftOutMod(byte dataPin, byte clockPin, byte bitOrder, byte value, byte clockType = 1, short clockDelay_us = 1) {
  for (int i = 0; i < 8; i++)  {
    // Send bit to data pin
		if (bitOrder == LSBFIRST) digitalWrite(dataPin, !!(value & (1 << i)));
		else digitalWrite(dataPin, !!(value & (1 << (7 - i))));
    // Pulse the clock, add necessary delay for the ESP32 and repeat until all 8 bits are sent
		digitalWrite(clockPin, (clockType ? LOW : HIGH));
    delayMicroseconds(clockDelay_us);
		digitalWrite(clockPin, (clockType ? HIGH : LOW));		
	}
}