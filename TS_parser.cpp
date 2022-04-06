#include "tsCommon.h"
#include "tsTransportStream.h"

#include <iostream>
#include <bitset>

int main(int argc, char *argv[ ], char *envp[ ])
{
  const int size = 188;

  xTS_PacketHeader    TS_PacketHeader;

  FILE * pFile = fopen( "../example_new.ts", "rb" );
  if( !pFile ) printf("Error");

      uint32_t* bufor = new uint32_t[size];

  int32_t TS_PacketId = 0;
  while( !feof( pFile ) )
  {


    fread( bufor, sizeof( uint8_t ), size, pFile );

    TS_PacketHeader.Reset();
    TS_PacketHeader.Parse( bufor );

    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    printf("\n");

    TS_PacketId++;
  }
  
  return 0;
}