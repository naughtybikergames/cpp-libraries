#include <nbg/cli_args.hpp>

using namespace std;
using namespace nbg;

cli_args::cli_args(int argc, char **argv) {
    _program = argv[0];

    for (int i = 1; i < argc; i++)
        _args.push_back(argv[i]);
}

const string& cli_args::program() const {
    return _program;
}

const string& cli_args::first() const {
    if (exist())
        return _args.front();

    return EMPTY_STRING;
}

const string& cli_args::last() const {
    if (exist())
        return _args.back();

    return EMPTY_STRING;
}

const string& cli_args::operator[](int index) const {
    if (index >= 0 && index < _args.size())
        return _args[index];

    return EMPTY_STRING;
}

bool cli_args::exist() const {
    return !_args.empty();
}

int cli_args::size() const {
    return _args.size() + 1;
}

cli_args::const_iterator cli_args::begin() const {
    return cli_args::const_iterator(_args.begin());
}

cli_args::const_iterator cli_args::end() const {
    return cli_args::const_iterator(_args.end());
}

string cli_args::shift() {
    std::string result = EMPTY_STRING;

    if (exist()) {
        result = cli_args::first();
        _args.erase(_args.begin());
    }
    
    return result;
}

vector<string> cli_args::shift(int by) {
    vector<std::string> result;

    while (by > 0) {
        std::string arg = shift();
        if (arg.empty())
            break;

        result.push_back(arg);
        by--;
    }

    return result;
}

string cli_args::string() const {
    std::string result = "";

    for (std::string arg : *this)
        result += arg + ' ';

    result.erase(result.length() - 1);

    return result;
}

char** cli_args::c_str_arr() {
    _cstrings.clear();
    _cstrings.reserve(size());

    _cstrings.push_back(const_cast<char*>(_program.c_str()));
    for (size_t i = 0; i < _args.size(); i++)
        _cstrings.push_back(const_cast<char*>(_args[i].c_str()));

    return &_cstrings[0];
}