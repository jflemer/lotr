
/* 

get characters data from lord.exe

 */


#include<stdio.h>


int start[4] = { 's', 'i', 'l', 'v' };

char name[128];

int main( void )
{

  FILE *in;
  int n, k, c;

  in = fopen( "lord.exe", "rb" );

  n=0;

  /* find start sequence  */

  while( n<4 )
    {
      if( fgetc(in)==start[n] )
	++n;
      else
	n=0;
    }


  name[0]=start[0];
  name[1]=start[1];
  name[2]=start[2];
  name[3]=start[3];
  k=4;

  printf( "char *object_names1[] =  { " );

  n=0;

  while( 1 )
    {

      c=fgetc(in);

      if( c==0xff ) break;

      if( c!='!' && c!='-' && c!=' ' && c!='\'' && ( c<'A' || c>'Z' ) && ( c<'a' || c>'z' ) )
	{

	  if( c==0 )
	    {
	      c=fgetc(in);
	      ungetc( c, in );

	      if( c!=0xb2 )
		c=0;
	      else
		c=1;
	    }

	  if( c!=0 )
	    {
	      k-=2;
	      while( c!=0xb2 )
		{
		  k++;
		  c=fgetc(in);
		}
	    }
	  name[k]=0;

	  printf( "/* %2x */ \"%s\",\n                           ", n++, name );

	  if( c!=0 )
	    {
	      fgetc(in);
	      k=fgetc(in)-1;
	      fgetc(in);
	      fgetc(in);
	      while( k-- )
		printf( "/* %2x */ \"%s\",\n                           ", n++, "" );
	    }
	  k=0;
	}      
      else
	{
	  name[k++]=c;
	}
      

    }
  
  printf( "\"\" };\n" );

  return 0;
}
