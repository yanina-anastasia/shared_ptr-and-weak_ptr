#include <iostream>

#include "shared_ptr.h"

using std::ostream;
using std::cout;
using std::endl;

void test1()
{
    shared_ptr<int> first;
    weak_ptr<int> second(first);

    first = make_shared<int>(5);
    auto t = first;

    std::cout << first.use_count();
}

int main() {
    test1();
    
}
