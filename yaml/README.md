# yaml
Wrapper for the yaml-cpp library's YAML::Node. Provides extra functionality such as:

- checking if node is valid
- when converting a node to another data type, if node is not valid, return default value (rather than an exception)

This extra functionality makes code handling a yaml file more maintainable for the caller.

## Dependencies
- yaml-cpp