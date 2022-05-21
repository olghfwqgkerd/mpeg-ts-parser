#include "TransportStream.h"

#include <iostream>
#include <bitset>
#include <fstream>

int main(){
  //=============================================================================================================================================================================
  
  bool SHOW_ID     = false,   // TS ID
       SHOW_TS_HD  = false,   // TS Header
       SHOW_TS_AF  = false,   // TS Adaptation Field
       SHOW_TS_PL  = false,   // TS Payload
       SHOW_PES_ST = false;   // PES status

  const uint16_t choosedPID = 136;    // Packet Identifier
  const uint8_t size = 188;

  //=============================================================================================================================================================================

  uint32_t length = 0;
  uint32_t PES_ID = 0;
  uint32_t CC_control = 0;
  uint32_t packetNumber = 0;

  uint8_t fullStep = 0;
  uint8_t step     = 0;
  uint8_t loadingProgres = 0;

  FILE *inputFile = fopen("../example_new.ts", "rb");

  if(inputFile) {
    fseek(inputFile, 0L, SEEK_END);
    length = ftell(inputFile);
    fseek(inputFile, 0L, SEEK_SET);

    packetNumber = length/188;

    TS_Packet *arrayOfTSpacket = new TS_Packet[length/188];   //
                                                              //tymczasowe rozwiązania w przyszlosci moze na liscie
    PES_Packet *arrayOfPESpacket = new PES_Packet[10000000];  //

    uint8_t *bufferInput = new uint8_t[size];

    uint8_t *bufferOutput = nullptr;

    //=============================================================================================================================================================================
    //  TS packets
    //=============================================================================================================================================================================

    //Load TS packets and parse header, adaptation field, payload
    for(uint32_t i = 0; i < packetNumber; i++){
      fread( bufferInput, sizeof(uint8_t), size, inputFile);
      arrayOfTSpacket[i].ParseHeader(bufferInput);
    }
    
    //=============================================================================================================================================================================
    //  PES packets
    //=============================================================================================================================================================================

    // Load PES packets and parse header
    for(uint32_t i = 0; i < packetNumber; i++){
      if(arrayOfTSpacket[i].Get_H_PID() == choosedPID){
        if(CC_control == arrayOfTSpacket[i].Get_H_CC()){
          arrayOfPESpacket[PES_ID].Builder(arrayOfTSpacket[i].GetPayload(), arrayOfTSpacket[i].GetSizeOfPayload(), 0);
          CC_control++;
          if(CC_control == 16 ) CC_control = 0;
          if(arrayOfPESpacket[PES_ID].IsFill()){
            PES_ID++;
          }
        } else{
          if(arrayOfTSpacket[i].Get_H_PID() != choosedPID){
            printf("\n------------------------------------------------------------\nERROR! WRONG CONTINUITY COUNTER! DATA LOST! FOR PID: %d\n  parsing continue for PID = %d\n------------------------------------------------------------\n", arrayOfTSpacket[i].Get_H_PID(),choosedPID);
            continue;
          } else{
            printf("\n------------------------------------------------------------\nERROR! WRONG CONTINUITY COUNTER! DATA LOST! FOR PID: %d\n  FATAL ERROR! PARSING BREAK FOR PID: %d\n    TS ID: %d\n    CC: %d\n    Expected CC: %d\n    PES ID:%d\n------------------------------------------------------------\n", arrayOfTSpacket[i].Get_H_PID(),choosedPID,i,arrayOfTSpacket[i].Get_H_CC(),CC_control,PES_ID);
            break;
          }
        }
      }
      
      //=============================================================================================================================================================================
      //  Display information
      //=============================================================================================================================================================================
      if(SHOW_ID) printf("%010d ", i+1);
      if(SHOW_TS_HD) arrayOfTSpacket[i].PrintHeader();
      if(SHOW_TS_PL) arrayOfTSpacket[i].PrintIsPayload();
      if(SHOW_TS_AF) arrayOfTSpacket[i].PrintAdaptationField();
      if(SHOW_PES_ST){
        if(arrayOfTSpacket[i].Get_H_PID() == choosedPID){
          if(arrayOfPESpacket[PES_ID].isPreInit == true && arrayOfPESpacket[PES_ID].isFill == false){
            if(arrayOfTSpacket[i].Get_H_PUS() == 1){
              printf("<s>---------");
              arrayOfPESpacket[PES_ID].PrintHeader();
            } else{
              printf(" |");
            }
          } else if(arrayOfPESpacket[PES_ID-1].isPreInit == true && arrayOfPESpacket[PES_ID-1].isFill == true){
            printf("[f]---------Total length: %d", arrayOfPESpacket[PES_ID-1].H_PESPL + 6 + arrayOfPESpacket[PES_ID-1].H_PESHDL);
          }
        }
      }
      if(SHOW_ID || SHOW_TS_HD || SHOW_TS_AF || SHOW_TS_PL || SHOW_PES_ST) printf("\n");
    }

    // Parse adnitional header
    for(uint32_t i = 0; i < PES_ID; i++){
      arrayOfPESpacket[i].ParseAditionalHeader();
    }

    //FILE *outputFile = fopen("../output.mp2", "w");
    std::ofstream outputFile;
    outputFile.open ("../output.mp2", std::ios::out | std::ios::app | std::ios::binary); 

    fullStep = PES_ID / 100;

    printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    uint32_t i;
    int32_t j;
    for(i = 0; i < PES_ID; i++){
      bufferOutput = arrayOfPESpacket[i].arrayOfPayload;
      for(j = 0; j < arrayOfPESpacket[i].H_PESPL; j++){
        outputFile << bufferOutput[j];
        //fwrite( bufferOutput, sizeof( uint8_t ), arrayOfPESpacket[i].H_PESPL, outputFile );
      }
      step++;
      if(step % fullStep == 0){
        step = 0;
        printf("Saving to file... %d%%\r",loadingProgres);
        loadingProgres++;
      }
    }
    printf("\n\nSaved successfully!");

    outputFile.close();
    //fclose(outputFile);
    fclose(inputFile);
  } else{
    printf("\nError! Input file doesn't exist!\n ");
  }
  printf("\n\nOpus magnum, dokonalo sie.\n ");
  return 0;
}