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

#pargma once

namespace IR {

#if 0
struct Config {
	enum {
		carrierFreqDelay = usToTicksCount(1000 * 1000 / 38222),
		leadingBurstDuration = usToTicksCount(9000),
		leadingSilenceDuration_normalData = usToTicksCount(4500),
		leadingSilenceDuration_repeatData = usToTicksCount(2250),
		bitBurstDuration = usToTicksCount(561),
		bit0SilenceDuration = usToTicksCount(561),
		bit1SilenceDuration = usToTicksCount(1687),
	};

};
#endif

#if SUPPORT_PUSH_MACRO
	#pragma push(Self)
#else
	#undef Self
#endif

#define Self NecDecoder
template<typename ConfigArg> struct Self {
	typedef ConfigArg Config;
	Config config;

	enum SendMode {
		SendMode_firstCommand,
		SendMode_repeatCommand
	};

	virtual void onCodeReceive(FU8 addr, FU8 command, SendMode sendMode) = 0;

	STRUCT_DATA_PACK(1)
	struct Packet {
		typedef U8 Addr;
		typedef U8 Command;

		Addr addr;
		Addr notAddr;
		Command command;
		Command notCommand;

		Bool isValid() {
			return this->addr == ~this->notAddr && this->command == ~this->notCommand;
		}
	};

	void onBufferReceive(FU32 inputBuffer, SendMode sendMode) {
		Packet &data = *(Packet *)(&(inputBuffer));
		if(data.isValid()) {
			this->onCodeReceive(data.addr, data.command, sendMode);
		}
		else {

		}
	}

	TicksCount firstTicksCount, prevTicksCount;
	void onPinValueChange(TicksCount currentTicksCount) {
		if(config.carrierFreqDelay < currentTicksCount - prevTicksCount) {
			this->onBurstSeqReceive(this->prevTicksCount - this->firstTicksCount, currentTicksCount - this->prevTicksCount);
			this->firstTicksCount = this->prevTicksCount = currentTicksCount;
		}
		else {
			//# continue receive carrier freq
		}
	}

	U32 inputBuffer;
	FU8 receivedBitsCount;

	void onBurstSeqReceive(TicksCount burstDuration, TicksCount silinceDuration) {
		//# control
		if(config.leadingPreBurstDuration < burstDuration) {
			//# pulse burst. Bits seq start
			if(config.leadingSilenceDuration_normalData < silinceDuration) {
				this->inputBuffer = 0;
				this->receivedBitsCount = 0;
			}
			//# repeat code
			else if(config.leadingSilenceDuration_repeatData < silinceDuration) {
				//# actually received
				if(this->receivedBitsCount == 32) {
					this->onBufferReceive(this->inputBuffer, SendMode_repeatCommand);
				}
				else {
					//# ask for repeat without first command. Ignore. Maybe we missed first command
				}

			}
			else {
				//# unknown code
			}
		}
		//# bit
		else if(config.bitBurstDuration < burstDuration && config.bit0SilenceDuration < silinceDuration) {
			this->inputBuffer <<= 1;
			(config.bit1SilenceDuration < silinceDuration) &&	(this->inputBuffer |= 1);
			this->receivedBitsCount += 1;
			if(this->receivedBitsCount != 32) {
			}
			else {
				this->onBufferReceive(this->inputBuffer, SendMode_firstCommand);
				//# keep as mark that we receive something
				if(0) this->receivedBitsCount = 0;
			}
		}
		else {
			//? Invelid seq. Reset state?
		}
	}

};
#if SUPPORT_PUSH_MACRO
	#pragma pop(Self)
#else
	#undef Self
#endif
#ifdef TESTRUNNER_ON
example(STRING(Self)) {
	struct Config {
		typedef U32 SomeType;

		enum { someArg = 42 };

		static U32 foo1(U32 x) {
			return x;
		}
		U32 foo2(U32 x) {
			return x;
		}
	};

	Self<Config> x;
	assert_eq(x.aabb(), 1);
	assert_not_eq(x.aabb(), 2);
}
#endif

} //# namespace IR
