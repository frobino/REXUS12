/*
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
FILE* fp=fopen("/dev/sdb","r"); 
    
return (0);
} 
*/

/* fopen example */
#include <stdio.h>
int main ()
{
  FILE * pFile;
  pFile = fopen ("/media/Vol1/myfile.txt","w");
  if (pFile!=NULL)
  {
    fputs ("fopen example",pFile);
    fclose (pFile);
  }
  return 0;
}
