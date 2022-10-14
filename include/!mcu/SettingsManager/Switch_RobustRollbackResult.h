/*
	Copyright 2022 Bga <bga.email@gmail.com>

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
#include <!cpp/TestRunner.h>
#include <!cpp/newKeywords.h>

#if 0

struct Config {
	enum {
		crc8Gen = 0x31, 
	};
	
	typedef ::Bga::Stream::Hash::Crc::Lbs_Bit<U8> Hash_Algo; 
	typedef ::Bga::Stream::Hash::Hash<Hash_Algo> Hash; 
	
	static Hash makeHash() {
		return Hash(Hash_Algo(crc8Gen)); 
	}
	
	enum {
		dataAlign = 4, 
	};
	
	static void read(Char const* nvramSrc, Char* ramDest, Size dataSize) {
		//for(U8 i = dataSize; i--;) {
		forInc(FU8, i, 0, dataSize) {
			ramDest[i] = EEPROM_read(uintptr_t(nvramSrc) + i);
		}
	}
	static void write(Char const* ramSrc, Char* nvramDest, Size dataSize) {
		//for(U8 i = dataSize; i--;) {
		forInc(FU8, i, 0, dataSize) {
			EEPROM_write(uintptr_t(nvramDest) + i, ramSrc[i]);
		}
	}
};
#endif

#pragma push_macro("Self")

#undef Self
#define Self Switch_RobustRollbackResult

namespace Bga { namespace Mcu { namespace SettingsManager {  

#if 0
template<class DataArg, class ConfigArg, class StoreArg, class Read_ErrorCodeArg>
struct Base {
	typedef DataArg Data;
	typedef ConfigArg Config;
	typedef StoreArg Store;
	typedef Read_ErrorCodeArg Read_ErrorCode;
	

	virtual void onAfterRead(Data& dataRef);
	Read_ErrorCode readBackup(Store const& store);
	
	virtual void onBeforeWrite(Data& dataRef);
	void writeBackup(Store& store);
};

namespace details { namespace Self {
	enum Read_ErrorCode {
		Read_ErrorCode_ok, 
		Read_ErrorCode_invalidCrc, 
		Read_ErrorCode_restoreBackup, 
	};
} } //# namespace
#endif // 0

template<class DataArg, class ConfigArg>
struct Self {
	typedef DataArg Data;
	typedef ConfigArg Config;
	
	Config config;
	
	struct Id {
		LU8 byte;
		
		Bool isValid() const {
			return ((this->byte & 0xF) ^ (this->byte >> 4)) /* & 0xF */ == 0xF;
		}
		operator FU8() const {
			return this->byte & 0xF;
		}
		
		Id& operator= (FU8 id) {
			this->byte = id | ((~id) << 4);
			return *this;
		}
		Id& operator++ () {
			*this = FU8(*this) + 1;
			return *this;
		}
	};


	#pragma pack(push, 1)
	struct Block_NoAlign {
		struct {
			Data data;
			Id id;
		} dataWrapper;
		typename Config::Hash::HashValue hashValue;

		typename Config::Hash::HashValue calcHash() const {
			return Config::makeHash().bulkCalc_charWithPad(this->dataWrapper);
		}
		inline Bool isValid() const {
			return this->hashValue == this->calcHash();
		}
		inline void updateAuxData() {
			this->hashValue = this->calcHash();
		}
	};
	
	typedef AlignSize<Block_NoAlign, Config::dataAlign> Block;

	struct Store {
		Block blocks[2];
	};
	#pragma pack(pop)
	
	enum Read_ErrorCode {
		Read_ErrorCode_ok, 
		Read_ErrorCode_fail, 
		Read_ErrorCode_corruptedCopy, 
		Read_ErrorCode_restoreBackup, 
	};

	virtual void onAfterRead(Data& dataRef) = 0;
	virtual void onBeforeWrite(Data& dataRef) = 0;
	

	Block memBlock;
	
	Data const& get_dataRef() const {
		return this->memBlock.dataWrapper.data;
	}
	Data& get_dataRef() {
		return this->memBlock.dataWrapper.data;
	}

	
	FU8 srcsToBitSet(Store const& store) const {
		return (store.blocks[0].isValid() ? 1 : 0) | ((store.blocks[1].isValid() ? 1 : 0) << 1); 
	}
	Int findLastStoreIndexByBitSet(Store const& store, FU8 validIndecesBitSet, Read_ErrorCode& retCode) const {
		FU8 id0 = store.blocks[0].dataWrapper.id;
		FU8 id1 = store.blocks[1].dataWrapper.id;
		
		Bool isBlock1NewerThanBlock0 = (FU8(id1 - id0) < FU8(id0 - id1));
		Bool isBlock1SameAsBlock0 = (id1 == id0);
		Bool isBothIdsValid = store.blocks[0].dataWrapper.id.isValid() && store.blocks[1].dataWrapper.id.isValid();
		
		switch(validIndecesBitSet) {
		  case(0): {
				retCode = Read_ErrorCode_fail;
				return -1;
			} break;
			case(1): case(2): {
				retCode = (!isBothIdsValid) ? Read_ErrorCode_corruptedCopy : (isBlock1SameAsBlock0) ? Read_ErrorCode_corruptedCopy : (validIndecesBitSet - isBlock1NewerThanBlock0 == 1) ? Read_ErrorCode_corruptedCopy : Read_ErrorCode_restoreBackup;
//				retCode = (!isBothIdsValid || isBlock1SameAsBlock0 || (validIndecesBitSet - isBlock1NewerThanBlock0 == 1)) ? Read_ErrorCode_corruptedCopy : Read_ErrorCode_restoreBackup;
				return validIndecesBitSet - 1;
			} break;
		  case(3): {
				retCode = Read_ErrorCode_ok;
				return (isBlock1NewerThanBlock0) ? 1 : 0;
			} break;
			default: {
				return -2;
			}
		}
	}
	Int findLastStoreIndex(Store const& store) const {
		Read_ErrorCode retCode;
		return this->findLastStoreIndexByBitSet(store, this->srcsToBitSet(store), retCode);
	}

	
	private:
	void readBlock(Block const& storeBlock) {
		this->config.read(reinterpret_cast<Char const*>(&storeBlock), reinterpret_cast<Char *>(&(this->memBlock)), sizeof(storeBlock));
	}
	
	public:
	Read_ErrorCode readBackup(Store const& store) {
		FU8 validBitSet = this->srcsToBitSet(store);
		Read_ErrorCode retCode;
		FI8 lastIndex = this->findLastStoreIndexByBitSet(store, validBitSet, retCode);
		
		if(lastIndex < 0) {
			return Read_ErrorCode_fail;
		}
		else {
			this->readBlock(store.blocks[lastIndex]);
			this->onAfterRead(this->get_dataRef());
			return retCode;
		}
	}
	
		
	protected:
	void writeBlock(Block& storeBlock) {
		this->memBlock.updateAuxData();
		this->config.write(reinterpret_cast<Char const*>(&(this->memBlock)), reinterpret_cast<Char*>(&storeBlock), sizeof(storeBlock));
	}
	
	
	
	public:
	void writeBackup(Store& store) {
		this->onBeforeWrite(this->get_dataRef());
		
		FU8 nextIndex;
		Id nextId;
		FI8 lastIndex = this->findLastStoreIndex(store);
		FU8 blocksCountToWrite = 1;
		
		if(lastIndex < 0) {
			nextId = 0;
			nextIndex = 0;
			blocksCountToWrite = 2;
		}
		else {
			nextId = store.blocks[lastIndex].dataWrapper.id;
			++nextId;
			nextIndex = FU8(lastIndex ^ 1); //# 0 -> 1, 1 -> 0
		}
		
		this->memBlock.dataWrapper.id = nextId; 
		forInc(FU8, i, 0, blocksCountToWrite) {
			this->writeBlock(store.blocks[nextIndex++]);
		}
	}
	
};


} } } //# namespace

