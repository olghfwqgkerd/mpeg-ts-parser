#include "tsTransportStream.h"

#include<iostream>

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


void xTS_PacketHeader::Reset()
{

}

int32_t xTS_PacketHeader::Parse(const uint32_t* Input)
{
    uint32_t Tmp = *((uint32_t*)Input);
    Tmp = xSwapBytes32(Tmp);

    uint32_t SBmask =   0b11111111000000000000000000000000;
    uint32_t Emask =    0b00000000100000000000000000000000;
    uint32_t Smask =    0b00000000010000000000000000000000;
    uint32_t Tmask =    0b00000000001000000000000000000000;
    uint32_t PIDmask =  0b00000000000111111111111100000000;
    uint32_t TSCmask =  0b00000000000000000000000011000000;
    uint32_t AFCmask =  0b00000000000000000000000000110000;
    uint32_t CCmask =   0b00000000000000000000000000001111;

    // SB
    SB = Tmp & SBmask;
    SB >>= 24;
    // E
    E = Tmp & Emask;
    E >>= 23;
    // S
    S = Tmp & Smask;
    S >>= 22;
    // T
    T = Tmp & Tmask;
    T >>= 21;
    // PID
    PID = Tmp & PIDmask;
    PID >>= 8;
    // TSC
    TSC = Tmp & TSCmask;
    TSC >>= 6;
    // AFC
    AFC = Tmp & AFCmask;
    AFC >>= 4;
    // CC
    CC = Tmp & CCmask;

    return 0;
}

void xTS_PacketHeader::Print() const
{
    printf("TS:\tSB=%d\tE=%d\tS=%d\tT=%d\tPID=%d\tTSC=%d\tAFC=%d\tCC=%d",SB,E,S,T,PID,TSC,AFC,CC);
}

uint32_t xTS_PacketHeader::getSB()
{
    return SB;
}

uint32_t xTS_PacketHeader::getE()
{
    return E;
}

uint32_t xTS_PacketHeader::getS()
{
    return S;
}

uint32_t xTS_PacketHeader::getT()
{
    return T;
}

uint32_t xTS_PacketHeader::getPID()
{
    return PID;
}

uint32_t xTS_PacketHeader::getTSC()
{
    return TSC;
}

uint32_t xTS_PacketHeader::getAFC()
{
    return AFC;
}

uint32_t xTS_PacketHeader::getCC()
{
    return CC;
}




//=============================================================================================================================================================================
