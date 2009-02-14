
/* 

get characters data from lord.exe

 */


#include<stdio.h>


int start[4] = { 0x14, 0x07, 0x30, 0x01 };

int main( void )
{

  FILE *in;
  int n, x, y, m;

  in = fopen( "towers.exe", "rb" );

  n=0;

  /* find start sequence  */

  while( n<4 )
    {
      if( fgetc(in)==start[n] )
	++n;
      else
	n=0;
    }



  printf( "int building_enters[][3] = { " );

  n=0;
  x=0x714;
  y=0x130;
  m=0;

  while( x!=0xffff )
    {

      printf( "{ 0x%02x, 0x%04x, 0x%04x } /* %2d */", m, x, y, n );
      
      m=fgetc(in);
      fgetc(in);

      x=fgetc(in);
      x+=0x100*fgetc(in);

      y=fgetc(in);
      y+=0x100*fgetc(in);

      if( x!=0xffff )
	printf( ",\n                             " );
      ++n;

    }
  
  printf( " };\n" );

  return 0;
}
