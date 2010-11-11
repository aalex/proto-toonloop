#include "property.h"
#include <string>
#include <map>
#include <iostream>

void cb(std::string &name, int value)
{
    std::cout << name << " changed to " << value << std::endl;
}

template <typename T> class PropertyHolder
{
    public:
        PropertyHolder() {};
        void add_property(const std::string &name, T value)
        {
            Property<T> *p = new Property<T>(name, value);
            properties_[name] = p;
        }
        
        void remove_property(const std::string &name)
        {
            Property<T> *p = properties_[name];
            properties_.erase(name);
            delete p;
        }

        Property<T> *get_property(const std::string &name) const
        {
            return properties_.find(name)->second;
        }
        std::map<std::string, Property<T>* > properties_;
    private:
};

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

    return 0;
}
