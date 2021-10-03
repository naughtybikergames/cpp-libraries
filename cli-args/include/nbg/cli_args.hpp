#ifndef NBG_CLI_ARGS_HPP
#define NBG_CLI_ARGS_HPP

#include <nbg/iterator.hpp>

#include <string>
#include <vector>

namespace nbg {
    class cli_args {
        private:
            std::string _program;
            std::vector<std::string> _args;
        public:
            using const_iterator = nbg::basic_iterator<std::vector<std::string>>;

            const std::string EMPTY_STRING = "";

            cli_args(int argc, char **argv);

            const std::string& program() const;
            const std::string& first() const;
            const std::string& last() const;

            const std::string& operator[](int index) const;
            
            bool exist() const;
            int size() const;

            const_iterator begin() const;
            const_iterator end() const;

            std::string shift();
            std::vector<std::string> shift(int by);

            virtual std::string string() const;
            virtual char** c_str_arr();
    };
}

#endif