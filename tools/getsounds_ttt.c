
/* 

get characters data from lord.exe

 */


#include<stdio.h>


int start[4] = { 'I', 'N', 'T', 'R' };

char name[128];

int main( void )
{

  FILE *in;
  int n, k, c;

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


  name[0]='i';
  name[1]='n';
  name[2]='t';
  name[3]='r';
  k=4;

  printf( "char *sound_names[] =  { " );

  n=0;

  while( 1 )
    {

      c=fgetc(in);
      if( c>='A' && c<='Z' ) c=c-'A'+'a';
      name[k++]=c;

      if( c==0 )
	{
	  for( k=0; name[k]!='.' &&  name[k]!=0; ++k );
	  name[k]=0;
	  printf( "/* %2x */ \"%s\",\n                         ", n++, name );
	  if( !strcmp( name, "magicf" ) ) break;
	  k=0;
	}


    }
  
  printf( "\"\" };\n" );

  return 0;
}
