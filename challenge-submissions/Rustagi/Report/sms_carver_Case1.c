#include <stdio.h>
#include <string.h>

main()
{
      FILE *fp,*fp2;
      fp = fopen("mtdblock6.img","rb"); // Please make sure that mtdblock6.img is in the same folder as the executable.
      fp2 = fopen("Case1_sms.xls","wb");
      fclose (fp2);
      int i,max_size,count=0;
      unsigned char phone_no_mre1[13] = {	0x34, 0x34, 0x33, 0x39, 0x32, 0x36, 0x34, 0x37, 0x36, 0x38, 0x01, 0x01, 0x2F};
      unsigned char phone_no_taog1[13] = {   0x34, 0x31, 0x32, 0x34, 0x33, 0x39, 0x33, 0x33, 0x38, 0x38, 0x02, 0x01, 0x2F};
      unsigned char phone_no_mre2[12] = {	0x34, 0x34, 0x33, 0x39, 0x32, 0x36, 0x34, 0x37, 0x36, 0x38, 0x01, 0x2F};
      unsigned char phone_no_taog2[12] = {	0x34, 0x31, 0x32, 0x34, 0x33, 0x39, 0x33, 0x33, 0x38, 0x38, 0x01, 0x2F};
      
      char b[13]={};
      char number[11]={};
      unsigned char date[6]={};
      //long int time=0;
      unsigned char read,type,m;
      fseek(fp,0,SEEK_END);
      max_size=ftell(fp);
      printf("File size is %d bytes\n",max_size);
      fseek(fp,0,SEEK_SET);
      while(ftell(fp)<max_size)
      {
           fread(b,1,13,fp);
           //time=0;
           if(!strncmp(b,phone_no_mre1,13))
           {
               fp2 = fopen("Case1_sms.xls","ab");
               m = 'A';                       
               count++;
               printf("Count %d Offset %d mre1\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,phone_no_mre1,10);
               strcat(number,"\0");
               //printf("Phone number: %s\n",number);
               fprintf(fp2,"%s;",number);
               fseek(fp,-2,SEEK_CUR);
               fread(date,1,6,fp);
               //printf("Date: %ld",time);
               
               for(i=0;i<=5;i++) 
               fprintf(fp2,"%.2x",date[i]);
               fprintf(fp2,";");
               fseek(fp,1,SEEK_CUR);//skip protocol
               read = getc(fp);
               fprintf(fp2,"%x;",read);
               fseek(fp,1,SEEK_CUR);//skip status
               type = getc(fp);
               fprintf(fp2,"%x;",type);
               fseek(fp,1,SEEK_CUR);//skip replypathpresent
               //printf("Message : ");
               while(m!='\0')
               {
                  m = getc(fp);
                  if(m!=0x0A)
                  fprintf(fp2,"%c",m);
                  else fprintf(fp2," ");
               }
               fprintf(fp2,"\n");
               fclose(fp2);
           }
           else if (!strncmp(b,phone_no_taog1,13))
           {
               fp2 = fopen("Case1_sms.xls","ab");
               m = 'A';                       
               count++;
               printf("Count %d Offset %d taog1\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,phone_no_taog1,10);
               strcat(number,"\0");
               //printf("Phone number: %s\n",number);
               fprintf(fp2,"%s;",number);
               fseek(fp,-2,SEEK_CUR);
               fread(date,1,6,fp);
               //printf("Date: ");
               for(i=0;i<=5;i++) 
               fprintf(fp2,"%.2x",date[i]);
               fprintf(fp2,";");
               fseek(fp,1,SEEK_CUR);//skip protocol
               read = getc(fp);
               fprintf(fp2,"%x;",read);
               fseek(fp,1,SEEK_CUR);//skip status
               type = getc(fp);
               fprintf(fp2,"%x;",type);
               fseek(fp,1,SEEK_CUR);//skip replypathpresent
               //printf("Message : ");
               while(m!='\0')
               {
                  m = getc(fp);
                  if(m!=0x0A)
                  fprintf(fp2,"%c",m);
                  else fprintf(fp2," ");
               }
               fprintf(fp2,"\n");
               fclose(fp2);
          
           }
           else if(!strncmp(b,phone_no_mre2,12))
           {
               fp2 = fopen("Case1_sms.xls","ab");
               m = 'A';                       
               count++;
               printf("Count %d Offset %d mre2\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,phone_no_mre2,10);
               strcat(number,"\0");
               fprintf(fp2,"%s;",number);
               fseek(fp,-3,SEEK_CUR);
               fread(date,1,6,fp);
               //printf("Date: ");
               for(i=0;i<=5;i++) 
               fprintf(fp2,"%.2x",date[i]);
               fprintf(fp2,";");
               //fseek(fp,1,SEEK_CUR);//skip protocol
               read = getc(fp);
               fprintf(fp2,"%x;",read);
               fseek(fp,1,SEEK_CUR);//skip status
               type = getc(fp);
               fprintf(fp2,"%x;",type);
               //fseek(fp,1,SEEK_CUR);//skip replypathpresent
               //printf("Message : ");
               while(m!='\0')
               {
                  m = getc(fp);
                  if(m!=0x0A)
                  fprintf(fp2,"%c",m);
                  else fprintf(fp2," ");
               }
               fprintf(fp2,"\n");
               fclose(fp2);
          
           }
           else if(!strncmp(b,phone_no_taog2,12))
           {
               fp2 = fopen("Case1_sms.xls","ab");
               m = 'A';                       
               count++;
               printf("Count %d Offset %d taog2\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,phone_no_taog2,10);
               strcat(number,"\0");
               fprintf(fp2,"%s;",number);
               fseek(fp,-3,SEEK_CUR);
               fread(date,1,6,fp);
               //printf("Date: ");
               for(i=0;i<=5;i++) 
               fprintf(fp2,"%.2x",date[i]);
               fprintf(fp2,";");
               fseek(fp,1,SEEK_CUR);//skip protocol
               read = getc(fp);
               fprintf(fp2,"%x;",read);
               fseek(fp,1,SEEK_CUR);//skip status
               type = getc(fp);
               fprintf(fp2,"%x;",type);
               fseek(fp,1,SEEK_CUR);//skip replypathpresent
               //printf("Message : ");
               while(m!='\0')
               {
                  m = getc(fp);
                  if(m!=0x0A)
                  fprintf(fp2,"%c",m);
                  else fprintf(fp2," ");
               }
               fprintf(fp2,"\n");
          
           }
           else
           fseek(fp,-12,SEEK_CUR);
           fclose(fp2);
      }
      fclose(fp);
      printf("SMSes are now carved\n");
      getch();
}        
           
