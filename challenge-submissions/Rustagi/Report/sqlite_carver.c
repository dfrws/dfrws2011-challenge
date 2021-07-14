#include <stdio.h>
#include <string.h>
int check (char c)
{
    if (c == 0x01 | c==0x02 | c==0x03 | c==0x04 | c==0x05)// The pointer map can have only these 5 type of records. If any other value is found, it marks the end of the pointer map.
    return 1;
    return 0;
}

main()
{
      FILE *fp,*fp1;
      char string[]="SQLite format 3";             
      char a[100],*start;
      char filename[100];
      int count = 0,size=0,records,page,maps,found=0,file=0,offset_start;
      char b[15]={};
      int i=0;
      char c;
      printf("Please enter the path of the file(without any spaces):  "); // Please keep the .exe in the same folder as of the dump file
      scanf("%s",a);
      fp = fopen(a,"rb");
      printf("%d\n",fp);
      while(fp!=EOF)
      {
          fread(b,1,15,fp); //Read the following 15 bytes from the dump
          i++;
          if(!strncmp(b,string,15)) //Compare with SQLite header
          {
                offset_start=(ftell(fp)-15); //Great !!! We found the SQLite header and we are marking the offset
                records = 0;
                page = 0;
                maps = 0;
                fseek(fp,1009,SEEK_CUR); //Move to the page 2 of the database which contains Pointer Map records. The page size of SQLite database in Android is 1024
                                         //The 1024 size can be found at offset 16 on page 1 of the database.
                
                while (check(c=fgetc(fp))) //Count the number of records for pointer map pages
                {
                     records++;
                     found = 1;
                     if(records==204) // The number of records > 204 Hence we need to move 207th page.
                     {
                          maps++;
                          page++;
                          fseek(fp,8+(1024*204),SEEK_CUR); //Moving to the next page containing pointer records
                          records = 0;
                          }
                     else
                     {
                         page++;
                         fseek(fp,4,SEEK_CUR); // Moving to the next record within the same page
                         }
                }
                if(found) //We now know the starting offset and size of the database. Now carve the file
                {
                         file++;
                         itoa(file,filename,10);
                         strcat(filename,".db");
                         fp1 = fopen(filename,"wb");
                         printf("File found at offset %ld\n",offset_start);
                         printf("Carving file %s Total size is %ld\n",filename,(page+maps+2)*1024);
                         found=0;
                         fseek(fp,offset_start,SEEK_SET);
                         for(i=0;i<((page+maps+2)*1024);i++)
                         {
                              c=getc(fp);
                              putc(c,fp1);
                              }  
                         fclose(fp1);
                         printf("Your file is ready to check :)\n\n");
                         fseek(fp,offset_start+1024,SEEK_SET);                        
                         }         
          }
          else
          fseek(fp,-14,SEEK_CUR);
          
       }
    
      printf("Total occurrence is %d ",count);
      getch();
}

      
      
