# include <map>
# include <string>
# include <cstdlib>
# include <tuple>
# include <sstream>

/* Parsing argument classes */
class TypedArg {
    public:
        virtual bool Parse(const std::string& name, const std::string& val) = 0;
        virtual std::string GetDefault() = 0;
};

class IntArg : public TypedArg {
    public:
        IntArg(int *argval, int def)
            : value(argval), default_val(def) { *value = def; }
        IntArg() { value = new int(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string&, const std::string& val) {
            int matched = std::sscanf(val.c_str(), "%d", value);
            return matched == 1;
        }

        int *value;
        int default_val;
};

class UintArg : public TypedArg {
    public:
        UintArg(unsigned int *argval, unsigned int def)
            : value(argval), default_val(def) { *value = def; }
        UintArg() { value = new unsigned int(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string&, const std::string& val) {
            int matched = std::sscanf(val.c_str(), "%u", value);
            return matched == 1;
        }

        unsigned int *value;
        unsigned int default_val;
};

class LonglongArg : public TypedArg {
    public:
        LonglongArg(long long *argval, long long def)
            : value(argval), default_val(def) { *value = def; }
        LonglongArg() { value = new long long(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string&, const std::string& val) {
            int matched = std::sscanf(val.c_str(), "%lld", value);
            return matched == 1;
        }

        long long *value;
        long long default_val;
};

class StringArg : public TypedArg {
    public:
        StringArg(std::string *argval, const std::string& def)
            : value(argval), default_val(def) { *value = def; }
        StringArg() { value = new std::string(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string&, const std::string& val) {
            *value = val;
            return true;
        }

        std::string *value;
        std::string default_val;
};

class FloatArg : public TypedArg {
    public:
        FloatArg(float *argval, float def)
            : value(argval), default_val(def) { *value = def; }
        FloatArg() { value = new float(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string&, const std::string& val) {
            int matched = std::sscanf(val.c_str(), "%f", value);
            return matched == 1;
        }

        float *value;
        float default_val;
};

class BoolArg : public TypedArg {
    public:
        BoolArg(bool *argval, bool def)
            : value(argval), default_val(def) { *value = def; }
        BoolArg() { value = new bool(); }

        std::string GetDefault() {
            std::ostringstream oss;
            oss << default_val;
            return oss.str();
        }

        bool Parse(const std::string& name, const std::string& val) {
            if ( val == "false" || val == "0" || val == "f" || name.substr(0, 2) == "no" )
                *value = false;
            else if ( val == "true" || val == "1" || val == "t" || val == "" )
                *value = true;
            else
                return false;
            return true;
        }

        bool *value;
        bool default_val;
};

class Parser {
    public:
        void IntVar(int *dest, const std::string& name, int default_value, const std::string& desc) {
            IntArg *arg = new IntArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }
        void UintVar(unsigned int *dest, const std::string& name, unsigned int default_value, const std::string& desc) {
            UintArg *arg = new UintArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }
        void LonglongVar(long long *dest, const std::string& name, long long default_value, const std::string& desc) {
            LonglongArg *arg = new LonglongArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }
        void StringVar(std::string *dest, const std::string& name, const std::string& default_value, const std::string& desc) {
            StringArg *arg = new StringArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }
        void FloatVar(float *dest, const std::string& name, float default_value, const std::string& desc) {
            FloatArg *arg = new FloatArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }
        void BoolVar(bool *dest, const std::string& name, bool default_value, const std::string& desc) {
            BoolArg *arg = new BoolArg(dest, default_value);

            descriptions[name] = desc;
            registered[name] = arg;
        }

        bool Parse(int argc, char **argv) {

            for ( int i = 1; i < argc;) {
                std::string current_name, current_value;

                if ( argv[i][0] == '-' && argv[i][1] != '-' ) {
                    current_name = std::string(argv[i]+1);
                } else if ( argv[i][0] == '-' && argv[i][1] == '-' ) {
                    current_name = std::string(argv[i]+2);
                }

                if ( i < (argc-1) && argv[i+1][0] == '-' ) {
                    current_value = std::string("");
                    i++;
                } else if ( i < (argc-1) ) {
                    current_value = std::string(argv[i+1]);
                    i += 2;
                } else if ( i >= (argc-1) ) {
                    current_value = std::string("");
                    i++;
                }

                // name and value parsed here
                args[current_name] = current_value;
            }

            // Search for --help or -h and print summary of arguments and return.
            if ( showHelpIfNeeded() )
                return false;

            // Parse actual arguments
            for ( auto &arg : args ) {
                TypedArg *ta = registered[arg.first];
                if ( ta != nullptr ) {
                    ta->Parse(arg.first, arg.second);
                    continue;
                }

                if ( arg.first.size() > 2 ) {
                    // Maybe the argument is prefixed with "no"
                    ta = registered[arg.first.substr(2, arg.first.size())];

                    if ( ta != nullptr ) {
                        ta->Parse(arg.first, arg.second);
                        continue;
                    }
                }
            }

            return true;
        }

        bool showHelpIfNeeded() {
            if ( args.find("help") != args.end() || args.find("h") != args.end() ) {
                for ( auto& arg : registered ) {
                    std::cout << "--" << arg.first << "\t\t" << descriptions[arg.first];
                    std::cout << " [default=" << arg.second->GetDefault() << "]\n";
                }
                return true;
            } else
                return false;
        }
    private:
        std::map<std::string, TypedArg*> registered;
        std::map<std::string, std::string> args;
        std::map<std::string, std::string> descriptions;
};

