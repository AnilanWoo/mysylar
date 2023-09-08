#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdio>
#include <memory>
#include <string>
#include <iostream> 
#include <sstream>
#include <map>
#include <boost/lexical_cast.hpp>
#include <utility>
#include <yaml-cpp/yaml.h>
#include "log.h"
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <list>
#include <functional>

namespace sylar {

class Person {
public:

    std::string m_name = "Anilan";
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name = " << m_name
            << " age=" << m_age
            << " sex= " << m_sex
            << "]";
        return ss.str();
    }

    bool operator==(const Person &oth) const {
        return m_sex == oth.m_sex 
            && m_name == oth.m_name
            && m_age == oth.m_age;
    }
};

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string &name, const std::string &description = "") 
    : m_name(name)
    ,m_description(description) {

    }
    virtual ~ConfigVarBase() {} 
    virtual std::string toString() = 0;
    virtual bool fromString(const std::string &val) = 0;
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const {return m_description; }
private:
    std::string m_name;
    std::string m_description;
};

// F: from type, T: to type
template<typename F , typename T>
class LexicalCast {
public:
    T operator() (const F &v) {
        return boost::lexical_cast<T>(v);
    }
};

// 开始偏特化
// string to vector
template<typename T>
class LexicalCast<std::string, std::vector<T>> {
public:
    std::vector<T> operator() (const std::string &v) {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// vector to string
template<typename T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T> &v) {
        YAML::Node node;
        for(auto &i : v) {
            // push_back方法会将数据存储一个
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to list
template<typename T>
class LexicalCast<std::string, std::list<T>> {
public:
    std::list<T> operator() (const std::string &v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> lst;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            lst.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return lst;
    }
};

// list to string
template<typename T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T> &v) {
        YAML::Node node;
        for(auto &i : v) {
            // push_back方法会将数据存储一个
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to set
template<typename T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator() (const std::string &v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> set;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            set.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return set;
    }
};

// set to string
template<typename T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T> &v) {
        YAML::Node node;
        for(auto &i : v) {
            // push_back方法会将数据存储一个
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to unordered_set
template<typename T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator() (const std::string &v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> uset;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            uset.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return uset;
    }
};

// unordered_set to string
template<typename T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T> &v) {
        YAML::Node node;
        for(auto &i : v) {
            // push_back方法会将数据存储一个
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// // string to Person
// template<>
// class LexicalCast<std::string, Person> {
// public:
//     Person operator() (const std::string &v) {
//         YAML::Node node = YAML::Load(v);
//         Person p;
//         std::stringstream ss;

//         p.m_name = node["name"].as<std::string>();
//         p.m_age = node["age"].as<int>();
//         p.m_sex = node["sex"].as<bool>();
//         std::cout << "in" << std::endl;
//         return p;
//     }
// };

// // Person to string
// template<>
// class LexicalCast<Person, std::string> {
// public:
//     std::string operator() (const Person &p) {
//         YAML::Node node;
//         node["name"] = p.m_name;
//         node["age"] = p.m_age;
//         node["sex"] = p.m_sex;
//         std::stringstream ss;
//         ss << node;
//         return ss.str();
//     }
// };  

template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <typename T, typename FromStr = LexicalCast<std::string, T>, typename ToStr = LexicalCast<T, std::string>>
class ConfigVar :public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void(const T &old_value, const T &new_value)> on_change_cb;

    const T getValue() const { return m_val; }
    void setValue(const T &v) { 
        if (v == m_val) {
            return;
        }
        for(auto i : m_cbs) {
            i.second(m_val, v);
        } 
        m_val = v;
    }

    ConfigVar(const std::string &name
    , const T &default_value
    , const std::string& description = "") 
    : ConfigVarBase(name, description), m_val(default_value) {}

    std::string toString() override {
        try {  
            return ToStr()(m_val);
        } catch (...) {
            std::cout << "ToStrerror" << std::endl;
        }
        return "";
    }
    bool fromString(const std::string &val) override {
        try {
            setValue(FromStr()(val));
        } catch (std::exception &e) {
            std::cout << "FromStrerror" << std::endl;
        }
        return false;
    }

    void addListener(uint64_t key, on_change_cb cb) {
        m_cbs[key] = cb; 
    }

    void delListener(uint64_t key) {
        m_cbs.erase[key];
    }

    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }
private:
    T m_val;
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    // 如果没有创建一个，有的话使用有的
    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name
            , const T &default_value, const std::string& description = "") {
        auto it = m_datas.find(name);

        if (it != m_datas.end()) {
            // 说明本来就有
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);

            if (tmp) {
                std::cout << "Look up name = " << name << "exists";
                return tmp;
            } else {
                std::cout << "error fomat" << std::endl;
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
            != std::string::npos) {
                std::cout<< "error" << std::endl;
        }

        typename ConfigVar<T>::ptr v{new  ConfigVar<T>{name, default_value, description}};
        
        GetDatas()[name] = v;    

        return v;
    }
    // 只适用于查找
    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name) {
        auto it = m_datas.find(name);
        if (it == m_datas.end()) {
            // 说明没找到
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoadFromYaml(const YAML::Node &root);
    static ConfigVarBase::ptr LookupBase(const std::string &name);
private:
    static ConfigVarMap& GetDatas() {
        return m_datas;
    }
    static ConfigVarMap m_datas;
};

}

#endif
