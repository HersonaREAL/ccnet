#include "config.h"


namespace ccnet {



// YAML map 使用前缀扁平化
static bool listAllMember(const std::string & prefix, 
                            const YAML::Node &node, 
                            std::list<std::pair<std::string, const YAML::Node>> &out)
{
    if (prefix.find_first_not_of("qwertyuiopasdfghjklzxcvbnm._0123456789") != std::string::npos) {
            LOG_ERROR() << "prefix name invail";
            return false;
    }

    out.push_back(std::make_pair(prefix, node));

    bool ret = true;
    if (node.IsMap()) {
        for (const auto & nd : node) {
            const std::string &nprefix = prefix.empty() ? nd.first.Scalar() : prefix + "." + nd.first.Scalar();
            ret = listAllMember(nprefix, nd.second, out);
        }
    }
    return ret;
}

void Config::loadFromYAML(const YAML::Node &root)
{
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    if (!listAllMember("", root, all_nodes)) {
        LOG_ERROR() << "list yaml all member error";
        return;
    }

    //改变已有配置
    for (const auto &node : all_nodes) {
        const std::string& key = node.first;

        ConfigVarBase::ptr var = lookupBase(key);

        if (var) {
            if (node.second.IsScalar()) {
                //字符串转val
                var->fromString(node.second.Scalar());
            } else {
                //复杂类型转换val
                std::stringstream ss;
                ss << node.second;
                var->fromString(ss.str());
            }
        }
    }
}

}