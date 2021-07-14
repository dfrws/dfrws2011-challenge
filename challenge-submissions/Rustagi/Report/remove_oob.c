#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>
main()
{
      FILE *fp, *fp2;
      char c;
      int eof,i,j=1;
      char filename[100],filename2[100];
      printf("Enter the filename without any spaces: "); //Important: Please keep the executable in the same folder as the dump file.
      scanf("%s",filename);
      strcpy(filename2,filename);
      strcat(filename2,"oob");
      fp = fopen(filename,"rb");
      fp2 = fopen(filename2,"wb");
      fseek(fp,0,SEEK_END);
      eof = ftell(fp);
      printf("End of file is at %d\n",eof);
      fseek(fp,0,SEEK_SET);
      while((ftell(fp))<=eof)
      {
          c = getc(fp);
          putc(c,fp2);
          if (j==2048)
          {
               j = 0;
               fseek(fp,64,SEEK_CUR);
               //printf("Skipping to %d\n",ftell(fp));
               //getch();
          }
          j++;
                      
      }
          
      printf("Your file %s is ready to check", filename2);
      fclose(fp);
      fclose(fp2);         
      getch();
}
