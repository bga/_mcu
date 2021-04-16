//# to C++ convert by
/*
	Copyright 2021 Bga <bga.email@gmail.com>

	This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

//# Original code by
/*
	Copyright © 2006-2008 Hans-Christoph Steiner. All rights reserved. Copyright (c) 2010 Arduino LLC. All right reserved.

	This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once

#include <!cpp/common.h>
#include <!cpp/bitManipulations.h>
#include <!cpp/newKeywords.h>

#pragma push_macro("Self")

#undef Self
#define Self HD44780

namespace Bga { namespace Mcu { namespace Display { namespace Self {

enum {
	CharSize_function_5x10 = 0x04,
	CharSize_function_5x8 = 0x00,
};

#if 0
struct Config {
	enum  {
		i2cAddr = 0x3F,

		displayColsCount = 16,
		displayRowsCount = 2,

		displayCharSize = CharSize_function_5x8,
	};

	struct Port {
		unsigned rsPin: 1; //# Read/Write bit
		unsigned rwPin: 1; //# Read/Write bit
		unsigned enPin: 1; //# Enable bit
		unsigned backlightPin: 1; //# flags for backlight control
		unsigned data: 4;
	};

	void writePort(Port data) {
		Wire.beginTransmission(this->i2cAddr);
		Wire.write(reinterpret_cast<U8>(data));
		Wire.endTransmission();
	}
};
#endif

struct PCF8574_I2C_BaseConfig {
	enum  {
		i2cAddr = 0x3F,
	};

	#pragma pack(push, 1)
	union Port {
		struct {
			unsigned rsPin: 1;
			unsigned rwPin: 1;
			unsigned enPin: 1;
			unsigned backlightPin: 1;
			unsigned data: 4;
		};
		U8 raw;
	};
	#pragma pack(pop)

	static_assert_eq(sizeof(Port), 1);

	void writePort(Port data) {
    // I2c_writeByte(i2cAddr, data);
	}
};


template<typename ConfigArg> struct Self {
	typedef ConfigArg Config;

	typedef typename Config::Port ConfigPort;

	Config m_config;

	static_assert_lte(Config::displayRowsCount, 4);
	static_assert_test(Config::displayCharSize, x == CharSize_function_5x8 || x == CharSize_function_5x10);

	enum {
		//# commands
		Cmd_clearDisplay = 0x01,
		Cmd_returnHome = 0x02,
		Cmd_entryModeSet = 0x04,
		Cmd_displayControl = 0x08,
		Cmd_cursorShift = 0x10,
		Cmd_functionSet = 0x20,
		Cmd_setCgramAddr = 0x40,
		Cmd_setDdramAddr = 0x80,

		//# flags for display/cursor shift
//		LCD_CURSORMOVE = 0x00,
		Cmd_cursorShift_displayMove = Cmd_cursorShift | 0x08,
		Cmd_cursorShift_displayMove_moveRight = Cmd_cursorShift_displayMove | 0x04,
		Cmd_cursorShift_displayMove_moveLeft  = Cmd_cursorShift_displayMove | 0x00,

		//# flags for function set
		Cmd_function_8bitMode = 0x10,
		Cmd_function_4bitMode = 0x00,
		Cmd_function_2line = 0x08,
		Cmd_function_1line = 0x00,
	};


	union DisplayControlState {
		FU8 raw;
		struct {
			FU8 isCursorBlink: 1;
			FU8 isCursorOn: 1;
			FU8 isDisplayOn: 1;
		};
	} m_displayControl_bit;

	enum Lcd_entryDirection {
		Lcd_entryDirection_rightToLeft = 0,
		Lcd_entryDirection_leftToRight = 1,
	};
	enum Lcd_entryShift {
		Lcd_entryShift_decrement = 0,
		Lcd_entryShift_increment = 1,
	};

	struct DisplayMode {
		FU8 raw;
		struct {
			FU8 entryShift: 1;
			FU8 entryDirection: 1;
		};
	} m_displayMode_bit;

	FU8 m_backlightPinVal;


	//# When the display powers up, it is configured as follows:
	//
	//# 1. Display clear
	//# 2. Function set:
	//#    DL = 1; 8-bit interface data
	//#    N = 0; 1-line display
	//#    F = 0; 5x8 dot character font
	//# 3. Display on/off control:
	//#    D = 0; Display off
	//#    C = 0; Cursor off
	//#    B = 0; Blinking off
	//# 4. Entry mode set:
	//#    I/D = 1; Increment by 1
	//#    S = 0; No shift
	//
	//# Note, however, that resetting the Arduino doesn't reset the LCD, so we
	//# can't assume that its in that state when a sketch starts (and the
	//# LiquidCrystal constructor is called).

	void init() {
		m_backlightPinVal = 1;

		FU8 displayFunction = Cmd_function_4bitMode | Cmd_function_1line | CharSize_function_5x8;

		if(1 < m_config.displayRowsCount) {
			displayFunction |= Cmd_function_2line;
		};

		//# for some 1 line displays you can select a 10 pixel high font
		if(m_config.displayCharSize != 0 && m_config.displayRowsCount == 1) {
			displayFunction |= CharSize_function_5x10;
		};

		//# SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
		//# according to datasheet, we need at least 40ms after power rises above 2.7V
		//# before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
		delay_ms(50);

		//# Now we pull both RS and R/W low to begin commands
		block { ConfigPort x; x.raw = 0; writeRaw(x); }	//# reset expanderand turn backlight off (Bit 8 =1)
		delay_ms(1000);

		//# put the LCD into 4 bit mode
		//# this is according to the hitachi HD44780 datasheet
		//# figure 24, pg 46

		forInc(FU8, i, 0, 3) {
			block { ConfigPort x; x.data = 0x03; writeNibble(x); }
			delay_us((i == 2) ? 150 : 4500); //# wait min 4.1ms
		}

		//# set to 4-bit interface
		block { ConfigPort x; x.data = 0x02; writeNibble(x); }

		//# set # lines, font size, etc.
		sendCommand(Cmd_functionSet | displayFunction);

		//# turn the display on with no cursor or blinking default
//		displayControl_bit.raw = Cmd_displayControl | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		m_displayControl_bit.raw = Cmd_displayControl;
		m_displayControl_bit.isDisplayOn = 1;
		sendDisplayControl();

		//# clear it off
		clear();

		//# Initialize to default text direction (for roman languages)
		m_displayMode_bit.raw = Cmd_entryModeSet;
		m_displayMode_bit.entryDirection = Lcd_entryDirection_leftToRight;
		m_displayMode_bit.entryShift = Lcd_entryShift_increment;

		//# set the entry mode
		sendDisplayMode();

		moveCursorToHome();
	}

	public:

	//# clear display, set cursor position to zero
	void clear() {
		sendCommand(Cmd_clearDisplay);
		delay_us(2000);
	}

	//# set cursor position to zero
	void moveCursorToHome() {
		sendCommand(Cmd_returnHome);
		delay_us(2000);
	}

	void setCursor(FU8 colNo, FU8 rowNo) {
		if(m_config.displayRowsCount <= rowNo) {
			rowNo = m_config.displayRowsCount - 1;  //# we count rows starting w/0
		};

		const FU8 rowOffsets[] = { 0x00, 0x40, 0x14, 0x54 };
		sendCommand(Cmd_setDdramAddr | (colNo + rowOffsets[rowNo]));
	}


	private: void sendDisplayControl() {
		sendCommand(/* LCD_CMD_DISPLAYCONTROL | */ m_displayControl_bit.raw);
	}

	public:
	Bool get_enableTextDisplay() {
		return m_displayControl_bit.isDisplayOn;
	}
	void set_enableTextDisplay(Bool isOn) {
		m_displayControl_bit.isDisplayOn = isOn;
		sendDisplayControl();
	}

	Bool get_enableCursorDisplay() {
		return m_displayControl_bit.isCursorOn;
	}
	void set_enableCursorDisplay(Bool isOn) {
		m_displayControl_bit.isCursorOn = isOn;
		sendDisplayControl();
	}

	Bool get_enableCursorBlink() {
		return m_displayControl_bit.isCursorBlink;
	}
	void set_enableCursorBlink(Bool isOn) {
		m_displayControl_bit.isCursorBlink = isOn;
		sendDisplayControl();
	}

	//# These commands scroll the display without changing the RAM
	void scrollDisplayLeft(void) {
//		command(Cmd_cursorShift | LCD_CMD_CURSORSHIFT_DISPLAYMOVE | LCD_CMD_CURSORSHIFT_DISPLAYMOVE_MOVELEFT);
		sendCommand(Cmd_cursorShift_displayMove_moveLeft);
	}
	void scrollDisplayRight(void) {
//		command(Cmd_cursorShift | LCD_CMD_CURSORSHIFT_DISPLAYMOVE | LCD_CMD_CURSORSHIFT_DISPLAYMOVE_MOVERIGHT);
		sendCommand(Cmd_cursorShift_displayMove_moveRight);
	}

	private: void sendDisplayMode() {
		sendCommand(/* LCD_CMD_ENTRYMODESET | */ m_displayMode_bit.raw);

	}

	public:
	#if 0
	enum DisplayOrientation {
		DisplayOrientation_leftToRight = 1,
		DisplayOrientation_rightToLeft = 0,
	};
	#endif
	//# This is for text that flows Left to Right
	Lcd_entryDirection get_orientation() {
		return m_displayMode_bit.entryDirection;
	}
	void set_orientation(Lcd_entryDirection entryDirection) {
		m_displayMode_bit.entryDirection = entryDirection;
		sendDisplayMode();
	}

	//# This will 'right justify' text from the cursor
	Lcd_entryShift get_autoscroll() {
		return m_displayMode_bit.entryShift;
	}
	void set_autoscroll(Lcd_entryShift v) {
		m_displayMode_bit.entryShift = v;
		sendDisplayMode();
	}

	//# Allows us to fill the first 8 CGRAM locations
	//# with custom characters
	void loadCustomChar(FU8 location, const U8 charmap[8]) {
		assert_param(location < 8);
		location &= 0x7; //# we only have 8 locations 0-7
		sendCommand(Cmd_setCgramAddr | (location << 3));
		forInc(FU8, i, 0, 8) {
			sendData(charmap[i]);
		}
	}

	enum BacklightState {
		Backlight_off = 0,
		Backlight_on = 1,
	};

	//# Turn the (optional) backlight off/on
	Bool get_backlight() {
		return m_backlightPinVal;
	}
	void set_backlight(Bool v) {
		m_backlightPinVal = v;
		block { ConfigPort x; x.raw = 0; writeRaw(x); }
	}

	void writeRaw(ConfigPort data) {
		data.backlightPin = m_backlightPinVal;
		m_config.writePort(data);
	}

	void writeNibble(ConfigPort value) {
		// writeRaw(value);
		writeWithPulse(value);
	}

	void writeWithPulse(ConfigPort data) {
		data.enPin = 1;
		writeRaw(data);
		delay_us(1); //# enable pulse must be >450ns

		data.enPin = 0;
		writeRaw(data);
		delay_us(50);	//# commands need > 37us to settle
	}

	void sendCommadOrData(FU8 value, Bool isData) {
		// write4bits((ConfigPort){ .rsPin = unsigned(isRsPin & 1), .data = unsigned(U8(value) >> 4) });
		block {
			ConfigPort x = {  };
			x.rsPin = unsigned(isData & 1);
			x.data = unsigned(U8(value) >> 4);
			writeNibble(x);
		};

		// write4bits((ConfigPort){ .rsPin = unsigned(isRsPin & 1), .data = unsigned(U8(value) & 0xf) });
		block {
			ConfigPort x = {  };
			x.rsPin = unsigned(isData & 1);
			x.data = unsigned(U8(value) & 0xf);
			writeNibble(x);
		};
	}
	inline void sendCommand(FU8 value) {
		sendCommadOrData(value, 0);
	}
	inline void sendData(FU8 value) {
		sendCommadOrData(value, 1);
	}


	void putc(const char c) {
		sendData(c);
	}
	void puts(const char s[]) {
		while(*s) {
			putc(*s);
			s++;
		}
	}

};
#pragma pop_macro("Self")

} //# namespace Self
} //# namespace Display
} //# namespace Mcu
} //# namespace Bga
