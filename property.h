/*
 * Toonloop
 *
 * Copyright 2010 Alexandre Quessy
 * <alexandre@quessy.net>
 * http://www.toonloop.com
 *
 * Toonloop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Toonloop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the gnu general public license
 * along with Toonloop.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <string>

/**
 * Property that can be changed by user interfaces mostly for effects.
 *
 * Can hold a value of a type such as int, float, etc.
 */
template <typename T> class Property
{
    public:
        typedef boost::signals2::signal<void (std::string, T)> OnChanged;
#if 0
        typedef OnChanged::slot_type OnChangedSlotType;

        boost::signals2::connection register_on_changed_slot(const OnChangedSlotType & slot)
        {
            return value_changed_signal_.connect(slot);
        }
#endif        
        Property() :
            name_(""),
            value_(0)
        {}
        Property(const std::string &name, T value) : 
            name_(name),
            value_(value) {}

        T &get_value() const { return value_; }

        std::string &get_name() const { return name_; }

        void set_value(T value)
        {
            if (value_ != value)
            {
                value_ = value;
                value_changed_signal_(name_, value_);
            }
        }
        OnChanged value_changed_signal_;
    private:
        //boost::signals2::signal<void (std::string, T)> value_changed_signal_;
        std::string name_;
        T value_;
};

