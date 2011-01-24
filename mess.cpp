#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <tr1/memory>

class BaseArgument
{
    public:
        virtual bool isString() const
        {
            return false;
        }
        virtual bool isBoolean() const
        {
            return false;
        }
        virtual bool isDouble() const
        {
            return false;
        }
        virtual bool is(const std::type_info &typeInfo) const
        {
            return false;
        }
};

template <typename T>
class Argument : public BaseArgument
{
    public:
        Argument(T value)
        {
            value_ = value;
        }
        virtual T getValue() const
        {
            return value_;
        }
        virtual bool is(const std::type_info &typeInfo) const
        {
            return typeid(value_) == typeInfo;
        }
    private:
        T value_;
};

template <typename T>
std::tr1::shared_ptr<BaseArgument> createArgument(const T &value)
{
    return std::tr1::shared_ptr<BaseArgument>(new Argument<T>(value));
}

template <typename T>
T getValue(const BaseArgument *arg)
{
    return (dynamic_cast<const Argument<T> *>(arg))->getValue();
}

typedef std::vector<std::tr1::shared_ptr<BaseArgument> >::iterator ArgIterator;

int main()
{
    using std::tr1::shared_ptr;
    using std::vector;
    using std::string;
    
    vector<shared_ptr<BaseArgument> > vec;
    for (int i = 0; i < 1000; i++)
    {
        vec.push_back(shared_ptr<BaseArgument>(createArgument<bool>(true)));
        vec.push_back(shared_ptr<BaseArgument>(createArgument<string>("hello")));
        vec.push_back(shared_ptr<BaseArgument>(createArgument<double>(3.14159)));
    }
    
    for (ArgIterator iter = vec.begin() ; iter != vec.end(); ++iter)
    {
        BaseArgument *arg = (*iter).get();
        if (arg->is(typeid(bool)))
            std::cout << "bool arg: " << getValue<bool>(arg) << std::endl;
        else if (arg->is(typeid(string)))
            std::cout << "string arg: " << getValue<string>(arg) << std::endl;
        else if (arg->is(typeid(double)))
            std::cout << "double arg: " << getValue<double>(arg) << std::endl;
    }
    return 0;
}

