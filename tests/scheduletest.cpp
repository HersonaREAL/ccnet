#include <ccnet.h>

using namespace ccnet;
int main() {
    Scheduler sc(1);
    sc.start();
    sc.stop();
}