
// mtproto/session_private.cpp
void myawesomedumper_Dump(void*from, unsigned int size, int recv, int session);

int myawesomedumper_Start(char*arg1,char*arg2);

/*
#define myawesomedumper_cpp
#include "myawesomedumper.h"
*/

#ifdef myawesomedumper_cpp
#undef myawesomedumper_cpp

//const char* tdesktop_api_layer = "05\x0d\x0a";
const char* tdesktop_api_layer = "19\x0d\x0a";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <QDateTime>

#include "base/unixtime.h"
#include "mtproto/core_types.h"
#include "mtproto/details/mtproto_dump_to_text.h"

FILE *myfile = NULL;

void myawesomedumper_Dump(void*from, unsigned int size, int recv, int session){
  if(!myfile){
    char str[256];
    sprintf(str,"myawesomedumper_%u.bin\0",(unsigned int)time(NULL));
    myfile = fopen(str,"wb");
    if(!myfile){
      return;
    }
    fwrite(tdesktop_api_layer,4,1,myfile);
  }
  int head[3];
  head[0] = (int)time(NULL);
  head[1] = session;
  head[2] = (int)size;
  if(recv){
    head[2] = -head[2];
  }
  fwrite(head,12,1,myfile);
  fwrite(from,size,1,myfile);
  fflush(myfile);
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
  if(layer!=*((int*)tdesktop_api_layer)){
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
  int head[3];
  int recv, block;
  int size = 256*1024;
  void *buf = (void*)malloc(size);
  const mtpPrime *from;
  if(buf){
    while(true){
      if(fread(head,12,1,bin)<1){
        break;
      }
      block = head[2];
      if(block<0){
        recv = 1;
        block = -block;
      }else{
        recv = 0;
      }
      if(block>size){
        size = block;
        buf = (void*)realloc(buf,size);
        if(!buf){
          break;
        }
      }
      if(fread(buf,block,1,bin)<1){
        break;
      }
      from = (const mtpPrime*)buf;
      fprintf(
        txt,
        "%s/%d: %u (%s)\n%s\n",
        (recv?"Recv":"Send"),
        head[1],
        (unsigned int)head[0],
        base::unixtime::parse(head[0])
          .toString("yyyy.MM.dd, hh:mm:ss").toUtf8().data(),
        MTP::details::DumpToText(
          from,
          (mtpPrime*)(((char*)buf)+block)
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