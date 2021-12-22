#include <utils.h>
#include <log.h>
#include <iostream>
#include <assert.h>

void TEST_ASSERT()
{
    // std::cout << ccnet::backTraceToString(10, 0, "test prefix: ") << std::endl;
    CCNET_ASSERT(1 == 1)
    CCNET_ASSERT_EX(1 == 0, "just test assert 2 %d %c %f %s",666, 'g', 1.33, "sb");
}

int main() {
    TEST_ASSERT();
    return 0;
}