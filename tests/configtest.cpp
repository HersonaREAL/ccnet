#include <log.h>
#include <config.h>

using namespace ccnet;


int main(int argc, char *argv[]) {
    ConfigVar<int>::ptr int_val = Config::lookup("system.port", 8080, "system port");
    LOG_INFO() << int_val->getVal();
    LOG_INFO() << int_val->toString();

    return 0;
}