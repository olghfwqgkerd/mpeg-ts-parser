#include "TransportStream.h"
#include <iostream>
#include <iomanip>

//||=============================================================================================================================================================================
//||
//|| TS_Packet
//||
//||=============================================================================================================================================================================

TS_Packet::TS_Packet(){
    H_SB    = 0;
    H_TE    = 0;
    H_PUS   = 0;
    H_TP    = 0;
    H_PID   = 0;
    H_TSC   = 0;
    H_AFC   = 0;
    H_CC    = 0;
    AF_AFL  = 0;
    AF_D    = 0;
    AF_RA   = 0;
    AF_ESP  = 0;
    AF_PCR  = 0;
    AF_OPCR = 0;
    AF_SP   = 0;
    AF_TPD  = 0;
    AF_AFE  = 0;

    isPayload = false;
    sizeOfPayload = 0;
    payload = nullptr;
}

void TS_Packet::ParseHeader(const uint8_t* Input){

    uint32_t tmp = Input[0] | (Input[1] << 8) | (Input[2] << 16) | (Input[3] << 24);
    tmp = xSwapBytes32(tmp);

    uint32_t H_SBmask =   0b11111111000000000000000000000000,
             H_TEmask =    0b00000000100000000000000000000000,
             H_PUSmask =    0b00000000010000000000000000000000,
             H_TPmask =    0b00000000001000000000000000000000,
             H_PIDmask =  0b00000000000111111111111100000000,
             H_TSCmask =  0b00000000000000000000000011000000,
             H_AFCmask =  0b00000000000000000000000000110000,
             H_CCmask =   0b00000000000000000000000000001111;

    uint32_t AF_AFLmask =   0b11111111000000000000000000000000;    

    uint32_t tmp_H_SB = tmp & H_SBmask;
    tmp_H_SB >>= 24;
    H_SB = tmp_H_SB;

    uint32_t tmp_H_TE = tmp & H_TEmask;
    tmp_H_TE >>= 23;
    H_TE = tmp_H_TE;

    uint32_t tmp_H_PUS = tmp & H_PUSmask;
    tmp_H_PUS >>= 22;
    H_PUS = tmp_H_PUS;

    uint32_t tmp_H_TP = tmp & H_TPmask;
    tmp_H_TP >>= 21;
    H_TP = tmp_H_TP;

    uint32_t tmp_H_PID = tmp & H_PIDmask;
    tmp_H_PID >>= 8;
    H_PID = tmp_H_PID;

    uint32_t tmp_H_TSC = tmp & H_TSCmask;
    tmp_H_TSC >>= 6;
    H_TSC = tmp_H_TSC;

    uint32_t tmp_H_AFC = tmp & H_AFCmask;
    tmp_H_AFC >>= 4;
    H_AFC = tmp_H_AFC;

    uint32_t tmp_H_CC = tmp & H_CCmask;
    H_CC = tmp_H_CC;

    if(H_AFC == 2 || H_AFC == 3){

        ParseAdaptationField(Input);
    }
    else if(H_AFC == 1 || H_AFC == 3){
        ParsePayload(Input, H_AFC, AF_AFL);
    }
}

void TS_Packet::PrintHeader() const{

    printf("TS: SB=%2d E=%1d S=%1d T=%1d PID=%4d TSC=%1d AFC=%1d CC=%2d ",H_SB,H_TE,H_PUS,H_TP,H_PID,H_TSC,H_AFC,H_CC);
}

void TS_Packet::ParseAdaptationField(const uint8_t* Input){   
    uint16_t tmp = Input[4] | (Input[5] << 8);
    tmp = xSwapBytes16(tmp);

    uint16_t mask = 0b1111111100000000;

    uint16_t tmp_AF_AFL = tmp & mask;
    tmp_AF_AFL >>= 8;
    AF_AFL = tmp_AF_AFL;

    mask = 0b0000000010000000;

    uint16_t tmp_AF_D = tmp & mask;
    tmp_AF_D >>= 7;
    mask >>= 1;
    AF_D = tmp_AF_D;

    uint16_t tmp_AF_RA = tmp & mask;
    tmp_AF_RA >>= 6;
    mask >>= 1;
    AF_RA = tmp_AF_RA;

    uint16_t tmp_AF_ESP = tmp & mask;
    tmp_AF_ESP >>= 5;
    mask >>= 1;
    AF_ESP = tmp_AF_ESP;

    uint16_t tmp_AF_PCR = tmp & mask;
    tmp_AF_PCR >>= 4;
    mask >>= 1;
    AF_PCR = tmp_AF_PCR;

    uint16_t tmp_AF_OPCR = tmp & mask;
    tmp_AF_OPCR >>= 3;
    mask >>= 1;
    AF_OPCR = tmp_AF_OPCR;

    uint16_t tmp_AF_SP = tmp & mask;
    tmp_AF_SP >>= 2;
    mask >>= 1;
    AF_SP = tmp_AF_SP;

    uint16_t tmp_AF_TPD = tmp & mask;
    tmp_AF_TPD >>= 1;
    mask >>= 1;
    AF_TPD = tmp_AF_TPD;

    uint16_t tmp_AF_AFE = tmp & mask;
    AF_AFE = tmp_AF_AFE;

    if(H_AFC == 1 || H_AFC == 3){
        ParsePayload(Input, H_AFC, AF_AFL);
    }
}

