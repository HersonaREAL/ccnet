#include "log.h"
#include <cstddef>
#include <ctime>
#include <memory>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>
#include <unordered_map>
#include <functional>


namespace ccnet {
class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string str) {}
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << '\t';
    }
};


class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string str)
        : m_str(str) {}
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << m_str;
    }

private:
    std::string m_str;
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:	
    NewLineFormatItem (const std::string &fmt = "") {};
	void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << std::endl;
    }
};

class MessageFormatItem : public LogFormatter::FormatItem {
public:	
    MessageFormatItem (const std::string &fmt = "") {};
	void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getThreadId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getThreadName();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        : m_format(format) 
    { 
        if (m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        struct tm t;
        char buf[128];
        time_t time = ev->getTime();
        localtime_r(&time, &t);
        strftime(buf, sizeof(buf), m_format.c_str(), &t);
        os << buf;
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem {
public:
    FileNameFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem (const std::string &fmt = "") {};
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr ev) override {
        os << ev->getLine();
    }
};
 
LogFormatter::LogFormatter(const std::string &pattern) 
    : m_pattern(pattern) 
{
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for (auto &item : m_items) {
        item->format(ss, logger, level, event);
    }

    return ss.str();
}

//%xxx %xxx{xxx} %%
void LogFormatter::init()
{
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nstr.append(1,m_pattern[i]); 
            continue;
        }  

        int status = 0;
        std::string str;
        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
            str.push_back(m_pattern[i + 1]);
            status = 1;
        } 

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
            nstr.clear();
        }
        
        if (status == 1) {
            vec.push_back(std::make_tuple(str, "", 1));
            i++;
        }




        // size_t n = i + 1;
        // int fmt_status = 0;
        // size_t fmt_begin = 0;

        // std::string str;
        // std::string fmt;

        // while (n < m_pattern.size()) {
        //     if (isspace(m_pattern[n])) {
        //         break;
        //     }

        //     if (fmt_status == 0) {
        //         if (m_pattern[n] == '{') {
        //             str = m_pattern.substr(i + 1, n - i - 1);
        //             fmt_status = 1; // ????????????
        //             fmt_begin = n;
        //             ++n;
        //             continue;
        //         }
        //     }

        //     if (fmt_status == 1) {
        //         if (m_pattern[n] == '}') {
        //             //??????{xxx}????????????Fmt
        //             fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
        //             fmt_status = 2;
        //             break;
        //         }
        //     }
        //     ++n;
        // }
        // if (fmt_status == 0) {
        //     if (!nstr.empty()) {
        //         vec.push_back(std::make_tuple(nstr, "", 0));
        //         nstr.clear();
        //     }
        //     str = m_pattern.substr(i + 1, n - i - 1);
        //     vec.push_back(std::make_tuple(str, fmt, 1));
        //     i = n;
        // } else if (fmt_status == 1) {
        //     std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
        //     vec.push_back(std::make_tuple("<<parse error>>", fmt, 1));
        // } else if (fmt_status == 2) {
        //     if (!nstr.empty()) {
        //         vec.push_back(std::make_tuple(nstr, "", 0));
        //         nstr.clear();
        //     }
        //     vec.push_back(std::make_tuple(str, fmt, 1));
        //     i = n;
        // }
    }

    if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::unordered_map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_fmt_items = {
#define def_item(str, item_type) \
    {#str, [](const std::string &fmt) { return std::make_shared<item_type>(fmt); } },
        def_item(m, MessageFormatItem)
        def_item(p, LevelFormatItem)
        def_item(r, ElapseFormatItem)
        def_item(c, NameFormatItem)
        def_item(t, ThreadIdFormatItem)
        def_item(n, NewLineFormatItem)
        def_item(d, DateTimeFormatItem)
        def_item(f, FileNameFormatItem)
        def_item(l, LineFormatItem)
        def_item(T, TabFormatItem)
        def_item(F, FiberIdFormatItem)
        def_item(N, ThreadNameFormatItem)
#undef def_item
    };
    /* 
    %T ??????
    %F ??????id
    %m ?????????
    %p level
    %r ???????????????
    %c ????????????
    %t ??????id
    %n ????????????
    %d ??????
    %f ?????????
    %l ??????
    %N ????????? 
    */

    for (const auto &i : vec) {
        if (std::get<2>(i) == 0) {
            // string
            m_items.push_back(std::make_shared<StringFormatItem>(std::get<0>(i)));
        } else {
            auto it = s_fmt_items.find(std::get<0>(i));
            if (it == s_fmt_items.end()) {
                //????????????
                m_items.push_back(std::make_shared<StringFormatItem>("<<error fmt>>: " + std::get<0>(i)));
                m_isError = true;
            }else {
                m_items.push_back(it->second(std::get<1>(i))); 
            }
        }

        // std::cout << "(" << std::get<0>(i) << "), (" << std::get<1>(i) <<"), (" << std::get<2>(i) << ")" <<std::endl;
    }

}



}
