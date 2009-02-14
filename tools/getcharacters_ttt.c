
/* 

get characters data from lord.exe

 */


#include<stdio.h>


int start[4] = { 0xa0, 0x00, 0x64, 0x00 };

int main( void )
{

  FILE *in;
  int x, y, c, char_num, a, b, d, e, f, g;
  int n;

  in = fopen( "towers.exe", "rb" );

  n=0;

  /* find sequence  0xa0, 0x00, 0x64, 0x00 */

  while( n<4 )
    {
      if( fgetc(in)==start[n] )
	++n;
      else
	n=0;
    }


  x=start[0]+start[1]*0x100;
  y=start[2]+start[3]*0x100;


  printf( "int characters_pos[][10] =   { " );

  n=0;

  while( x!=0xffff )
    {

      c=fgetc(in);
      char_num=fgetc(in);
      a=fgetc(in);
      b=fgetc(in);
      d=fgetc(in);
      e=fgetc(in);
      f=fgetc(in);
      g=fgetc(in);

      printf( "{ 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x }, /* %3d */\n                               ", x, y, c, char_num, a, b, d, e, f, g, n++ );

      x=fgetc(in)+fgetc(in)*0x100;
      y=fgetc(in)+fgetc(in)*0x100;      
    }
  
  printf( "{ 0xffff, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };\n" );

  return 0;
}
