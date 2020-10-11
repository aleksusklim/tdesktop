/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "core/launcher.h"

#define myawesomedumper_cpp
#include "myawesomedumper.h"

int main(int argc, char *argv[]) {
  
  if(argc==3 && myawesomedumper_Start(argv[1],argv[2])){
    return 0;
  }
  
	const auto launcher = Core::Launcher::Create(argc, argv);
	return launcher ? launcher->exec() : 1;
}
