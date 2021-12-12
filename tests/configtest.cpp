#include <log.h>
#include <config.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

using namespace ccnet;

ConfigVar<int>::ptr int_val = Config::lookup("system.port", 8080, "system port");
ConfigVar<float>::ptr float_val = Config::lookup("system.port", (float)8080, "system port");
ConfigVar<std::vector<int>>::ptr int_vec = Config::lookup("system.vec", std::vector<int>{1, 2, 3, 4}, "system vec");
ConfigVar<std::list<int>>::ptr int_list = Config::lookup("system.list", std::list<int>{12, 23, 45}, "system list");
ConfigVar<std::set<int>>::ptr int_set = Config::lookup("system.intset", std::set<int>{20,21,33,44}, "system int set");
ConfigVar<std::unordered_set<int>>::ptr int_uset = Config::lookup("system.intuset", std::unordered_set<int>{20,21,33,44}, "system int uset");
ConfigVar<std::set<std::string>>::ptr str_set = Config::lookup("system.strset", std::set<std::string>{"20t","21t","33t","ttt44"}, "system str set");
ConfigVar<std::unordered_set<std::string>>::ptr str_uset = Config::lookup("system.struset", std::unordered_set<std::string>{"2a0","2f1","3f3","4ss4"}, "system str uset");
ConfigVar<std::unordered_map<std::string, int>>::ptr int_umap = Config::lookup("system.intumap", std::unordered_map<std::string, int>{{"test1", 1}, {"test2", 2}, {"aaaa",6}, {"test1", 33}}, "system int umap");
ConfigVar<std::unordered_map<std::string, int>>::ptr int_map = Config::lookup("system.intmap", std::unordered_map<std::string, int>{{"test1", 1}, {"test2", 2}, {"aaaa",6}, {"test1", 33}}, "system int umap");
ConfigVar<std::unordered_map<std::string, std::string>>::ptr str_umap = Config::lookup("system.strumap", std::unordered_map<std::string, std::string>{{"abc", "cde"}, {"fff", "0xfff"}}, "system str umap");


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
#define TEST_SEQ(val, info) \
    do{ \
        LOG_INFO()<< #info ", " << val->getName() << ": ";\
        const auto &v = val->getVal(); \
        for (const auto &i : v) \
            LOG_INFO() << i; \
        LOG_INFO() << std::endl; \
    }while(0);

#define TEST_MAP(val, info) \
    do {\
        LOG_INFO()<< #info ", " << val->getName() << ": ";\
        const auto &v = val->getVal();\
        for (const auto &i : v) \
            LOG_INFO() << "{ " << i.first << ", " << i.second << " }";\
        LOG_INFO() << std::endl;\
    }while(0);

    LOG_INFO() << "-----------before-----------";
    LOG_INFO() << int_val->getVal();
    LOG_INFO() << int_val->toString();
    LOG_INFO() << std::endl;
    
    //test vec
    TEST_SEQ(int_vec, before)
    TEST_SEQ(int_list, before)
    TEST_SEQ(int_set, before)
    TEST_SEQ(int_uset, before)
    TEST_SEQ(str_set, before)
    TEST_SEQ(str_uset, before)
    TEST_MAP(int_umap, before)
    TEST_MAP(int_map, before)
    TEST_MAP(str_umap, before)
    
    YAML::Node root = YAML::LoadFile("/home/cc/ccnet/bin/conf/log.yml");
    Config::loadFromYAML(root);


    LOG_INFO() << "-----------after-----------";
    LOG_INFO() << int_val->getVal();
    LOG_INFO() << int_val->toString();
    LOG_INFO() << std::endl;

    TEST_SEQ(int_vec, after)
    TEST_SEQ(int_list, after)
    TEST_SEQ(int_set, after)
    TEST_SEQ(int_uset, after)
    TEST_SEQ(str_set, after)
    TEST_SEQ(str_uset, after)
    TEST_MAP(int_umap, after)
    TEST_MAP(int_map, after)
    TEST_MAP(str_umap, after)


#undef TEST_SEQ
#undef TEST_MAP
}

void test_cb() {
    ConfigVar<float>::ptr f = Config::lookup("test.change", (float)1.234, "test change config");
    f->addListener(0, [](const float &old_val, const float &new_val) {
        LOG_INFO() << "old val: "<< old_val << ", new val: " << new_val;
    } );
    f->addListener(1, [](const float &old_val, const float &new_val) {
        LOG_INFO() << "old val: "<< old_val << ", new val: " << new_val << "2";
    } );
    f->addListener(2, [](const float &old_val, const float &new_val) {
        LOG_INFO() << "old val: "<< old_val << ", new val: " << new_val << "3";
    } );

    LOG_INFO() << "before: " << f->getVal();
    YAML::Node root = YAML::LoadFile("/home/cc/ccnet/bin/conf/log.yml");
    Config::loadFromYAML(root);
    LOG_INFO() << "after: " << f->getVal();
}

int main(int argc, char *argv[]) {

    test_yaml();
    test_cfg();
    test_cb();
    return 0;
}