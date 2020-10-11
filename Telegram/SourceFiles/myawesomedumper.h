
// mtproto/session_private.cpp
void myawesomedumper_Dump(void*from, unsigned int size, int recv);

int myawesomedumper_Start(char*arg1,char*arg2);

/*
#define myawesomedumper_cpp
#include "myawesomedumper.h"
*/

#ifdef myawesomedumper_cpp
#undef myawesomedumper_cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mtproto/core_types.h"
#include "mtproto/details/mtproto_dump_to_text.h"

FILE *myfile = NULL;
int tdesktop_api_layer = 119;

void myawesomedumper_Dump(void*from, unsigned int size, int recv){
  if(!myfile){
    char str[256];
    sprintf(str,"myawesomedumper_%u.bin\0",(unsigned int)time(NULL));
    myfile = fopen(str,"wb");
    if(!myfile){
      return;
    }
    fwrite(tdesktop_api_layer,4,1,myfile);
  }
  int head = (int)size;
  if(recv){
    head = -head;
  }
  fwrite(&head,4,1,myfile);
  fwrite(from,size,1,myfile);
};

int myawesomedumper_Start(char*arg1,char*arg2){
  if(myfile || !arg1 || !arg1[0] || !arg2 || !arg2[0]){
    return 0;
  }
  if(strcmp(arg1,"-myawesomedumper")
  && strcmp(arg1,"/myawesomedumper")
  && strcmp(arg1,"myawesomedumper")
  && strcmp(arg1,"--myawesomedumper")){
    return 0;
  }
  FILE *bin = fopen(arg2,"rb");
  if(!bin){
    return 0;
  }
  int layer = 0;
  fread(&layer,4,1,bin);
  if(layer!=tdesktop_api_layer){
    return 1;
  }
  char *name = (char*)malloc(strlen(arg2)+8);
  if(!name){
    fclose(bin);
    return 1;
  }
  strcpy(name,arg2);
  strcat(name,".txt\0");
  FILE *txt = fopen(name,"w");
  free(name);
  if(!txt){
    fclose(bin);
    return 1;
  }
  int head,recv;
  int size = 1024;
  void *buf = (void*)malloc(size);
  const mtpPrime *from;
  if(buf){
    while(true){
      if(fread(&head,4,1,bin)<1){
        break;
      }
      if(head<0){
        recv = 1;
        head = -head;
      }else{
        recv = 0;
      }
      if(head>size){
        size = head;
        buf = (void*)realloc(buf,size);
        if(!buf){
          break;
        }
      }
      if(fread(buf,head,1,bin)<1){
        break;
      }
      from = (const mtpPrime*)buf;
      fprintf(
        txt,
        (recv?"Recv:\n%s\n":"Send:\n%s\n"),
        MTP::details::DumpToText(
          from,
          (mtpPrime*)(((char*)buf)+head)
        ).toUtf8().data()
      );
    }
  }
  if(buf){
    free(buf);
  }
  fclose(bin);
  fclose(txt);
  return 1;
};

#endif

//EOF