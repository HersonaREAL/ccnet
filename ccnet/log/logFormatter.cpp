#include "log.h"
#include <sstream>


namespace ccnet {
LogFormatter::LogFormatter(const std::string &pattern) 
    : m_pattern(pattern) 
{

}

std::string LogFormatter::format(LogEvent::ptr event)
{
    std::stringstream ss;
    for (auto &item : m_items) {
        item.format(ss, event);
    }

    return ss.str();
}

void LogFormatter::init()
{

}

}