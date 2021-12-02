#include <iostream>
#include "../ccnet/log/log.h"


using namespace ccnet;
int main(int argc, char *argv[]) { 
    Logger::ptr logger(new Logger());
    logger->addAppender(LogAppender:ptr(new StdoutLogAppender()));
}
