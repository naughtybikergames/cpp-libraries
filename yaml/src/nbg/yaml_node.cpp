#include <nbg/yaml_node.hpp>

using namespace std;
using namespace nbg;

YAMLNode::YAMLNode(const string &file) : _node(YAML::LoadFile(file)) {}

YAMLNode::YAMLNode(const YAML::Node &node) : _node(node) {}

bool YAMLNode::valid() const {
    return _node.IsDefined() && !_node.IsNull();
}

bool YAMLNode::is_sequence() const {
    return _node.IsSequence();
}

YAML::const_iterator YAMLNode::begin() const {
    return _node.begin();
}

YAML::const_iterator YAMLNode::end() const {
    return _node.end();
}