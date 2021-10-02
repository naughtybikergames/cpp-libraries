#ifndef NBG_YAML_NODE_HPP
#define NBG_YAML_NODE_HPP

#include <yaml-cpp/yaml.h>

#include <string>

namespace nbg {
    class YAMLNode {
        private:
            const YAML::Node _node;
        public:
            YAMLNode(const std::string &file);
            YAMLNode(const YAML::Node &node);

            bool valid() const;
            bool is_sequence() const;

            YAML::const_iterator begin() const;
            YAML::const_iterator end() const;

            template<typename T>
            T as() const {
                T result = T{};

                if (valid())
                    result = _node.as<T>();

                return result;
            }

            template<typename T>
            const YAMLNode operator[](const T &key) const {
                return YAMLNode(_node[key]);
            }
    };
}

#endif