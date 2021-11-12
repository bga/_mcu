/*
	Copyright 2020 Bga <bga.email@gmail.com>

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#pragma once

#include <!cpp/common.h>
#include <!cpp/bitManipulations.h>
// #include <!cpp/TestRunner.h>

namespace Bga { namespace Mcu { namespace Display { namespace Text { 

#if 0
struct Config {
	Pin::PullHiZ<GPIOD_BaseAddress, 5> m_digit2CathodeGpioPort;
	Pin::PullHiZ<GPIOD_BaseAddress, 6> m_digit1CathodeGpioPort;
	Pin::PullHiZ<GPIOD_BaseAddress, 4> m_digit0CathodeGpioPort;
	Pin::PullHiZ<GPIOA_BaseAddress, 1> m_digit5CathodeGpioPort;
	Pin::PullHiZ<GPIOB_BaseAddress, 4> m_digit4CathodeGpioPort;
	Pin::PullHiZ<GPIOB_BaseAddress, 5> m_digit3CathodeGpioPort;


	Pin::PushPull<GPIOC_BaseAddress, 6> m_digit0AnodeGpioPort;
	Pin::PushPull<GPIOC_BaseAddress, 7> m_digit1AnodeGpioPort;
	Pin::PushPull<GPIOC_BaseAddress, 3> m_digit2AnodeGpioPort;
	Pin::PushPull<GPIOC_BaseAddress, 4> m_digit3AnodeGpioPort;
	Pin::PushPull<GPIOA_BaseAddress, 3> m_digit4AnodeGpioPort;
	Pin::PushPull<GPIOA_BaseAddress, 2> m_digit5AnodeGpioPort;
	Pin::PushPull<GPIOD_BaseAddress, 1> m_digit6AnodeGpioPort;
	Pin::PushPull<GPIOC_BaseAddress, 5> m_digitDotAnodeGpioPort;
};
#endif

#pragma push_macro("Self")
#pragma push_macro("SelfNS")

#undef Self
#undef SelfNS
#define Self _7Segment_CommonCathode
#define SelfNS BGA__CONCAT(Self, NS)

namespace SelfNS { namespace details { 

	template<UInt digitsNoArg, typename ConfigArg> struct Base {
		typedef ConfigArg Config;
		Config m_config;

		enum {
			digitsNo = digitsNoArg, 
		};
		
		FU8 displayChars[digitsNo];
		FU8 currentDisplayIndex;
	};

	template<UInt digitsNoArg, typename ConfigArg> struct CathodeFns;

	template<typename ConfigArg> struct CathodeFns<3, ConfigArg>: public Base<3, ConfigArg> {
		void initCathodes() {
			this->m_config.m_digit0CathodeGpioPort.init();
			this->m_config.m_digit1CathodeGpioPort.init();
			this->m_config.m_digit2CathodeGpioPort.init();
		}
		void turnOffCathodes() {
			this->m_config.m_digit0CathodeGpioPort.hiZ();
			this->m_config.m_digit1CathodeGpioPort.hiZ();
			this->m_config.m_digit2CathodeGpioPort.hiZ();
		}
		void setCathode(FU8 digitIndex) {
			switch(digitIndex) {
				case(0): this->m_config.m_digit0CathodeGpioPort.off(); break;
				case(1): this->m_config.m_digit1CathodeGpioPort.off(); break;
				case(2): this->m_config.m_digit2CathodeGpioPort.off(); break;
			}
		}
	};

	template<typename ConfigArg> struct CathodeFns<6, ConfigArg>: public Base<6, ConfigArg> {
		void initCathodes() {
			this->m_config.m_digit0CathodeGpioPort.init();
			this->m_config.m_digit1CathodeGpioPort.init();
			this->m_config.m_digit2CathodeGpioPort.init();
			this->m_config.m_digit3CathodeGpioPort.init();
			this->m_config.m_digit4CathodeGpioPort.init();
			this->m_config.m_digit5CathodeGpioPort.init();
		}
		void turnOffCathodes() {
			this->m_config.m_digit0CathodeGpioPort.hiZ();
			this->m_config.m_digit1CathodeGpioPort.hiZ();
			this->m_config.m_digit2CathodeGpioPort.hiZ();
			this->m_config.m_digit3CathodeGpioPort.hiZ();
			this->m_config.m_digit4CathodeGpioPort.hiZ();
			this->m_config.m_digit5CathodeGpioPort.hiZ();
		}
		void setCathode(FU8 digitIndex) {
			switch(digitIndex) {
				case(0): this->m_config.m_digit0CathodeGpioPort.off(); break;
				case(1): this->m_config.m_digit1CathodeGpioPort.off(); break;
				case(2): this->m_config.m_digit2CathodeGpioPort.off(); break;
				case(3): this->m_config.m_digit3CathodeGpioPort.off(); break;
				case(4): this->m_config.m_digit4CathodeGpioPort.off(); break;
				case(5): this->m_config.m_digit5CathodeGpioPort.off(); break;
			}
		}
	};
	

} } //# namespace

template<UInt digitsNoArg, typename ConfigArg> struct Self: public SelfNS::details::CathodeFns<digitsNoArg, ConfigArg> {
	void initAnodes() {
		this->m_config.m_digit0AnodeGpioPort.init();
		this->m_config.m_digit1AnodeGpioPort.init();
		this->m_config.m_digit2AnodeGpioPort.init();
		this->m_config.m_digit3AnodeGpioPort.init();
		this->m_config.m_digit4AnodeGpioPort.init();
		this->m_config.m_digit5AnodeGpioPort.init();
		this->m_config.m_digit6AnodeGpioPort.init();
		this->m_config.m_digitDotAnodeGpioPort.init();
	}
	void init() {
		this->initCathodes();
		this->initAnodes();
	}

	void setAnodesState(FU8 digitBitsState) {
		this->m_config.m_digit0AnodeGpioPort.setValue(hasBit(digitBitsState, 0));
		this->m_config.m_digit1AnodeGpioPort.setValue(hasBit(digitBitsState, 1));
		this->m_config.m_digit2AnodeGpioPort.setValue(hasBit(digitBitsState, 2));
		this->m_config.m_digit3AnodeGpioPort.setValue(hasBit(digitBitsState, 3));
		this->m_config.m_digit4AnodeGpioPort.setValue(hasBit(digitBitsState, 4));
		this->m_config.m_digit5AnodeGpioPort.setValue(hasBit(digitBitsState, 5));
		this->m_config.m_digit6AnodeGpioPort.setValue(hasBit(digitBitsState, 6));
		this->m_config.m_digitDotAnodeGpioPort.setValue(hasBit(digitBitsState, 7));
	}

	void turnOffDisplay() {
		this->turnOffCathodes();
	}
	void setDigit(FU8 digitIndex, FU8 digitBitsState) {
		this->turnOffDisplay();

		this->setAnodesState(digitBitsState);
		this->setCathode(digitIndex);
	}
	void update() {
		this->setDigit(this->currentDisplayIndex, this->displayChars[this->currentDisplayIndex]);
		cycleInc(this->currentDisplayIndex, arraySize(this->displayChars));
	}
	void updateManual(FU8 digitIndex) {
		this->setDigit(digitIndex, this->displayChars[digitIndex]);
	}
};

#pragma pop_macro("SelfNS")
#pragma pop_macro("Self")

 } } } } //# namespace
