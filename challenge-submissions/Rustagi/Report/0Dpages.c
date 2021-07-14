#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

main()
{
      FILE *fp1,*fp2;
      int filenumber=1,i,offset,offset_end;
      char filename1[100],filename2[100],c;
      while(filenumber<=1909)
      {
           itoa(filenumber,filename1,10);
           strcpy(filename2,filename1);
           strcat(filename1,".db");
           strcat(filename2,".0D");
           
           fp1 = fopen(filename1,"rb");
          
           if(fp1)
           {
                  fp2 = fopen(filename2,"wb");
                  offset=0;
                  fseek(fp1,0,SEEK_END);
                  offset_end = ftell(fp1);
                  system("cls");
                  printf("file %s found and ends at %d\n",filename1,offset_end);
                  fseek(fp1,1024,SEEK_SET);
                  while (offset<offset_end)
                  {   
                      offset = ftell(fp1);
                      c = getc(fp1);
                                  
                      if(offset%1024 == 0 && c==0x0D) 
                      {  
                         printf("\nNow at offset %d\n",offset); 
                         for(i=0;i<=1023;i++)
                         {
                             putc(c,fp2);
                            // printf("%.2x ",c);
                             c=getc(fp1);
                         }   
                         fseek(fp1,-1,SEEK_CUR);
                        // getch();
                                     
                        // getch();
                      }
                  
                  }
                  fclose(fp2);
                  fclose(fp1);
                 // getch(); 
           }
               
           filenumber++;
        }
}
                              
