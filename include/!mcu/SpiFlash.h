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

#error Not tested

#pragma once

#include <!cpp/common.h>
// #include <!cpp/TestRunner.h>

#if 0
struct Config {
	static FU8 spiRead() {
		return x;
	}
	static void spiWrite(FU8 data) {
	}
	static void chipSelect(Bool isSelect) {
	}
	static void sleepMs(FU16 ms) {
	}
};
#endif

#pragma push_macro("Self")

#undef Self
#define Self ClassName
template<typename ConfigArg> struct Self {
	typedef ConfigArg Config;
	Config config;
	
	Self() {
	}
	
	enum {
		CMD_READJEDECID = 0x9F,
		CMD_READSTATUSREGISTER = 0x05,
		
		CMD_WRITE_ENABLE = 0x06,
		CMD_WRITE_DISABLE = 0x04,
		
		CMD_READ = 0x03,
		CMD_ARRAYREAD = 0x0B,
		CMD_PAGEPROGRAM = 0x02,
		
		CMD_SECTOR_ERASE = 0x20,
		CMD_BLOCK_ERASE_32K = 0x52,
		CMD_BLOCK_ERASE_64K = 0xD8,
		CMD_CHIP_ERASE = 0x60,
		
		CMD_DEEP_POWERDOWN = 0xB9,
		CMD_RELEASE_DEEP_POWERDOWN = 0xAB,
		
		STATUS_REG_SRP = (1 << 7),
		STATUS_REG_B = (1 << 5),
		STATUS_REG_BP1 = (1 << 3),
		STATUS_REG_BP2 = (1 << 2),
		STATUS_REG_WEL = (1 << 1),
		STATUS_REG_BUSY = (1 << 0),
	};
	
	
	void deepPowerdown(Bool isPowerDown) {
		this->config.chipSelect(true);
		this->config.spiWrite(isPowerDown ? CMD_DEEP_POWERDOWN : CMD_RELEASE_DEEP_POWERDOWN);
		this->config.chipSelect(false);
	}
	
	FU8 readStatusRegister() {
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_READSTATUSREGISTER);
		FU8 status = this->config.spiRead();
		this->config.chipSelect(false);
		
		return status;
	}

	Bool isBusy() {
		return (readStatusRegister() & STATUS_REG_BUSY);
	}
	
	void waitBusy() {
		while(this->isBusy()) {}
	}
	
	struct Jedecid {
		FU8 manufacturer;
		FU8 memoryType;
		FU8 capacity;
	};
	
	void readJedecid(Jedecid* jedecidPtr) {
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_READJEDECID);
		jedecidPtr->manufacturer = this->config.spiRead();
		jedecidPtr->memoryType = this->config.spiRead(); 
		jedecidPtr->capacity = this->config.spiRead(); 
		this->config.chipSelect(false);
	}
	
	void eraseChip_nonBlocking() {
		//# Enable write, and send chip erase command
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_WRITE_ENABLE);
		this->config.chipSelect(false);
		
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_CHIP_ERASE);
		this->config.chipSelect(false);
	}

	void eraseChip() {
		this->eraseChip_nonBlocking();
		while(this->isBusy()) {
			this->config.sleepMs(100);
		}
	}
	
	
	private:
	void inline writeAddress(FU32 addr) {
		this->config.spiWrite(U8(addr >> 16));  //# addr[23:16]
		this->config.spiWrite(U8(addr >>  8));  //# addr[15:8]
		this->config.spiWrite(U8(addr >>  0));  //# addr[7:0]
  }
  public:  
  
  void eraseSector(FU32 addr) {
		this->waitBusy();

		//# Enable write, and send chip erase command
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_WRITE_ENABLE);
		this->config.chipSelect(false);
		
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_SECTOR_ERASE);
		this->writeAddress(addr);
		this->config.chipSelect(false);
	}
	
	void eraseBlock32K(FU32 addr) {
		this->waitBusy();

		//# Enable write, and send chip erase command
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_WRITE_ENABLE);
		this->config.chipSelect(false);
		
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_BLOCK_ERASE_32K);
		this->writeAddress(addr);
		this->config.chipSelect(false);
	}
	
	void eraseBlock64K(FU32 addr) {
		this->waitBusy();

		//# Enable write, and send chip erase command
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_WRITE_ENABLE);
		this->config.chipSelect(false);
		
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_BLOCK_ERASE_64K);
		this->writeAddress(addr);
		this->config.chipSelect(false);
	}
	
	void read(FU32 addr, U8* data, FU16 len) {
		// this->waitBusy();

		this->config.chipSelect(true);
		// this->config.spiWrite(CMD_ARRAYREAD);
		this->config.spiWrite(CMD_READ);
		this->writeAddress(addr);
		// this->config.spiWrite(0x00);              //# Wait states
		for(; len > 0; len -= 1) {
			data[0] = this->config.spiRead();
			data += 1;
		}
		this->config.chipSelect(false);
	}
	
	void write(FU32 addr, U8* data, FU16 len) {
		this->waitBusy();

		this->config.chipSelect(true);
		this->config.spiWrite(CMD_WRITE_ENABLE);
		this->config.chipSelect(false);
		
		this->config.chipSelect(true);
		this->config.spiWrite(CMD_PAGEPROGRAM);
		this->writeAddress(addr);
		for(; len > 0; len -= 1) {
			this->config.spiWrite(data[0]);
			data += 1;
		}
		this->config.chipSelect(false);
		
	}
	
};
#pragma pop_macro("Self")
