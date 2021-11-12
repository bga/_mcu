#pragma once

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

#include <stddef.h>
#include <!cpp/common.h>
#include <!cpp/newKeywords.h>
#include <!cpp/TestRunner.h>

#if 0
static const int eepromSaveOffset = 0;

struct EepromConfig {
  typedef AddlerCrc Crc; 
  typedef EEPROM_SizeT EepromSizeT;
  
  static const int copiesCount = eepromSaveCopiesCount;
  
  static void EepromApi_lockWriteAccess() {
    
  }
  static void EepromApi_unlockWriteAccess() {
    
  }
  static U8 EepromApi_readByte(EepromSizeT offset) {
    return EEPROM_read(eepromSaveOffset + offset);
  }
  static void EepromApi_writeByte(EepromSizeT offset, U8 v) {
    EEPROM_write(eepromSaveOffset + offset, v);
  }
  static void EepromApi_readBlock(U8 * data, EepromSizeT offset, EepromSizeT dataSize) {
    //for(U8 i = dataSize; i--;) {
    forInc(FU8, i, 0, dataSize) {
      data[i] = EEPROM_read(offset + i);
    }
  }
  static void EepromApi_writeBlock(U8 const * data, EepromSizeT offset, EepromSizeT dataSize) {
    //for(U8 i = dataSize; i--;) {
    forInc(FU8, i, 0, dataSize) {
      EEPROM_write(offset + i, data[i]);
    }
  }
};
#endif


#define Self EepromManager
//#include <!cpp/classHelpers.h>
#define p (*this)
template<class DataArg, class ConfigArg>
class Self {
  public:
  typedef DataArg Data;
  typedef ConfigArg Config;
  
  Config config;
  
  #pragma pack(push, 1)
  struct Block {
    typename Config::Crc::CrcValue crcValue;
    union {
      U8 rawData[sizeof(Data)];
      Data data;
    };
  };
  #pragma pack(pop)
  
  enum { faultOffset = Config::copiesCount * sizeof(Block) };
  
  private:
  typedef typename Config::EepromSizeT Config_EepromSizeT;
  typedef typename Config::Crc Config_Crc;
  typedef typename Config::Crc::CrcValue Config_Crc_CrcValue;
  
  
  //# runtime data
  Block lastRev;
  
  //# eeprom memory layout
  struct DataStorage {
    //# 0 or 1. 0xFF in memory was erased
    Block revs[Config::copiesCount];
  };
  
  public:
  
  Config_Crc_CrcValue calcRevCrcValue(const Block& rev) {
    Config_Crc crc; crc.init();
    
    forInc(FU8, i, 0, sizeof(rev.rawData)) {
      crc.update(rev.rawData[i]);
    }
    return crc.getCrcValue();
  }
  
  #if 0
  #if 0
  Config_Crc_CrcValue readRev(Block& rev, Config_EepromSizeT offset) {
    rev.crcValue = p.config.EepromApi_readByte(offset);
    offset += sizeof(U8);
    
    Config_Crc crc; crc.init();
    const Config_EepromSizeT maxOffset = offset + sizeof(Block); 
    for(;offset < maxOffset; offset += 1) {
      const U8 v = reinterpret_cast<U8 *>(&rev)[offset] = p.config.EepromApi_readByte(offset);
      crc.update(v);
    }
    return crc.getCrcValue();
  }
  #else
  Config_Crc_CrcValue readRev(Block& rev, Config_EepromSizeT offset) {
    Config_Crc crc; crc.init();
  
    const Config_EepromSizeT maxOffset = offset + sizeof(Block); 
    const Config_EepromSizeT dataOffset = offset + offsetof(Block, data); 
    
    for(; offset < maxOffset; offset += 1) {
      const U8 v = reinterpret_cast<U8 *>(&rev)[offset] = p.config.EepromApi_readByte(offset);
      if(dataOffset <= offset) crc.update(v);
    }
    return crc.getCrcValue();
  }
  #endif
  #endif
  