void TS_Packet::PrintAdaptationField() const{   
    if(H_AFC == 2 || H_AFC ==3){

        printf("AF: AFL=%2d DC=%1d RA=%1d ESP=%1d PR=%1d OR=%1d SP=%1d TP=%1d EX=%1d ",AF_AFL, AF_D, AF_RA, AF_ESP, AF_PCR, AF_OPCR, AF_SP, AF_TPD, AF_AFE);
    }
}

void TS_Packet::ParsePayload(const uint8_t* Input, uint8_t H_AFC, uint8_t AF_AFL){
    switch(H_AFC){
        case 1:    // only Payload
            isPayload = true;
            sizeOfPayload = 184;
            payload = new uint8_t[sizeOfPayload];
            for(uint8_t i = 0; i < sizeOfPayload; i++){
                payload[i] = Input[(i + 4)];
            }
        break; 
        case 2:     // only AF
            isPayload = false;
        break;
        case 3:     // AF and Payload
            isPayload = true;
            sizeOfPayload = 184 - (1 + AF_AFL);
            payload = new uint8_t[sizeOfPayload];
            for(uint8_t i = 0; i < sizeOfPayload; i++){
                payload[i] = Input[(i + 4 + 1 + AF_AFL)];
            }
        break;
        default:
        break;      
    }
}

void TS_Packet::PrintIsPayload() const{

    switch(isPayload){
        case true:
            printf("Pakiet zawiera Payload. ");
        break;
        case false:
            printf("Pakiet NIE zawiera Payload! ");
        break;
    }
}

uint8_t TS_Packet::Get_H_PUS(){
    return H_PUS;
}

uint16_t TS_Packet::Get_H_PID(){
    return H_PID;
    
}

uint8_t TS_Packet::Get_H_CC(){
    return H_CC;
    
}

uint8_t* TS_Packet::GetPayload(){
    if(isPayload){
        return payload;
    }
}

void TS_Packet::PrinttPayload(uint8_t x){
    if(isPayload){
        printf("---%d\n",payload[x]);
    }
}

uint32_t TS_Packet::GetSizeOfPayload(){
    return sizeOfPayload;
}

//||=============================================================================================================================================================================
//||
//|| PES_Packet
//||
//||=============================================================================================================================================================================

PES_Packet::PES_Packet(){

    isPreInit = false;
    isInit = false;
    isFill = false;
    isCutPayload = false;

    H_PSCP = 0;
    H_SID = 0;
    H_PESPL = 0;

    headerLength = 0;

    tmp_payload = nullptr;
    arrayOfPayload = nullptr;
    positionInArray = 0;

    H_TEN = 0;
    H_PESSC = 0;
    H_PESP = 0;
    H_DAI = 0;
    H_C = 0;
    H_OOC = 0;
    H_PTSDTSF = 0;
    H_ESCRF = 0;
    H_ESRF = 0;
    H_DSMTMF = 0;
    H_ACIF = 0;
    H_PESCRCF = 0;
    H_PESEF = 0;
    H_PESHDL = 0;

    lastPositionInPayload = 0;
}

bool PES_Packet::Builder(const uint8_t* Input, uint32_t sizeoOfInput, uint32_t tempLastPositionInPayload){
    if(!isFill){
        if(!isPreInit){
            Init(Input);
        }
        if(!isInit){
            printf("\n no bylo 0 niestey \n");
        }
        if(isPreInit && isInit){
            Fill(Input, sizeoOfInput, tempLastPositionInPayload);
        }
    } 

    return isFill;
}

void PES_Packet::Init(const uint8_t* Input){
        uint32_t tmp1 = (Input[0]) | (Input[1] << 8) | (Input[2] << 16) | (Input[3] << 24);
        tmp1 = xSwapBytes32(tmp1);

        uint32_t mask = 0b11111111111111111111111100000000;
        uint32_t tmp_H_PSCP = tmp1 & mask;
        tmp_H_PSCP >>= 8;
        H_PSCP = tmp_H_PSCP;

        mask = 0b00000000000000000000000011111111;
        uint32_t tmp_H_SID = tmp1 & mask;
        H_SID = tmp_H_SID;

        uint16_t tmp2 = Input[4] | (Input[5] << 8);
        tmp2 = xSwapBytes16(tmp2);
        H_PESPL = tmp2;

        arrayOfPayload = new uint8_t[H_PESPL];
        
        for(uint32_t i = 0; i < H_PESPL; i++){
            arrayOfPayload[i] = 0;
        }

        isPreInit = true;

        if(H_PESPL == 0){
            isInit = false;
        } else {
            isInit = true;
        }
}

