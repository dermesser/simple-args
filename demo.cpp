# include <iostream>
# include "args.hpp"

int main(int argc, char **argv) {
    int i;
    std::string s;
    bool b;

    Parser *p = new Parser();

    p->IntVar(&i, "integer", 42, "Value of i");
    p->StringVar(&s, "some_string_val", "abc", "Value of s");
    p->BoolVar(&b, "xybool", true, "Value of b");

    p->Parse(argc, argv);


    std::cout << i << " " << s << " " << b << std::endl;
    return 0;
}
