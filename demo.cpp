# include <iostream>
# include "args.hpp"

int main(int argc, char **argv) {
    int i;
    std::string s;
    bool b;

    std::list<std::string> remaining;

    Parser *p = new Parser();

    p->IntVar(&i, "integer", 42, "Value of i");
    p->StringVar(&s, "some_string_val", "abc", "Value of s");
    p->BoolVar(&b, "xybool", true, "Value of b");

    p->Parse(argc, argv, &remaining);

    for ( std::string& s: remaining )
        std::cout << s << std::endl;

    std::cout << i << " " << s << " " << b << std::endl;
    return 0;
}