void PES_Packet::Fill(const uint8_t* Input, uint32_t sizeOfInput, uint32_t tempLastPositionInPayload){
    // dostaje caly Payload
    // musze znac jego dlugosc
    // aktualny counter oraz jaki maxymalny czyli dlugosc bajtow
    lastPositionInPayload = tempLastPositionInPayload;
    for(uint8_t i = lastPositionInPayload; i < sizeOfInput; i++){
        if(positionInArray < H_PESPL){
            arrayOfPayload[positionInArray] = Input[i];
            positionInArray++;
        } else{
            lastPositionInPayload = i;
            isCutPayload = true;
            isFill = true;
            break;
        }
    }
}

void PES_Packet::ParseAditionalHeader(){
    if (H_SID != 0xBC && H_SID != 0xBE && H_SID != 0xBF && H_SID != 0xF0 && H_SID != 0xF1 && H_SID != 0xFF && H_SID != 0xF2 && H_SID != 0xF8){
        uint16_t temp = (arrayOfPayload[6]) | (arrayOfPayload[7] << 8);
        temp = xSwapBytes16(temp);

        uint16_t mask = 0b1100000000000000;

        uint8_t tmp_H_TEN = temp & mask;
        tmp_H_TEN >>= 14;
        H_TEN = tmp_H_TEN;

        mask = 0b0011000000000000;

        uint8_t tmp_H_PESSC = temp & mask;
        tmp_H_PESSC >>= 12;
        H_PESSC = tmp_H_PESSC;

        mask = 0b0000100000000000;

        uint8_t tmp_H_PESP = temp & mask;
        tmp_H_PESP >>= 11;
        H_PESP = tmp_H_PESP;

        mask = 0b0000010000000000;

        uint8_t tmp_H_DAI = temp & mask;
        tmp_H_DAI >>= 10;
        H_DAI = tmp_H_DAI;

        mask = 0b0000001000000000;

        uint8_t tmp_H_C = temp & mask;
        tmp_H_C >>= 9;
        H_C = tmp_H_C;

        mask = 0b0000000100000000;

        uint8_t tmp_H_OOC = temp & mask;
        tmp_H_OOC >>= 8;
        H_OOC = tmp_H_OOC;

        mask = 0b0000000011000000;

        uint8_t tmp_H_PTSDTSF = temp & mask;
        tmp_H_PTSDTSF >>= 6;
        H_PTSDTSF = tmp_H_PTSDTSF;

        mask = 0b0000000000100000;

        uint8_t tmp_H_ESCRF = temp & mask;
        tmp_H_ESCRF >>= 5;
        H_ESCRF = tmp_H_ESCRF;

        mask = 0b0000000000010000;

        uint8_t tmp_H_ESRF = temp & mask;
        tmp_H_ESRF >>= 4;
        H_ESRF = tmp_H_ESRF;

        mask = 0b0000000000001000;

        uint8_t tmp_H_DSMTMF = temp & mask;
        tmp_H_DSMTMF >>= 3;
        H_DSMTMF = tmp_H_DSMTMF;

        mask = 0b0000000000000100;

        uint8_t tmp_H_ACIF = temp & mask;
        tmp_H_ACIF >>= 2;
        H_ACIF = tmp_H_ACIF;

        mask = 0b0000000000000010;

        uint8_t tmp_H_PESCRCF = temp & mask;
        tmp_H_PESCRCF >>= 1;
        H_PESCRCF = tmp_H_PESCRCF;

        mask = 0b0000000000000001;

        uint8_t tmp_H_PESEF = temp & mask;
        H_PESEF = tmp_H_PESEF;

        temp = (arrayOfPayload[8]) | (arrayOfPayload[8] << 8); // fill to 16b
        temp = xSwapBytes16(temp);

        mask = 0b1111111100000000;

        uint8_t tmp_H_PESHDL = temp & mask;
        H_PESHDL = tmp_H_PESHDL;

    }
}

void PES_Packet::PrintHeader() const{
    if(isInit){
        printf("PSCP=%1d SID=%3d PESPL=%4d ",H_PSCP, H_SID, H_PESPL);
    } else{
        printf("PES packet is empty!");
    }
}

const uint8_t* PES_Packet::GetTmpPayload(){
    return tmp_payload;
}

void PES_Packet::PrinttTempPayload(uint8_t x){
        printf("---%d\n",tmp_payload[x]);
}

bool PES_Packet::IsInit(){
    return isInit;
}

bool PES_Packet::IsFill(){
    return isFill;
}

bool PES_Packet::IsCutPayload(){
      return isCutPayload;
}

uint16_t PES_Packet::GetlastPositionInPayload(){
    return lastPositionInPayload;
}
