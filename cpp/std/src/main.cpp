#include "std/std.hpp"
#include "std/String.hpp"
#include "std/DerivedTask.hpp"
#include "std/Template.hpp"

int main() {
    cout<<"[BEGIN] testing..."<<endl;
    TestStringClass();

    TestDerivedTask();

    TestTemplate();
    cout<<"[END] testing..."<<endl;
}
