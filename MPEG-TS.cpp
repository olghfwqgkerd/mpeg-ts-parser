#include "TransportStream.h"

#include <iostream>
#include <bitset>

int main(int argc, char *argv[ ], char *envp[ ])
{
  uint32_t tsPacketId = 0;
  uint64_t length = 0;
  const int size = 188;
  TS_Packet tsPacket;

  FILE * file = fopen( "../example_new.ts", "rb" );
  if( !file ) printf("Error");

  fseek(file, 0L, SEEK_END);
  length = ftell(file);
  fseek(file, 0L, SEEK_SET);

  TS_Packet* arrayOfTSpacket = new TS_Packet[length/188];
  uint8_t* buffer = new uint8_t[size];

  for(uint32_t i = 0; i < 25; i++)
  {
    fread( buffer, sizeof( uint8_t ), size, file );
    arrayOfTSpacket[i].ParseHeader( buffer );
  }
  
  for(uint32_t i = 0; i < 25; i++)
  {
    printf("%010d ", tsPacketId);
    arrayOfTSpacket[i].PrintHeader();
    arrayOfTSpacket[i].PrintAdaptationField();
    printf("\n");
    tsPacketId++;
  }

  fclose( file );
  return 0;
}