#pragma once

#include <algorithm>
#include <bits/stdint-uintn.h>
#include <cstddef>
#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <functional>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <log.h>
#include "config_template.h"

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
    virtual std::string getTypeName() const = 0;
protected:
    std::string m_name;
    std::string m_description;
};


// 支持复杂类型转str以及str转复杂类型
template<class T, class Cast2Str = BaseCast<T, std::string>, 
                  class Cast2Var = BaseCast<std::string, T>>
class ConfigVar : public ConfigVarBase
{
public:
    using ptr = std::shared_ptr<ConfigVar>;
    using cb = std::function<void(const T& old_val, const T& new_val)>;
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
            // 触发回调
            setVal(Cast2Var()(val));
        } catch (std::exception &e) {
            LOG_ERROR() << "ConfigVar::fromString exception"
            << e.what() << "convert: " << "string to " << typeid(m_val).name();
            return false;
        }
        return true;
    }

    std::string getTypeName() const override {
        return typeid(T).name();
    }

    const T& getVal() const { return m_val; }
    void setVal(const T& val) { 
        if (m_val == val) {
            return;
        }    
        for (auto &p : m_cbs) {
            p.second(m_val, val);
        }
        m_val = val; 
    }

    //观察者模式， 监视配置变更
    void addListener(uint64_t key, cb callback)
    {
        m_cbs[key] = callback;
    }

    void deleteListener(uint64_t key)
    {
        m_cbs.erase(key);
    }

    void clearListener()
    {
        m_cbs.clear();
    }


private:
    T m_val;
    std::map<uint64_t, cb> m_cbs;
};



class Config 
{
public:
    using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string &name, 
                                             const T& default_val, const std::string& description = "") 
    {
        auto it = s_datas().find(name);
        if (it != s_datas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp) {
                // 存在且类型相同
                LOG_INFO() << "lookup name = " << name << "exists";
                return tmp;
            } else {
                // 存在且类型不同，报错
                LOG_ERROR() << "lookup name = " << name << " exists, but lookup type: " << typeid(T).name() 
                                                        << ", real type: " << it->second->getTypeName(); 
                return nullptr;
            }
        }

        //限定字符
        if (name.find_first_not_of("qwertyuiopasdfghjklzxcvbnm._0123456789") != std::string::npos) {
            LOG_ERROR() << "look up name invail";
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v = std::make_shared<ConfigVar<T>>(name, default_val, description);
        s_datas()[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string &name) {
        auto it = s_datas().find(name);
        return it == s_datas().end() ? nullptr : std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static ConfigVarBase::ptr lookupBase(const std::string &name) {
        auto it = s_datas().find(name);
        return it == s_datas().end() ? nullptr : it->second;
    }

    static void loadFromYAML(const YAML::Node &node);

//防止静态变量未初始化直接被使用 
    static ConfigVarMap& s_datas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }
private:
};





}


