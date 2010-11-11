#include "property.h"
#include "propertyholder.h"
#include <string>
#include <map>
#include <iostream>

void cb(std::string &name, int value)
{
    std::cout << name << " changed to " << value << std::endl;
}


int main(int argc, char *argv[])
{
    std::string name("hello");
    Property<int> p(name, 0);
    p.value_changed_signal_.connect(&cb);
    p.set_value(8);

    PropertyHolder<int> holder = PropertyHolder<int>();
    holder.add_property("bar", 9);
    Property<int> *x = holder.get_property("bar");
    x->value_changed_signal_.connect(&cb);
    //x->register_on_changed_slot(&cb);
    x->set_value(6);
    holder.remove_property("bar");

    return 0;
}
