#pragma once
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <log.h>

namespace ccnet {

template<class From, class To>
class BaseCast
{
public:
    To operator()(const From & v) {
        return boost::lexical_cast<To>(v);
    }
};


//偏特化vector 2 str
template<class T>
class BaseCast<std::vector<T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::vector<T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const T &val : vals) {
            YAML::Node tmpNode = YAML::Load(cast2str()(val));
            node.push_back(tmpNode);
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 vec
template<class T>
class BaseCast<std::string, std::vector<T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::vector<T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        size_t sz = node.size();
        typename std::vector<T> res;

        assert(node.IsSequence());

        std::stringstream ss;
        for (size_t i = 0; i < sz; i++) {
            ss.str("");
            ss << node[i];
            res.push_back(cast2Var()(ss.str()));
        }
        return res;
    }
};

//偏特化list 2 str
template<class T>
class BaseCast<std::list<T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::list<T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const T &val : vals) {
            YAML::Node tmpNode = YAML::Load(cast2str()(val));
            node.push_back(tmpNode);
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 list 
template<class T>
class BaseCast<std::string, std::list<T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::list<T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        size_t sz = node.size();
        typename std::list<T> res;

        assert(node.IsSequence());

        std::stringstream ss;
        for (size_t i = 0; i < sz; i++) {
            ss.str("");
            ss << node[i];
            res.push_back(cast2Var()(ss.str()));
        }
        return res;
    }
};

//偏特化set 2 str
template<class T>
class BaseCast<std::set<T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::set<T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const T &val : vals) {
            YAML::Node tmpNode = YAML::Load(cast2str()(val));
            node.push_back(tmpNode);
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 set
template<class T>
class BaseCast<std::string, std::set<T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::set<T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        size_t sz = node.size();
        typename std::set<T> res;

        assert(node.IsSequence());
        std::stringstream ss;
        for (size_t i = 0; i < sz; i++) {
            ss.str("");
            ss << node[i];
            res.insert(cast2Var()(ss.str()));
        }
        return res;
    }
};

//偏特化uset 2 str
template<class T>
class BaseCast<std::unordered_set<T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::unordered_set<T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const T &val : vals) {
            YAML::Node tmpNode = YAML::Load(cast2str()(val));
            node.push_back(tmpNode);
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 uset
template<class T>
class BaseCast<std::string, std::unordered_set<T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::unordered_set<T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        size_t sz = node.size();
        typename std::unordered_set<T> res;

        assert(node.IsSequence());

        std::stringstream ss;
        for (size_t i = 0; i < sz; i++) {
            ss.str("");
            ss << node[i];
            res.insert(cast2Var()(ss.str()));
        }
        return res;
    }
};

//偏特化umap 2 str
template<class T>
class BaseCast<std::unordered_map<std::string, T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::unordered_map<std::string, T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const auto &val : vals) {
            node[val.first] = YAML::Load(cast2str()(val.second));
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 umap 
template<class T>
class BaseCast<std::string, std::unordered_map<std::string, T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::unordered_map<std::string, T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        typename std::unordered_map<std::string, T> res;
        assert(node.IsMap());

        std::stringstream kss;
        std::stringstream vss;
        for (const auto& p : node) {
            kss.str("");
            vss.str("");
            kss << p.first;
            vss << p.second;
            res[kss.str()] = cast2Var()(vss.str());
        }
        return res;
    }
};

// 偏特化map 2 str
template<class T>
class BaseCast<std::map<std::string, T>, std::string>
{
public:
    using cast2str = BaseCast<T, std::string>;
    std::string operator()(const std::map<std::string, T> & vals) {
        YAML::Node node;
        std::stringstream ss;
        for (const auto &val : vals) {
            node[val.first] = YAML::Load(cast2str()(val.second));
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 map
template<class T>
class BaseCast<std::string, std::map<std::string, T>>
{
public:
    using cast2Var = BaseCast<std::string, T>;
    std::map<std::string, T> operator()(const std::string &str) {
        YAML::Node node = YAML::Load(str);
        typename std::map<std::string, T> res;
        assert(node.IsMap());

        std::stringstream kss;
        std::stringstream vss;
        for (const auto& p : node) {
            kss.str("");
            vss.str("");
            kss << p.first;
            vss << p.second;
            res[kss.str()] = cast2Var()(vss.str());
        }
        return res;
    }
};


//偏特化logcof
template<>
class BaseCast<LogConf, std::string>
{
public:
    std::string operator()(const LogConf &vals) {
        //TODO
        return "";
    }
};

template<>
class BaseCast<std::string,  LogConf>
{
public:
    LogConf operator()(const std::string &str) {
        //TODO
        YAML::Node n = YAML::Load(str);
        LogConf res;
        if (!n["name"].IsDefined())
            return res;
        res.name = n["name"].as<std::string>();    

        res.level = n["level"].IsDefined() ? 
                    LogLevel::ToLevel(n["level"].as<std::string>()) : LogLevel::UNKNOW;

        res.formatter = n["formatter"].IsDefined() ? 
                    n["formatter"].as<std::string>() : "%d%T%N:%t%T%F%T[%p]%T[%c]%T<%f:%l>: %m%n";

        // cast appenders
        if (n["appenders"].IsDefined()) {
            //TODO
            const auto &appenders_nd = n["appenders"];
            size_t sz = appenders_nd.size();
            for (size_t i = 0; i < sz; i++) {
                // 0 -- StdoutAp, 1 -- FileAp
                const auto &apnd = appenders_nd[i];
                std::string type = apnd["type"].IsDefined() ? apnd["type"].as<std::string>() : "StdoutLogAppender";

                LogAppenderConf apcf;
                if (type == "StdoutLogAppender") {
                    apcf.type = 0;
                } else if (type == "FileLogAppender") {
                    apcf.type = 1;
                    // file not found
                    if (!apnd["file"].IsDefined())
                        continue;
                    apcf.file = apnd["file"].as<std::string>();
                } else {
                    assert(0);
                }

                // formatter 无自己定义的就用logger的                   
                if (apnd["formatter"].IsDefined()) {
                    apcf.formatter = apnd["formatter"].as<std::string>();
                }

                // level无定义使用logger的
                if (apnd["level"].IsDefined()) {
                    apcf.level = LogLevel::ToLevel(apnd["level"].as<std::string>());
                } else {
                    apcf.level = res.level;
                }
                res.appenders.push_back(apcf);
            }
        }

        return res;
    }    
};

}//ccnet namespace