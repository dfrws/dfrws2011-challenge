#include <stdio.h>
#include <string.h>

main()
{
      FILE *fp,*fp2;
      fp = fopen("mtd8.dd","rb");
      fp2 = fopen("Case2_sms.xls","wb");
      fclose(fp2);
      int i,max_size,count=0;
      unsigned char shandra[22] = {0x73, 0x68, 0x61, 0x6E, 0x64, 0x72, 0x61, 0x40, 0x63, 0x68, 0x65, 0x65, 0x72, 0x66, 0x75, 0x6C,	0x2E, 0x63, 0x6F, 0x6D, 0x01, 0x2F};
      unsigned char v6245[6] = {0x36, 0x32, 0x34, 0x35, 0x01, 0x2F};
      unsigned char dynadel[23] = {0x73, 0x6D, 0x73, 0x2E, 0x64, 0x79, 0x6E, 0x61, 0x64, 0x65, 0x6C, 0x40, 0x67, 0x6D, 0x61, 0x69, 0x6C, 0x2E, 0x63, 0x6F, 0x6D, 0x01, 0x2F};
      unsigned char phone_no_taog2[12] = {	0x34, 0x31, 0x32, 0x34, 0x33, 0x39, 0x33, 0x33, 0x38, 0x38, 0x01, 0x2F};
      
      char b[23]={};
      char number[21]={};
      unsigned char date[6]={};
     // long int time=0;
      unsigned char read,type,m;
      fseek(fp,0,SEEK_END);
      max_size=ftell(fp);
      printf("File size is %d bytes\n",max_size);
      fseek(fp,0,SEEK_SET);
      while(ftell(fp)<max_size)
      {
           fread(b,1,23,fp);
    //       time=0;
             for(i=0;i<=20;i++)
             number[i]='\0';
           if(!strncmp(b,shandra,22))
           {
               fp2 = fopen("Case2_sms.xls","ab");                      
               m = 'A';                       
               count++;
               printf("Count %d Offset %d shandra\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,shandra,20);
               strcat(number,"\0");
               //printf("Phone number: %s\n",number);
               fprintf(fp2,"%s;",number);
               fseek(fp,-3,SEEK_CUR);
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
           else if (!strncmp(b,v6245,6))
           {
               fp2 = fopen("Case2_sms.xls","ab");  
               m = 'A';                       
               count++;
               printf("Count %d Offset %d 6245\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,v6245,4);
               strcat(number,"\0");
               //printf("Phone number: %s\n",number);
               fprintf(fp2,"%s;",number);
               fseek(fp,-19,SEEK_CUR);
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
           else if(!strncmp(b,dynadel,23))
           {
               fp2 = fopen("Case2_sms.xls","ab"); 
               m = 'A';                       
               count++;
               printf("Count %d Offset %d dynadel\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,dynadel,21);
               strcat(number,"\0");
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
           else if(!strncmp(b,phone_no_taog2,12))
           {
               fp2 = fopen("Case2_sms.xls","ab");  
               m = 'A';                       
               count++;
               printf("Count %d Offset %d taog2\n",count,ftell(fp));
               fprintf(fp2,"%d;%d;",count,ftell(fp));
               strncpy(number,phone_no_taog2,10);
               strcat(number,"\0");
               fprintf(fp2,"%s;",number);
               fseek(fp,-13,SEEK_CUR);
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
           else
           fseek(fp,-22,SEEK_CUR);
           
      }
      fclose(fp);
      printf("SMSes are now carved\n");
      getch();
}        
           
