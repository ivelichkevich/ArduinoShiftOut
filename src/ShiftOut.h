/* ShiftOut.h - Arduino library that writes out values to an 8 bit shift register (74HC595).
 * You can daisy-chain several shift register in order to set up to 64 pins by only using 3 Arduino pins.
 *
 * Created by Henrik Heine, July 24, 2016
 *
LICENSE
The MIT License (MIT)

Copyright (c) 2016 Henrik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef ShiftOut_h
#define ShiftOut_h
#include "Arduino.h"

struct BaseShiftOut {
    virtual uint16_t getDataWidth() = 0;
    virtual boolean get(int nBit) = 0;
    virtual void set(int nBit, int value) = 0;
    virtual void setHigh(int nBit) = 0;
    virtual void setLow(int nBit) = 0;
    virtual void invert(int nBit) = 0;
    virtual void setAllHigh() = 0;
    virtual void setAllLow() = 0;
    virtual void invert() = 0;
    virtual void setState(uint64_t state) = 0;
    virtual uint64_t getState()  = 0;
    virtual void write() = 0;
    virtual void write(uint64_t data) = 0;
};

template<byte chipCount, typename ShiftType> class _ShiftOut : public BaseShiftOut {
protected:
	byte dataPin;
	byte clockPin;
	byte latchPin;
	const uint16_t dataWidth;
	ShiftType state;
public:
	_ShiftOut(int data, int clock, int latch) : dataWidth(chipCount * 8), state(0) {
		pinMode(dataPin  = data, OUTPUT);
		pinMode(clockPin = clock, OUTPUT);
		pinMode(latchPin = latch, OUTPUT);
	}
	uint16_t getDataWidth() override { return dataWidth; }
	boolean get(int nBit) override { return bitRead(state, nBit); }
	void set(int nBit, int value) override { if(value == 0) { setLow(nBit); } else { setHigh(nBit); } }
	void setHigh(int nBit) override { state = bitSet(state, nBit); }
	void setLow(int nBit) override { state = bitClear(state, nBit); }
	void invert(int nBit) override { set(nBit, !get(nBit)); }
	void setAllHigh() override { state = ~0; }
	void setAllLow() override { state = 0; }
	void invert() override { state = ~state; }
    void setState(uint64_t state) override { this->state = (ShiftType) state; }
    uint64_t getState() override { return (uint64_t) state; }
    void write() {
		for(byte i = 0; i < chipCount; i++) { shiftOut(dataPin, clockPin, MSBFIRST, (byte)(state >> (8 * i))); }
		digitalWrite(latchPin, HIGH); 
		digitalWrite(latchPin, LOW);
	}
	void write(uint64_t data) override {
		state = (ShiftType) data;
        write();
	}
};
/** fallback with 64 bit state (up to 8 shift registers in chipCount param) */
template<byte chipCount> class ShiftOut : public _ShiftOut<chipCount, uint64_t> {
    public: ShiftOut(int data, int clock, int latch) : _ShiftOut<chipCount, uint64_t>(data, clock, latch) {}
};
template<> class ShiftOut<1> : public _ShiftOut<1, uint8_t>  {
    public: ShiftOut(int data, int clock, int latch) : _ShiftOut(data, clock, latch) {}
};
template<> class ShiftOut<2> : public _ShiftOut<2, uint16_t> {
    public: ShiftOut(int data, int clock, int latch) : _ShiftOut(data, clock, latch) {}
};
template<> class ShiftOut<3> : public _ShiftOut<3, uint32_t> {
    public: ShiftOut(int data, int clock, int latch) : _ShiftOut(data, clock, latch) {}
};
template<> class ShiftOut<4> : public _ShiftOut<4, uint32_t> {
    public: ShiftOut(int data, int clock, int latch) : _ShiftOut(data, clock, latch) {}
};
#endif