#pragma once

#include <algorithm>
#include <exception>
#include <memory>
#include <sstream>
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

template<class T>
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
            return boost::lexical_cast<std::string>(m_val);
        } catch(std::exception &e) {
            LOG_ERROR() << "ConfigVar::toString exception"
            << e.what() << "convert: " << typeid(m_val).name() << "to string";

        }
        return "";
    }

    bool fromString(const std::string &val) override {
        try {
            m_val = boost::lexical_cast<T>(val);
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


