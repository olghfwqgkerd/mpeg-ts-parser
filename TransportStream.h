#pragma once
#include "swpbts.h"
#include <string>

/*
MPEG-TS packet:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |                             Header                            | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   4 |                  Adaptation field + Payload                   | `
`     |                                                               | `
` 184 |                                                               | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

=============================================================================================================================================================================

MPEG-TS packet header:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits          if 2 - only adaptation field, if 3 - adaptation field + payload
Continuity counter           (CC ) :  4 bits

=============================================================================================================================================================================

MPEG-TS adaptation field:
`                                                                       `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6                                  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- . . . -+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |               |D|R|E|P|O|S|T|E|                 |             | `
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

//=============================================================================================================================================================================

class TS_Packet
{
protected:
  uint8_t H_SB = 0, H_E = 0, H_S = 0, H_T = 0, H_TSC = 0, H_AFC = 0, H_CC = 0,
           AF_AFL = 0, AF_DC = 0, AF_RA = 0, AF_ESP = 0, AF_PR = 0, AF_OR = 0, AF_SP = 0, AF_TP = 0, AF_EX = 0;

  uint16_t H_PID = 0;

public:
  /**enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };
  void     Reset();
  **/

  void  ParseHeader(const uint8_t* Input);
  void  PrintHeader() const;

  void  ParseAdaptationField(const uint8_t* Input);
  void  PrintAdaptationField() const;

  uint8_t   get_H_SB();
  uint8_t   get_H_E();
  uint8_t   get_H_S();
  uint8_t   get_H_T();
  uint16_t  get_H_PID();
  uint8_t   get_H_TSC();
  uint8_t   get_H_AFC();
  uint8_t   get_H_CC();
 
  //TODO
  //bool     hasAdaptationField() const { /*TODO*/ }
  //bool     hasPayload        () const { /*TODO*/ }
};

//=============================================================================================================================================================================
