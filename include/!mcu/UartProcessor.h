#ifndef UART_PROCESSOR_H
#define UART_PROCESSOR_H

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

#include <stdlib.h>

#include <!cpp/MemoryRegion.h>
#include <!cpp/newKeywords.h>
#include <!cpp/TestRunner.h>
#include <!cpp/common.h>
#include <!cpp/bitManipulations.h>


#ifdef TESTRUNNER_ON
	#include <string>
	#include <!cpp/debug.h>
#endif // TESTRUNNER_ON

//namespace UartProcessor {
#if 0
struct Config {
	typedef FU32 ErrorCode;
	typedef FU16 Hash;

	enum {
		nameMaxSize = sizeof(Hash),
		operatorSize = 1,
		dataMaxSize = 32,

		bufferSize = nameMaxSize + operatorSize + dataMaxSize
	};
	void write(FU8 byte) {

	}
	
	static const Hash Hash_initValue = 0;
	Hash Hash_append(Hash hash, U8 ch) {
		return 31 * hash + ch;
	}
};
#endif // 1
//# write { p00=FF00\n } read { p00?\n }
template<typename ConfigArg>
struct UartProcessor {
	typedef ConfigArg Config;
	Config config;

	typedef typename Config::ErrorCode ErrorCode;
	typedef typename Config::Hash Hash;

	virtual ErrorCode readData(Hash hash, MemoryRegion& data) = 0; union {

	};
	virtual ErrorCode writeData(Hash hash, MemoryRegion data) = 0; union {

	};

	enum {
		Error_ok = staticSwapBytes32(makeU32leFrom4U8('O', 'K', '\x00','\x00')),
		Error_bufferOverflow = staticSwapBytes32(makeU32leFrom4U8('#', 'O', 'V', 'F')),
		Error_nameNotFound = staticSwapBytes32(makeU32leFrom4U8('#', 'N', 'A', 'M')),
//		Error_valueSize = staticSwapBytes32(makeU32leFrom4U8('#', 'V', '_', 'S')),
//		Error_valueData = staticSwapBytes32(makeU32leFrom4U8('#', 'V', '_', 'D')),
		Error_valueSize = staticSwapBytes32(makeU32leFrom4U8('#', 'V', 'S', 'I')),
		Error_valueData = staticSwapBytes32(makeU32leFrom4U8('#', 'V', 'D', 'A')),
		Error_unknownOperator = staticSwapBytes32(makeU32leFrom4U8('#', 'O', 'P', '\x00')),
		Error_unknown = staticSwapBytes32(makeU32leFrom4U8('#', 'U', 'N', 'K')),
//		Error_ = '#',
	};

	enum {
		bufferSize = Config::bufferSize
	};

	U8 buffer[bufferSize];
	FU8 bufferPos;

	void init() {
		this->bufferPos = 0;
	}

	static FU8 nibbleToHex(FU8 x) {
		if(x < 10) {
			return '0' + x;
		}
		else {
			return 'A' + (x - 10);
		}
	}
	static FI8 hexToNibble(FU8 x) {
		('a' <= x) ? (x -= 'a' - 'A') : 0;

		if('A' <= x && x <= 'F') {
			return x - 'A' + 10;
		}
		else if('0' <= x && x <= '9') {
			return x - '0' + 0;
		}
		else {
			return -1;
		}
	}
	static FI16 hexToByte(FU16 hex) {
		FI8 hiNibble = hexToNibble(hex & 0xFF);
		FI8 lowNibble = hexToNibble((hex >> 8) & 0xFF);
		if(lowNibble != -1 && hiNibble != -1) {
			return lowNibble | (hiNibble << 4);
		}
		else {
			return -1;
		}
	}
	void writeHexNibble(FU8 x) {
		this->config.write(nibbleToHex(x & 0x0F));
	}
	void writeHex(FU8 x) {
		writeHexNibble(x >> 4);
		writeHexNibble(x);
	}
	void reportError(ErrorCode errorCode) {
		this->config.write('\n');
		#if 0
		forInc(FU8, i, 0, 4) {
			this->config.write(reinterpret_cast<U8 *>(&(errorCode))[3 - i]);
		}
		#elif 0
		for(FU8 i = 4; i--;) {
			this->config.write(reinterpret_cast<U8 *>(&(errorCode))[i]);
		}
		#elif 1
		while(errorCode != 0) {
			this->config.write(U8(errorCode));
			errorCode >>= 8;
		}
		#endif // 0
	}

