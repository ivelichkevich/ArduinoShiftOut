#include <ShiftOut.h>

BaseShiftOut* shift;
int led = 0;

void setup() {
	Serial.begin(9600);
	shift = shiftOutFactory(5, 4, 6, 5);
}
void loop() {
	shift->setAllLow();   // set all to zero
	shift->setHigh(led); // set led to high
	shift->write();     // write out to 74HC595
	led = (led + 1) % shift->getDataWidth(); // increment led
	delay(250); // wait a bit
}