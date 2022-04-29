#pragma once
#include "swpbts.h"
#include <string>

/*
MPEG-TS packet:
`                    4               3               2               1  `
`      8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1  `
`   1 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`     |                             Header                            | `
`   4 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   5 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`     |                  Adaptation field + Payload                   | `
`     |                                                               | `
`     |                                                               | `
` 188 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

=============================================================================================================================================================================

MPEG-TS packet header:
`                    4               3               2               1  `
`      8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`     |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits if 2 - only adaptation field, if 3 - adaptation field + payload
Continuity counter           (CC ) :  4 bits

=============================================================================================================================================================================

MPEG-TS adaptation field:
`                                                                       `
`      2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3                                  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- . . . -+-+-+-+-+-+-+-+-+-+-+-+ `
`     |               |D|R|E|P|O|S|T|E|                 |             | `
`     |      AFL      |C|A|S|R|R|P|P|X|      optional   |   optional  | `
`     |               | | |P| | | | | |      fields     |   stuffing  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- . . . -+-+-+-+-+-+-+-+-+-+-+-+ `


Adaptation field length               (AFL) :  8 bits
Discontinuity indicator               (DC ) :  1 bit
Random access indicator               (RA ) :  1 bit
Elementary stream priority indicator  (ESP) :  1 bit
Program Clock Reference flag          (PR ) :  1 bits
Orginal Program ClockReference flag   (OR ) :  1 bits
Splicing point flag                   (SP ) :  1 bits
Transport private data flag           (TP ) :  1 bits
Adaptation field extension flag       (EX ) :  1 bits

=============================================================================================================================================================================
*/
/*
class xTS
{
public:
  static constexpr uint32_t TS_PacketLength = 188;
  static constexpr uint32_t TS_HeaderLength = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};
*/
//=============================================================================================================================================================================

class TS_Packet{
public:
    uint8_t H_SB,     // Sync byte
            H_TE,     // Transport error
            H_PUS,    // Payload unit star
            H_TP,     // Transport priority 
            H_TSC,    // Transport scrambling control 
            H_AFC,    // Adaptation field control 
            H_CC,     // Continuity counter

            AF_AFL,   // Adaptation field length 
            AF_D,     // Discontinuity
            AF_RA,    // Random access
            AF_ESP,   // Elementary stream priority
            AF_PCR,   // PCR
            AF_OPCR,  // OPCR
            AF_SP,    // Splicing point
            AF_TPD,   // Transport private data
            AF_AFE;   // Adaptation field extension

  uint16_t  H_PID;    // PID

  bool isPayload;
  uint8_t* payload;
  uint32_t sizeOfPayload;

public:
  TS_Packet();

  void ParseHeader(const uint8_t* Input);
  void PrintHeader() const;

  void ParseAdaptationField(const uint8_t* Input);
  void PrintAdaptationField() const;

  void ParsePayload(const uint8_t* Input, uint8_t H_AFC, uint8_t AF_AFL);
  void PrintIsPayload() const;
  void PrinttPayload(uint8_t x);

  uint8_t Get_H_PUS();
  uint16_t Get_H_PID();
  uint8_t Get_H_CC();

  uint8_t* GetPayload();
  uint32_t GetSizeOfPayload();
};

//=============================================================================================================================================================================

class PES_Packet{
public:
  bool isPreInit,
       isInit,
       isFill,
       isCutPayload;

  uint32_t H_PSCP;    // Packet start code prefix
  uint8_t  H_SID;     // Sream ID
  uint16_t H_PESPL;   // PES packet length

  uint8_t H_TEN,      // '10'
          H_PESSC,    // PES scrambling control
          H_PESP,     // PES priority
          H_DAI,      // Data alignment indicator
          H_C,        // Copyright
          H_OOC,      // Original or copy
          H_PTSDTSF,  // PTS DTS flags
          H_ESCRF,    // ESCR flag
          H_ESRF,     // ES rate flag
          H_DSMTMF,   // DSM trick mode flag
          H_ACIF,     // Additional copy info flag
          H_PESCRCF,  // PES CRC flag
          H_PESEF,    // PES extension flag
          H_PESHDL;   // PES header data length 

  const uint8_t* tmp_payload;

  uint16_t headerLength;

  uint8_t* arrayOfPayload;
  uint16_t positionInArray;
  uint16_t lastPositionInPayload;

public:
  PES_Packet();

  bool Builder(const uint8_t* Input, uint32_t sizeoOfInput, uint32_t tempLastPositionInPayload);

  void Init(const uint8_t* Input);
  bool IsInit();
  
  void Fill(const uint8_t* Input, uint32_t sizeOfInput, uint32_t tempLastPositionInPayload);
  bool IsFill();

  bool IsCutPayload();

  const uint8_t* GetTmpPayload();
  void PrinttTempPayload(uint8_t x);

  void ParseAditionalHeader();
  void PrintHeader() const;

  uint16_t GetlastPositionInPayload();
};
