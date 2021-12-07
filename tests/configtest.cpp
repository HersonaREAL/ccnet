#include <log.h>
#include <config.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

using namespace ccnet;

ConfigVar<int>::ptr int_val = Config::lookup("system.port", 8080, "system port");

void print_yaml(const YAML::Node &node, int level) {
    if (node.IsScalar()) {
        LOG_INFO()<< std::string(level * 3, ' ') << node.Scalar() << ", tag: " << node.Tag();
    }
    else if(node.IsNull()) {
        LOG_INFO() << std::string(level * 3, ' ')<< "NULL, tag: " << node.Tag() ;
    }
    else if(node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            LOG_INFO() << std::string(level * 3, ' ')<< it->first << ", tag: " << it->second.Tag() ;
            print_yaml(it->second, level + 1);
        }
    }
    else if(node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            LOG_INFO() << std::string(level * 3, ' ')<< i << ", tag: " << node[i].Tag() ;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/home/cc/ccnet/bin/conf/log.yml");
    // LOG_INFO() << root;
    print_yaml(root, 0);
}

void test_cfg() {
    LOG_INFO() << "-----------before-----------";
    LOG_INFO() << int_val->getVal();
    LOG_INFO() << int_val->toString();
    LOG_INFO() << "-----------after-----------";
    YAML::Node root = YAML::LoadFile("/home/cc/ccnet/bin/conf/log.yml");
    Config::loadFromYAML(root);
    LOG_INFO() << int_val->getVal();
    LOG_INFO() << int_val->toString();
}


int main(int argc, char *argv[]) {

    test_yaml();
    test_cfg();
    return 0;
}