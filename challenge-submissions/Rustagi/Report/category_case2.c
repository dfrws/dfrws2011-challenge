#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
main()
{
      int filenumber=1,temp,signature;
      FILE *fp;
      char filename[100],directory[1000],path[1000];
      system("mkdir contacts2,cached_geopositions,browser,webview,downloads,mmsssms,assets,fb,mailstore,twitter,misc");
      while (filenumber<=1086)
      {
            itoa(filenumber,filename,10);
            strcat(filename,".db");            
            printf("file %s ",filename);
            //getch();
            fp = fopen(filename,"rb"); //Important: Please keep the executable in the same folder as the db files.
            fseek(fp,103,SEEK_SET);
            fread(&temp,4,1,fp);
            signature = ((temp>>24)&0xff) | ((temp<<8)&0xff0000) |((temp>>8)&0xff00) | ((temp<<24)&0xff000000);
            printf("%.8x ",signature);
            switch(signature)
            {
                   case 0x00180388:
                        strcpy(directory,"contacts2");
                        break;
                   case 0x00010231:
                        strcpy(directory,"cached_geopositions");
                        break;
                   case 0x0003022D:
                        strcpy(directory,"browser");
                        break;
                   case 0x000103FB:
                        strcpy(directory,"webview");
                        break;
                   case 0x00030102:
                        strcpy(directory,"downloads");
                        break;
                   case 0x000D03BF:
                        strcpy(directory,"mmsssms");
                        break;
                   case 0x0004016F:
                        strcpy(directory,"assets");
                        break;
                   case 0x000403EC:
                        strcpy(directory,"fb");
                        break;
                   case 0x000603E2:
                        strcpy(directory,"mailstore");
                        break;
                   case 0x000703DD:
                        strcpy(directory,"twitter");
                        break;
                   case 0x0003022A:
                        strcpy(directory,"btopp");
                        break;
                    
                   default:
                           strcpy(directory,"misc");
                           }
            strcpy(path,"copy ");
            strcat(path,filename);
            strcat(path," .\\");
            strcat(path,directory);
            strcat(path,"\\");
            printf("%s\n",path);
            system(path);
            //getch();
            filenumber++;
            fclose(fp);
            }
            getch();
}
            
            
