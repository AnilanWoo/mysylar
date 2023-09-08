// #include "config.h"
// #include <list>
// #include <utility>
// namespace sylar {

// Config::ConfigVarMap Config::m_datas;

// ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
//     auto it = m_datas.find(name);
//     return it == m_datas.end() ? nullptr : it->second; 
// }

// static void ListAllMember(const std::string prefix,
//                             const YAML::Node &node,
//                             std::list<std::pair<std::string, const YAML::Node>> &output) {
//     if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
//             != std::string::npos) {
//                 std::cout << "error" << std::endl;
//                 return;
//             }
//     output.push_back(std::make_pair(prefix, node));
//     if (node.IsMap()) {
//         for (auto it = node.begin(); it != node.end(); ++it) {
//             ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
//         }
//     }
// }

// void Config::LoadFromYaml(const YAML::Node &root) {
//     std::cout << "1" << std::endl;
//     std::list<std::pair<std::string, const YAML::Node> > all_nodes;
//     ListAllMember("", root, all_nodes);
    
//     for(auto &i : all_nodes) {
//         std::string key = i.first;
//         if (key.empty()) continue;
//         ConfigVarBase::ptr var = LookupBase(key);
//         if (var) {
//             if (i.second.IsScalar()) {
//                 var->fromString(i.second.Scalar());
//             } else {
//                 std::stringstream ss;
//                 ss << i.second;
//                 var->fromString(ss.str());
//             }
//         }
//     }
// }
// }
#include "config.h"
#include <list>
#include <utility>
namespace sylar {

Config::ConfigVarMap Config::m_datas;

ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
    auto it = m_datas.find(name);
    return it == m_datas.end() ? nullptr : it->second; 
} 

static void ListAllMember(const std::string prefix,
                            const YAML::Node &node,
                            std::list<std::pair<std::string, const YAML::Node>> &output) {
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
            != std::string::npos) {
                std::cout << "error" << std::endl;
                return;
            }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node &root) {
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMember("", root, all_nodes);
    
    for(auto &i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) continue;

        ConfigVarBase::ptr var = LookupBase(key);
        if (var) {
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}


}