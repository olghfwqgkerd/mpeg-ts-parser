#include "TransportStream.h"
#include <iostream>
#include <iomanip>

//=============================================================================================================================================================================
// TS_Packet
//=============================================================================================================================================================================

//void TS_Packet::Reset()
//{
//
//}

void TS_Packet::ParseHeader(const uint8_t* Input)
{
    uint32_t tmp = Input[0] | (Input[1] << 8) | (Input[2] << 16) | (Input[3] << 24);
    tmp = xSwapBytes32(tmp);

    uint32_t H_SBmask =   0b11111111000000000000000000000000,
             H_Emask =    0b00000000100000000000000000000000,
             H_Smask =    0b00000000010000000000000000000000,
             H_Tmask =    0b00000000001000000000000000000000,
             H_PIDmask =  0b00000000000111111111111100000000,
             H_TSCmask =  0b00000000000000000000000011000000,
             H_AFCmask =  0b00000000000000000000000000110000,
             H_CCmask =   0b00000000000000000000000000001111;

    uint32_t AF_AFLmask =   0b11111111000000000000000000000000;    

    // SB
    uint32_t tmp_H_SB = tmp & H_SBmask;
    tmp_H_SB >>= 24;
    H_SB = tmp_H_SB;

    // E
    uint32_t tmp_H_E = tmp & H_Emask;
    tmp_H_E >>= 23;
    H_E = tmp_H_E;

    // S
    uint32_t tmp_H_S = tmp & H_Smask;
    tmp_H_S >>= 22;
    H_S = tmp_H_S;

    // T
    uint32_t tmp_H_T = tmp & H_Tmask;
    tmp_H_T >>= 21;
    H_T = tmp_H_T;

    // PID
    uint32_t tmp_H_PID = tmp & H_PIDmask;
    tmp_H_PID >>= 8;
    H_PID = tmp_H_PID;

    // TSC
    uint32_t tmp_H_TSC = tmp & H_TSCmask;
    tmp_H_TSC >>= 6;
    H_TSC = tmp_H_TSC;

    // AFC
    uint32_t tmp_H_AFC = tmp & H_AFCmask;
    tmp_H_AFC >>= 4;
    H_AFC = tmp_H_AFC;

    // CC
    uint32_t tmp_H_CC = tmp & H_CCmask;
    H_CC = tmp_H_CC;

    if(H_AFC == 2 || H_AFC == 3)
    {
        ParseAdaptationField(Input);
    }
    else ParsePayload(Input, H_AFC, AF_AFL);
}

void TS_Packet::PrintHeader() const
{
    printf("TS: SB=%2d E=%1d S=%1d T=%1d PID=%4d TSC=%1d AFC=%1d CC=%2d ",H_SB,H_E,H_S,H_T,H_PID,H_TSC,H_AFC,H_CC);
}

void TS_Packet::ParseAdaptationField(const uint8_t* Input)
{   
        uint16_t tmp = Input[4] | (Input[5] << 8);
        tmp = xSwapBytes16(tmp);

        uint16_t mask = 0b1111111100000000;

        uint16_t tmp_AF_AFL = tmp & mask;
        tmp_AF_AFL >>= 8;
        AF_AFL = tmp_AF_AFL;

        mask = 0b0000000010000000;

        uint16_t tmp_AF_DC = tmp & mask;
        tmp_AF_DC >>= 7;
        mask >>= 1;
        AF_DC = tmp_AF_DC;

        uint16_t tmp_AF_RA = tmp & mask;
        tmp_AF_RA >>= 6;
        mask >>= 1;
        AF_RA = tmp_AF_RA;


        uint16_t tmp_AF_ESP = tmp & mask;
        tmp_AF_ESP >>= 5;
        mask >>= 1;
        AF_ESP = tmp_AF_ESP;


        uint16_t tmp_AF_PR = tmp & mask;
        tmp_AF_PR >>= 4;
        mask >>= 1;
        AF_PR = tmp_AF_PR;


        uint16_t tmp_AF_OR = tmp & mask;
        tmp_AF_OR >>= 3;
        mask >>= 1;
        AF_OR = tmp_AF_OR;


        uint16_t tmp_AF_SP = tmp & mask;
        tmp_AF_SP >>= 2;
        mask >>= 1;
        AF_SP = tmp_AF_SP;


        uint16_t tmp_AF_TP = tmp & mask;
        tmp_AF_TP >>= 1;
        mask >>= 1;
        AF_TP = tmp_AF_TP;

        uint16_t tmp_AF_EX = tmp & mask;
        AF_EX = tmp_AF_EX;

        ParsePayload(Input, H_AFC, AF_AFL);

}

void TS_Packet::PrintAdaptationField() const
{   
    if(H_AFC == 2 || H_AFC ==3)
    {
        printf("AF: AFL=%2d DC=%1d RA=%1d ESP=%1d PR=%1d OR=%1d SP=%1d TP=%1d EX=%1d ",AF_AFL, AF_DC, AF_RA, AF_ESP, AF_PR, AF_OR, AF_SP, AF_TP, AF_EX);
    }
}

void TS_Packet::ParsePayload(const uint8_t* Input, uint8_t H_AFC, uint8_t AF_AFL)
{
    switch(H_AFC)
    {
        case 2:     // tylko pole AF
            isPayload = false;
        break;
        case 3:     // po polu AF jest pole Payload
            isPayload = true;
            payload = new uint8_t[184 - (1 + AF_AFL)];
            for(uint8_t i = 0; i < (184 - (1 + AF_AFL)); i++)
            {
                payload[i] = Input[i + 4 + 1 + AF_AFL];
            }
        break;
        default:    // nie ma pola AF jest tylko payload
            isPayload = true;
            payload = new uint8_t[184];
            for(uint8_t i = 0; i < 184; i++)
            {
                payload[i] = Input[i + 4 + 1 + AF_AFL];
            }
        break;      
        //  DLUGOSC HEAD PES 6 BITOW BEDZIE 1 TS
    }
}

void TS_Packet::PrintIsPayload() const
{
    switch(isPayload)
    {
        case true:
            printf("Pakiet zawierta Payload.");
        break;
        case false:
            printf("Pakiet NIE zawiera Payload!");
        break;
    }
}