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
#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__
#include "state.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

class StateMachine
{
    public:
        void update();
        State* get_state(std::string name);
        State* get_current_state();
        /** 
         * Called when the state machine changes to another state.
         * Arguments: name of the state.
         */
        boost::signals2::signal<void (std::string)> state_changed_signal_;
    private:
        std::string current_state_name_;
        std::tr1::unordered_map<std::string, State*> states_; 
};
#endif
