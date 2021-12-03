#include "utils.h"
#include <bits/stdint-uintn.h>

namespace ccnet {
pid_t getThreadId() {
    return syscall(SYS_gettid);
}

uint32_t getFiberId() {
    //TODO
    return 0;
}

}