#if 0
switch(typename StoreManager::Read_ErrorCode err = storeManager.readBackup(ramData)) {
	case(Read_ErrorCode_ok): {
	} break;
	case(Read_ErrorCode_restoreBackup): {
	} break;
	case(Read_ErrorCode_fail): {
	} break;
}
if(err != Read_ErrorCode_ok) {
	...
};
ramData.a += 1;

storeManager.writeBackup(ramData);
#endif // 0


#define BGA__TESTRUNNER_ON

#ifdef BGA__TESTRUNNER_ON

#include <!cpp/wrapper/algorithm>
#include <!cpp/Stream/Hash/Hash.h>
#include <!cpp/Stream/Hash/Crc/Lsb_Bit.h>
#include <!cpp/debug2.h>

namespace details { namespace Self { 
	LU32 externalVar = 0; 
} } //# namespace

example(BGA__STR(Self)) {
//int main() {
	using ::Bga::Mcu::SettingsManager::Self;
	using ::details::Self::externalVar;
	
	struct Data {
		LU32 a;
		LU32 b;
		LU32 n;
	};

	struct Config {
		enum {
			crc8Gen = 0x31, 
		};
		
		typedef ::Bga::Stream::Hash::Crc::Lbs_Bit<U8> Hash_Algo; 
		typedef ::Bga::Stream::Hash::Hash<Hash_Algo> Hash; 
		
		static Hash makeHash() {
			return Hash(Hash_Algo(crc8Gen)); 
		}
		
		enum {
			dataAlign = 4, 
		};

		static void read(Char const* nvramSrc, Char* ramDest, Size dataSize) {
			::std::copy_n(nvramSrc, dataSize, ramDest);
		}
		static void write(Char const* ramSrc, Char* nvramDest, Size dataSize) {
			::std::copy_n(ramSrc, dataSize, nvramDest);
		}
	};

	struct AppStoreManager: public Self<Data, Config> {
		virtual void onAfterRead(Data& dataRef) override {
			externalVar = dataRef.n;
		}
		virtual void onBeforeWrite(Data& dataRef) override {
			dataRef.n = externalVar;
		}
	};
	
	struct App_Store {
		union {
			typename AppStoreManager::Store dataStore;
			Char rawStoreData[sizeof(dataStore)];
		};
		
		void eriseData() {
			::std::fill_n(this->rawStoreData, sizeof(this->rawStoreData), 0xCC);
		}
		void corruptFirstBlock() {
			this->rawStoreData[0] += 1;
		}
		void corruptSecondBlock() {
			this->rawStoreData[arraySize(this->rawStoreData) / 2] += 1;
		}
	};
	App_Store App_store;

	
	enum { 
		rawStoreDataSize = sizeof(App_store.rawStoreData),  
	};  
	
	AppStoreManager appStoreManager;

	Data& data = appStoreManager.get_dataRef();
	App_store.eriseData();
	
	Data expectedData = {
		.a = 1, 
		.b = 2, 
		.n = 3, 
	};
	
	data = expectedData;
	
	//# also test onBeforeWrite
	data.n = externalVar;
	externalVar += 1;
	appStoreManager.writeBackup(App_store.dataStore);
	assert_eq(data.n, externalVar);
	BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
	
	//hex_dump(std::cout, std::string(reinterpret_cast<const char *>(rawStoreData), rawStoreDataSize));
	
	data.a = data.b = 0;
	//# also test onAfterRead
	externalVar = data.n;
	data.n += 1;
	assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
	assert_eq(externalVar, data.n);
	assert_eq(data.a, expectedData.a);
	assert_eq(data.b, expectedData.b);


	App_store.corruptFirstBlock();
	BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
	assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_corruptedCopy);
	
	App_store.corruptSecondBlock();
	BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
	assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_fail);

	
	//# write, corrupt, read, corrupt, read
	if(1) {
		if(1) {
			App_store.eriseData();

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			App_store.corruptFirstBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_corruptedCopy);

			App_store.corruptSecondBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_fail);
		};

		if(1) {
			App_store.eriseData();

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			App_store.corruptSecondBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_corruptedCopy);

			App_store.corruptFirstBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_fail);
		};
	};
			
	//# write, read, write, read, corrupt, read
	if(1) {
		if(1) {
			App_store.eriseData();

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			App_store.corruptFirstBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_corruptedCopy);
		};
		
		if(1) {
			App_store.eriseData();

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			appStoreManager.writeBackup(App_store.dataStore);
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_ok);
			assert_eq(data.a, expectedData.a);
			assert_eq(data.b, expectedData.b);

			App_store.corruptSecondBlock();
			BGA__DEBUG__HEX_INSPECT_ARRAY(App_store.rawStoreData, 0, rawStoreDataSize)
			assert_eq(appStoreManager.readBackup(App_store.dataStore), AppStoreManager::Read_ErrorCode_restoreBackup);
		};
	};
}
#endif // BGA__TESTRUNNER_ON


#pragma pop_macro("Self")