  //# returns 1st valid block offset. { 0 } means master, { faultOffset } is offset after all block and means unrecoverable error. Other values means recovery to prev state
  Config_EepromSizeT searchLastRev() {
    const Config_EepromSizeT maxOffset = Config::copiesCount * sizeof(Block);
    Config_EepromSizeT offset = 0;
    loop {
      if(maxOffset <= offset) {
        return faultOffset;
      }
      else {
        #if 0
        Config_Crc_CrcValue crc = p.readRev(p.lastRev, offset);
        #else
        p.config.EepromApi_readBlock(reinterpret_cast<U8 *>(&(p.lastRev)), offset, sizeof(p.lastRev));
        Config_Crc_CrcValue crc = p.calcRevCrcValue(p.lastRev);
        #endif
        if(p.lastRev.crcValue == crc) {
          break;
        }
        else {
          offset += sizeof(Block);
        }
      }
    }
    
    return offset;
  }
  
  
  Data& getData() {
    return p.lastRev.data;
  }
  
  void writeData() {
    p.lastRev.crcValue = p.calcRevCrcValue(p.lastRev);
    
    p.config.EepromApi_unlockWriteAccess();
    const Config_EepromSizeT maxOffset = Config::copiesCount * sizeof(Block);
    for(Config_EepromSizeT offset = 0; offset < maxOffset; offset += sizeof(Block)) {
      p.config.EepromApi_writeBlock(reinterpret_cast<U8 *>(&(p.lastRev)), offset, sizeof(p.lastRev));
    }
    
    p.config.EepromApi_lockWriteAccess();
  }
};
#undef p
#undef Self

#ifdef BGA__TESTRUNNER_ON
#include <!cpp/AddlerCrc.h>

example("EepromManager::generic") {
  enum { 
    eepromSize = 64, 
    eepromSaveCopiesCount = 3, 
    eepromSaveOffset = 0
  };  
  
  
  struct EepromConfig {
    #define p (*this)
    typedef AddlerCrc Crc; 
    typedef U8 EepromSizeT;
    
    U8 eepromData[eepromSize];
    
    enum {
      copiesCount = eepromSaveCopiesCount
    };  
    
    void EepromApi_lockWriteAccess() {
      
    }
    void EepromApi_unlockWriteAccess() {
      
    }
    U8 EepromApi_readByte(EepromSizeT offset) {
      return p.eepromData[eepromSaveOffset + offset];
    }
    void EepromApi_writeByte(EepromSizeT offset, U8 v) {
      p.eepromData[eepromSaveOffset + offset] = v;
    }
    void EepromApi_readBlock(U8 * data, EepromSizeT offset, EepromSizeT dataSize) {
      //for(U8 i = dataSize; i--;) {
      forInc(FU8, i, 0, dataSize) {
        data[i] = p.eepromData[eepromSaveOffset + offset + i];
      }
    }
    void EepromApi_writeBlock(U8 const * data, EepromSizeT offset, EepromSizeT dataSize) {
      //for(U8 i = dataSize; i--;) {
      forInc(FU8, i, 0, dataSize) {
        p.eepromData[eepromSaveOffset + offset + i] =  data[i];
      }
    }
    #undef p
  };
  
  struct Data {
    U32 a;
    U32 b;
  };
  
  typedef EepromManager<Data, EepromConfig> AppEepromManager;
  AppEepromManager appEepromManager;

  Data& data = appEepromManager.getData();
  U8* eepromData = appEepromManager.config.eepromData;
  forInc(FU8, i, 0, eepromSize) {
    eepromData[i] = 0xCC;
  }
  //#define eepromData appEepromManager.config.eepromData
  
  Data expectedData = {
    .a = 1, 
    .b = 2
  };
  
  data = expectedData;
  appEepromManager.writeData();
  
  //hex_dump(std::cout, std::string(reinterpret_cast<const char *>(eepromData), eepromSize));
  
  data.a = data.b = 0;
  assert_eq(appEepromManager.searchLastRev(), 0);
  assert_eq(data.a, expectedData.a);
  assert_eq(data.b, expectedData.b);
  
  //# corrupt 2nd block
  eepromData[1 + sizeof(AppEepromManager::Block)] += 1;
  assert_eq(appEepromManager.searchLastRev(), 0);
  
  //# corrupt 1st block. So we can still read 3rd block
  eepromData[5] += 1;
  assert_not_eq(appEepromManager.searchLastRev(), 0);
  assert_not_eq(appEepromManager.searchLastRev(), AppEepromManager::faultOffset);
  
  //# corrupt last 3rd block. Memory should be in fault state
  eepromData[5 + 2 * sizeof(AppEepromManager::Block)] += 1;
  assert_eq(appEepromManager.searchLastRev(), AppEepromManager::faultOffset);

  //# restore memory
  appEepromManager.writeData();
  assert_eq(appEepromManager.searchLastRev(), 0);
  
}
#endif
