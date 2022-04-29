#include "TransportStream.h"

#include <iostream>
#include <bitset>

int main(){
//=============================================================================================================================================================================
  
  bool SHOW_ID     = true,  // TS ID
       SHOW_TS_HD  = true,  // TS Header
       SHOW_TS_AF  = false,  // TS Adaptation Field
       SHOW_TS_PL  = false,  // TS Payload
       SHOW_PES_ST = true;   // PES status

  const uint16_t PID = 136;    // Packet Identifier
  const uint8_t size = 188;

//=============================================================================================================================================================================

  uint32_t length = 0;
  uint32_t PES_ID = 0;
  uint32_t CC_control = 0;

  FILE *file = fopen("../example_new.ts", "rb");
  if(!file) printf("Error");

  fseek(file, 0L, SEEK_END);
  length = ftell(file);
  fseek(file, 0L, SEEK_SET);

  TS_Packet *arrayOfTSpacket = new TS_Packet[length/188];

  PES_Packet *arrayOfPESpacket = new PES_Packet[100000]; //tymczasowe rozw LISTA

  uint8_t *buffer = new uint8_t[size];

  for(uint32_t i = 0; i < length/188; i++){   //length/188
    fread( buffer, sizeof(uint8_t), size, file);
    arrayOfTSpacket[i].ParseHeader(buffer);
  }
  
  for(uint32_t i = 0; i < length/188; i++){   //length/188
    if(arrayOfTSpacket[i].Get_H_PID() == PID){
      //if(!(arrayOfPESpacket[PES_ID].IsFill())){
        if(CC_control == arrayOfTSpacket[i].Get_H_CC()){
          arrayOfPESpacket[PES_ID].Builder(arrayOfTSpacket[i].GetPayload(), arrayOfTSpacket[i].GetSizeOfPayload(), 0);
          CC_control++;
          if(arrayOfPESpacket[PES_ID].IsFill()){
            PES_ID++;
            CC_control = 0;
          }
        } else{
          if(arrayOfTSpacket[i].Get_H_PID() != PID){
            printf("\n------------------------------------------------------------\nERROR! WRONG CONTINUITY COUNTER! DATA LOST! FOR PID: %d\n  parsing continue for PID = %d\n------------------------------------------------------------\n", arrayOfTSpacket[i].Get_H_PID(),PID);
            continue; // what now?
          } else{
            printf("\n------------------------------------------------------------\nERROR! WRONG CONTINUITY COUNTER! DATA LOST! FOR PID: %d\n  FATAL ERROR! PARSING BREAK FOR PID: %d\n------------------------------------------------------------\n", arrayOfTSpacket[i].Get_H_PID(),PID);
            break; // what now?
          }
          //SPRAWDZIC CO TU SIE DZIEJE?????????????????????????????????????????????????????????????????????????????????????? posprawdzac dane jakie tam daje
        }
        
      //}else if(arrayOfPESpacket[PES_ID].IsCutPayload()){
      //  PES_ID++;
      //  arrayOfPESpacket[PES_ID].Builder(arrayOfTSpacket[i].GetPayload(), arrayOfTSpacket[i].GetSizeOfPayload(),arrayOfPESpacket[PES_ID-1].GetlastPositionInPayload());
      //}
    }
    if(SHOW_ID) printf("%010d ", i);
    if(SHOW_TS_HD) arrayOfTSpacket[i].PrintHeader();
    if(SHOW_TS_AF) arrayOfTSpacket[i].PrintAdaptationField();
    if(SHOW_TS_PL) arrayOfTSpacket[i].PrintIsPayload();
    if(SHOW_PES_ST){
      if(arrayOfTSpacket[i].Get_H_PID() == PID){
        if(arrayOfPESpacket[PES_ID].isPreInit == true && arrayOfPESpacket[PES_ID].isFill == false){
          if(arrayOfTSpacket[i].Get_H_CC() == 0){
            printf("<s>---STARTED  ");
            arrayOfPESpacket[PES_ID].PrintHeader();
          } else{
            printf(" |");
          }
        } else if(arrayOfPESpacket[PES_ID-1].isPreInit == true && arrayOfPESpacket[PES_ID-1].isFill == true){
          printf("[f]---FINISHED Total length: %d", arrayOfPESpacket[PES_ID-1].H_PESPL + 6 + arrayOfPESpacket[PES_ID-1].H_PESHDL);
        }
      }
    }
    if(SHOW_ID || SHOW_TS_HD || SHOW_TS_AF || SHOW_TS_PL || SHOW_PES_ST) printf("\n");
  }
  
  for(uint32_t i = PES_ID; i > 0; i--){
    arrayOfPESpacket[PES_ID].ParseAditionalHeader();
  }

/*
  printf("\n\n");
  for(int i = 0; i < arrayOfPESpacket[0].H_PESPL; i++){
    printf("%d ", arrayOfPESpacket[0].arrayOfPayload[i]);
    //printf("%d ", i);
  }
  printf("\n\n");
  for(int i = 0; i < arrayOfTSpacket[3].sizeOfPayload; i++){
    printf("%d ", arrayOfTSpacket[3].payload[i]);
    //printf("%d ", i);
  }
  printf("\n\n");
    for(int i = 0; i < arrayOfTSpacket[4].sizeOfPayload; i++){
    printf("%d ", arrayOfTSpacket[4].payload[i]);
    //printf("%d ", i);
  }
  printf("\n\n");
  arrayOfPESpacket[1].PrintHeader();
*/

  fclose(file);
  return 0;
}