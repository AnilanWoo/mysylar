#include "../src/config.h"
#include "../src/log.h"
#include "../src/util.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <sstream>
#include <stdio.h>


// sylar::ConfigVar<int>::ptr i_instance  = 
//     sylar::Config::Lookup("system.port", (int)8080, "system port");

// sylar::ConfigVar<float>::ptr i2_instance  = 
//     sylar::Config::Lookup("system.port", (float)22.11, "system port");


// sylar::ConfigVar<float>::ptr f_instance  = 
//     sylar::Config::Lookup("system.value", (float)10.45444, "system value");

// sylar::ConfigVar<std::vector<int>>::ptr iv_instance  = 
//     sylar::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system value");

// sylar::ConfigVar<std::list<int>>::ptr il_instance = 
//     sylar::Config::Lookup("system.int_list", std::list<int>{1, 2, 3}, "system value");

// sylar::ConfigVar<std::set<int>>::ptr is_instance = 
//     sylar::Config::Lookup("system.int_set", std::set<int>{1, 2, 3}, "system value");

// sylar::ConfigVar<std::unordered_set<int>>::ptr ius_instance = 
//     sylar::Config::Lookup("system.int_uset", std::unordered_set<int>{1, 2, 3}, "system value");


// void print_yaml(const YAML::Node& node, int level) {
//     if (node.IsScalar()) {
//         // 如果是标量，就直接输出
//         std::cout <<  std::string(4 * level, ' ') << node.Scalar() << " - " << node.Type() << std::endl;
//     } else if (node.IsNull()) {
//         std::cout << std::string(4 * level, ' ') << "NULL" << std::endl;
//     } else if (node.IsMap()) {
//         for (auto it = node.begin(); it != node.end(); ++it) {
//             std::cout << std::string((4 * level), ' ') << it->first.Scalar() << std::endl;
//             print_yaml(it->second, level + 1);
//         }
//     } else if (node.IsSequence()) {
//         // 如果node的类型是sequence，则node.size()是sequence的长度
//         for (size_t i = 0; i < node.size(); i++) {
//             // std::cout << std::string(4 * level, ' ') << i << " - " << node[i].Type() << " - " << level << std::endl;
//             print_yaml(node[i], level + 1);
//         }
//     }
// }

// void test_yaml() {
//     YAML::Node root = YAML::LoadFile("/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/C++开发/sylar/mysylar/bin/conf/log.yml");
//     print_yaml(root, 0);
// }

// void test_config() {

// #define XX(g_value, name, prefix) \
//     { \
//         auto v = g_value->getValue(); \
//         for(auto i : v) { \
//             std::cout << #prefix << " " << #name << " " << i << std::endl; \
//         } \
//     }

//     XX(il_instance, int_list, before)
//     // XX(is_instance, int_set, before);
//     // XX(ius_instance, int_uset, before);
    // is_instance->addListener(10, [](const std::set<int> &old_vlaue, const std::set<int> &new_value){
    //     std::cout << "change" << std::endl;
    // });
    // il_instance->addListener(10, [](const std::list<int> &old_vlaue, const std::list<int> &new_value){
    //     std::cout << "change" << std::endl;
    // });
 
//     std::cout << "--" << std::endl;
//     YAML::Node root = YAML::LoadFile("/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/C++开发/sylar/mysylar/bin/conf/log.yml");
//     sylar::Config::LoadFromYaml(root);

//     XX(il_instance, int_list,after)
//     // XX(is_instance, int_set, after);
//     // XX(ius_instance, int_uset, after);

// #undef XX
    
// }

// class Person {
// public:

//     std::string m_name = "Anilan";
//     int m_age = 0;
//     bool m_sex = 0;

//     std::string toString() const {
//         std::stringstream ss;
//         ss << "[Person name = " << m_name
//             << " age=" << m_age
//             << " sex= " << m_sex
//             << "]";
//         return ss.str();
//     }
// };

// sylar::ConfigVar<sylar::Person>::ptr p_instance = 
//     sylar::Config::Lookup("class.person", sylar::Person{}, "class person");

// void test_class() {
//     sylar::Person p = p_instance->getValue();
//     std::cout << "before " <<"name: " << p.m_name << " age: " << p.m_age << " sex: " << p.m_sex << std::endl;
//     // XX(p_instance, person, before);
//     YAML::Node root = YAML::LoadFile("/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/C++开发/sylar/mysylar/bin/conf/log.yml");
//     sylar::Config::LoadFromYaml(root);
//     p = p_instance->getValue();
//     std::cout << "name: " << p.m_name << " age: " << p.m_age << " sex: " << p.m_sex << std::endl;
//     // std::cout << "after" << p_instance->getValue() << std::endl;
// }
void testlog() {
    YAML::Node root = YAML::LoadFile("/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/C++开发/sylar/mysylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);

}
int main() {
    // sylar::Config::ConfigVarMap sylar::Config::m_datas.clear();
    // sylar::ConfigVar<int>::ptr i_instance  = 
    // sylar::Config::Lookup("system.port", (int)8080, "system port");
    // testlog();
    // sylar::ConfigVar<int> t = sylar::ConfigVar<int>{"1", 0};
    // test_config();
    // test_class();
    // test_yaml();
    // YAML::Node node;
    // node["se"].push_back(std::vector<int>{1, 2, 3});
    // node["23"].push_back(std::vector<int>{4, 5, 6});

    // std::stringstream ss;
    // ss << node;
    // std::cout << ss.str() << std::endl;
}