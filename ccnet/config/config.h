#pragma once

#include <algorithm>
#include <exception>
#include <memory>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <log.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

namespace ccnet {

class ConfigVarBase
{
public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string &name, const std::string& description = "")
        : m_name(name) ,
          m_description(description) 
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), [](char c) { return std::tolower(c); });
    }
    virtual ~ConfigVarBase() {}

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    // val 2 str
    virtual std::string toString() = 0;
    // str 2 val
    virtual bool fromString(const std::string &val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};



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
        for (int i = 0; i < sz; i++) {
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
        for (int i = 0; i < sz; i++) {
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
        for (int i = 0; i < sz; i++) {
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
        for (int i = 0; i < sz; i++) {
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
            node.push_back(std::make_pair(val.first, cast2str()(val.second)));
        }
        ss << node;
        return ss.str();
    }
};

//偏特化str 2 umap 
template<class T>
class BaseCast<std::string, std::unordered_set<std::string, T>>
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

//偏特化umap 2 str
// template<class T>
// class BaseCast<std::map<std::string, T>, std::string>
// {
// public:
//     using cast2str = BaseCast<T, std::string>;
//     std::string operator()(const std::map<std::string, T> & vals) {

//     }
// };

// //偏特化str 2 umap
// template<class T>
// class BaseCast<std::string, std::map<std::string, T>>
// {
// public:
//     using cast2Var = BaseCast<std::string, T>;
//     std::map<std::string, T> operator()(const std::string &str) {

//     }
// };

// 支持复杂类型转str以及str转复杂类型
template<class T, class Cast2Str = BaseCast<T, std::string>, 
                  class Cast2Var = BaseCast<std::string, T>>
class ConfigVar : public ConfigVarBase
{
public:
    using ptr = std::shared_ptr<ConfigVar>;
    ConfigVar(const std::string& name, 
                const T& default_val, 
                const std::string description = "")
            : ConfigVarBase(name, description), m_val(default_val) {}

    std::string toString() override {
        try {
            return Cast2Str()(m_val);
        } catch(std::exception &e) {
            LOG_ERROR() << "ConfigVar::toString exception"
            << e.what() << "convert: " << typeid(m_val).name() << "to string";

        }
        return "";
    }

    bool fromString(const std::string &val) override {
        try {
            m_val = Cast2Var()(val);
        } catch (std::exception &e) {
            LOG_ERROR() << "ConfigVar::fromString exception"
            << e.what() << "convert: " << "string to " << typeid(m_val).name();
            return false;
        }
        return true;
    }

    const T& getVal() const { return m_val; }
    void setVal(const T& val) { m_val = val; }

private:
    T m_val;
};



class Config 
{
public:
    using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string &name, 
                                             const T& default_val, const std::string& description = "") 
    {
        auto tmp = lookup<T>(name);
        if (tmp) {
            LOG_ERROR() << "lookup name="<<name<< "exists";
            return tmp;
        }

        //限定字符
        if (name.find_first_not_of("qwertyuiopasdfghjklzxcvbnm._0123456789") != std::string::npos) {
            LOG_ERROR() << "look up name invail";
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v = std::make_shared<ConfigVar<T>>(name, default_val, description);
        s_datas[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string &name) {
        auto it = s_datas.find(name);
        return it == s_datas.end() ? nullptr : std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static ConfigVarBase::ptr lookupBase(const std::string &name) {
        auto it = s_datas.find(name);
        return it == s_datas.end() ? nullptr : it->second;
    }

    static void loadFromYAML(const YAML::Node &node);
private:
    static ConfigVarMap s_datas;
};
}


