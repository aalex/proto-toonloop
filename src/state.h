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
#ifndef __STATE_H__
#define __STATE_H__

#include "rule.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

class State
{
    public:
        /** 
         * Called when this state is entered.
         * Arguments: none
         */
        boost::signals2::signal<void ()> state_entered_signal_;
        /** 
         * Called when this state is left.
         * Arguments: none
         */
        boost::signals2::signal<void ()> state_left_signal_;
        std::string get_name();
        void add_rule(Rule *rule);
    private:
        std::string name_;
        std::vector<Rule> rules_;
};
#endif
