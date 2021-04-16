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

#waring not tested

#include <!cpp/common.h>
#include <!cpp/bitManipulations.h>
#include <!cpp/newKeywords.h>
// #include <!cpp/TestRunner.h>

namespace Protocol {

#if 0
struct Config {
	using ::Bga::Mcu::Hal::Pin::PushPull;
	
	PushPull<PC_BASE_ADDRESS, 1> m_clockPort;
	PushPull<PC_BASE_ADDRESS, 1> m_mosiPort;
	PushPull<PC_BASE_ADDRESS, 1> m_misoPort;
};
#endif

#pragma push_macro("Self")

#undef Self
#define Self ClassName
template<typename ConfigArg> struct Self {
	typedef ConfigArg Config;
	Config m_config;

	Self() {
	}
	
	protected:
	void clockOut() {
		m_config.m_clockPort.on();
		m_config.m_clockPort.off();
	}
	
	public:
	void writeBit(Bool bit) {
		m_config.m_mosiPort.setValue(bit);
		clockOut();
	}
	template<class ValueArg, BGA__TEMPLATE__ENABLE_IF(boost::is_integral(ValueArg)::value)>
	void write_lsb(ValueArg x, UInt bitsCount = sizeof(ValueArg) * 8) {
		const ValueArg hiBit = _BV(bitsCount - 1);
		forInc(FU8, i, 0, bitsCount) {
			writeBit(x & hiBit);
			clockOut();
			x <<= 1;
		}
	}
	template<class ValueArg, BGA__TEMPLATE__ENABLE_IF(boost::is_integral(ValueArg)::value)>
	void write_msb(ValueArg x, UInt bitsCount = sizeof(ValueArg) * 8) {
		forInc(FU8, i, 0, bitsCount) {
			writeBit(x & ValueArg(1));
			x >>= 1;
		}
	}
};
#pragma pop_macro("Self")

} //# namespace Protocol
