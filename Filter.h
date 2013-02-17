/**
 * Filter - Arduino data filtering library
 * 
 * The Filter library provides Arduino programmers with data filtering 
 * operations on a configurable number of recent values. 
 * 
 * Copyright 2012-2013 Karl Ward
 * See the file CREDITS for details on external code referenced/incorporated
 * See the file COPYING for details on software licensing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Version 0.4 */

#ifndef Filter_h
#define Filter_h 

#include "Arduino.h"

class Filter {
  public:
    // constructor, create a Filter that contains up to specified number of values
    Filter(int sampleSize); 

    // put a new value into the Filter object, discarding oldest value if necessary
    void put(int value); 

    // return a string describing the state of the object
    String describe(); 

    // return the absolute maximum, largest value currently in Filter object 
    int maximum(); // absolute maximum only for now

    // return the mean, average of values currently in Filter object
    int mean(); 

    // return the absolute minimum, smallest value currently in Filter object 
    int minimum(); // absolute minimum only for now

    // return the standard deviation of values currently in Filter object
    float stdev(); 

  private:
    int _maximum; 
    long _mean;
    int _minimum; 
    int _sampleSize;
    float _stdev; 
    int *_values;
    int _valuesCount;
    int _valuesFirst;
    int _valuesLast;
};

#endif

