#ifndef __PROPERTYHOLDER_H__
#define __PROPERTYHOLDER_H__
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

#include "property.h"
/**
 * Holds many properties identified by their name.
 */
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
#endif
