#include <stdio.h>
#include <string.h>

main()
{
      FILE *fp,*fp2;
      fp = fopen("mtdblock6.img","rb");// Important: Please keep the executable in the same folder as the file mtdblock6.img
      //fp2 = fopen("Case1_IH.xls","wb");
      //printf("fp2 %x\n",fp2);
      //getch();
      int i,max_size,returnx,count=0;
      unsigned int signature2 = 0x00000001;
      unsigned int signature1= 0x00010501;
      unsigned char http[4]={'h','t','t','p'};
      unsigned char c[4]={};
      unsigned char title [100];
      unsigned int sig2 = 0;
      unsigned int sig1 = 0;
      unsigned char date[6]={};
      unsigned char read,type,m;
      fseek(fp,0,SEEK_END);
      max_size=ftell(fp);
      printf("File size is %d bytes\n",max_size);
      fseek(fp,0,SEEK_SET);
      while(ftell(fp)<max_size)
      {
           fread(&sig1,1,4,fp);
           fread(&sig2,1,4,fp);
           //printf("%x\n",b);
           //printf("%x\n",signature);    
          // getch();
           if(sig1==signature1 && sig2==signature2)   
            {
               m = 'A';                       
               count++;
              // printf("Count %d Offset %d \n",count,ftell(fp));
               //fprintf(fp2,"%d;%d;",count,ftell(fp));
               do
               {
                   m = getc(fp);
                   printf("%c",m);   
                   fread(c,1,4,fp);
                   fseek(fp,-4,SEEK_CUR);
                }while (strncmp(c,http,4));
               printf(";");
               //printf("%s",c);
               do
               {
                   m = getc(fp);
                   if(m!=0x01)
                   printf("%c",m);
               }while (m!=0x01);
               printf(";");
               fread(date,1,6,fp);
               for(i=0;i<=5;i++) 
               printf("%.2x",date[i]);
               printf(";Count %d Offset %d \n",count,ftell(fp));
               printf("\n");
               
             }
             else
             fseek(fp,-7,SEEK_CUR);
      }                                      
      fclose(fp);
      fclose(fp2);
      printf("titles are now carved\n");
      getch();
}