	void read(FU8 ch) {
		if(bufferSize <= this->bufferPos) {
			this->reportError(Error_bufferOverflow);
			this->bufferPos = 0;
		}
		else if(ch != '\n') {
			this->buffer[this->bufferPos++] = ch;
		}
		else {
			FU32 errorCode = Error_unknown;
			FU8 i = 0;

//			FU32 hash = ((FU32 *)this->buffer)[0];
			Hash hash = config.Hash_initValue;
			while(i < this->bufferPos && this->buffer[i] != '?' && this->buffer[i] != '=') {
				hash = config.Hash_append(hash, this->buffer[i]);
				i += 1;
			}
			FU8 hashEnd = i;
			FU8 op = this->buffer[hashEnd];
			FU8 dataStart = hashEnd + 1;
//			this->buffer[hashEnd] = 0;
			//# read
			if(op == '?') {
				MemoryRegion rData = { .dataSize = 0, .data = &(this->buffer[dataStart]) };
				errorCode = this->readData(hash, rData);
				if(errorCode == Error_ok) {
					for(i = 0; i != rData.dataSize; i += 1) {
						writeHex(rData.data[i]);
					}
				}
				else {
				}
			}
			//# write
			else if(op == '=') {
				#if 0
				class ParseError {

				};
				try {

				}
				catch(const ParseError& err) {

				}
				#else
				FU8 dataSizeInNibbles = this->bufferPos  - dataStart;
				MemoryRegion wData = {
					.dataSize = dataSizeInNibbles / 2,
					.data = &(this->buffer[dataStart])
				};
				//# alreay error if nibbles count is odd
				FU8 errorsCount = dataSizeInNibbles & 1;
				//# TODO good syntax?
//				#define assignIf if
//				assignIf(dataSizeInNibbles & 1) {
//					errorsCount += 1;
//				}
				for(i = 0; i < wData.dataSize; i += 1) {
					FU16 hex = (reinterpret_cast<U16 *>(wData.data))[i];
					FI16 byte = hexToByte(hex);
					if(byte != -1) {
						wData.data[i] = byte;
					}
					else {
						errorsCount += 1;
					}
				}

				//# is we reached data' end
//				errorsCount += (dataSizeInNibbles - 2 * i);
				if(errorsCount != 0) {
					errorCode = Error_valueData;
				}
				else {
					errorCode = this->writeData(hash, wData);
				}
				#endif // 1


			}
			else {
				errorCode = Error_unknownOperator;
			}

			this->bufferPos = 0;
			this->reportError(errorCode);
		}
	}
};

#ifdef TESTRUNNER_ON
example("UartProcessor") {
	struct __ {
		std::string trimRight(const std::string& str) {
			size_t endpos = str.find_last_not_of(" \t");
//			size_t startpos = str.find_first_not_of(" \t");
			return ((std::string::npos != endpos) ? str.substr(0, endpos + 1) : str);
		}

		U16 swapBytes(U16 x) {
			return (U8(x) << 8) | U8(x >> 8);
		}
	} _;

	struct UartProcessorConfig {
		typedef FU32 ErrorCode;
		typedef FU16 Hash;

		enum {
			hashMaxSize = sizeof(Hash),
			operatorSize = 1,
			dataMaxSize = 32,

			bufferSize = hashMaxSize + operatorSize + dataMaxSize
		};

		std::string txBuffer;

		void write(FU8 byte) {
			txBuffer.push_back(byte);
		}

		const Hash Hash_initValue = 0; //# c++ forbids static consts in local classes
		Hash Hash_append(Hash hash, U8 ch) {
			return ch | (hash << 8);
		}
	};


	struct AppUartProcessor: UartProcessor<UartProcessorConfig> {
		U16 aParam;
		U16 bParam;
		ErrorCode readData(Hash hash, MemoryRegion& data) override {
			switch(hash) {
				case('a'): return (data.data = reinterpret_cast<U8 *>(&(this->aParam))), (data.dataSize = sizeof(this->aParam)), Error_ok;
				default: return Error_nameNotFound;
			}

		}
		ErrorCode writeData(Hash hash, MemoryRegion data) override {
			Debug_print("%i", data.dataSize);
			switch(hash) {
				case('a'): return (data.dataSize == sizeof(this->aParam)) ? (this->aParam = *reinterpret_cast<U16 *>(data.data), Error_ok) : Error_valueSize;
				case('ab'): return (data.dataSize == sizeof(this->aParam)) ? (this->aParam = *reinterpret_cast<U16 *>(data.data), Error_ok) : Error_valueSize;
				default: return Error_nameNotFound;
			}
		}

		Bool readString(const char* cmd) {
			while(*cmd) {
				this->read(*cmd);
				cmd += 1;
			}

			return *cmd == 0;
		}
	};

	AppUartProcessor appUartProcessor;
	appUartProcessor.init();

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("a=ABCD\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\nOK"));
	assert_eq(appUartProcessor.aParam, _.swapBytes(0xABCD));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("ab=ABCD\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\nOK"));
	assert_eq(appUartProcessor.aParam, _.swapBytes(0xABCD));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("a?\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("ABCD\nOK"));


	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("a=AB\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#VSI"));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("x=AB\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#NAM"));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("x?\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#NAM"));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("x=ABA\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#VDA"));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("a$\n"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#OP"));

	appUartProcessor.config.txBuffer.clear();
	assert_eq(appUartProcessor.readString("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), yes);
	assert_eq(_.trimRight(appUartProcessor.config.txBuffer), std::string("\n#OVF"));

}
#endif // TESTRUNNER_ON

//} // namespace UartProcessor
#endif
