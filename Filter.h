/**
 * Filter - Arduino data filtering library
 * 
 * The Filter library provides Arduino programmers with data filtering 
 * operations on a configurable number of recent values. 
 * 
 * Copyright 2012-2013 Karl Ward, Surya Mattu, Tom Igoe, and contributors
 * See the file CREDITS for contributors and external code referenced/incorporated
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

/* Version 0.6.0 */

#ifndef Filter_h
#define Filter_h 

#include "Arduino.h"

// See https://github.com/karlward/DataStream
#include "DataStream.h"

class Filter {
  public:
    // CONSTRUCTORS
    Filter(); // no-arg version; if you use this, you must call resize() yourself 

    // create a Filter that contains up to specified number of values
    Filter(long sampleSize); 

    // copy constructor
    Filter(const Filter& other);

    // assignment operator
    Filter& operator= (const Filter& other); 


    // DATA STRUCTURE METHODS 
    // the number of items currently stored in Filter
    unsigned long available() const;

    // the maximum number of items that could be stored in Filter
    unsigned long capacity() const;

    // read an item from Filter without removing it
    long peek() const;
    long peek(const long index) const;

    // set the maximum number of elements that can be stored in Filter
    void resize(long newMaxSize); 

    // put a new value into the Filter object, discarding oldest value if necessary
    void write(long value); 


    // GENERAL PURPOSE METHODS
    // return a string describing the state of the object
    String describe(); 


    // BASIC STATISTICS METHODS
    // return the absolute maximum, largest value currently in Filter object 
    long maximum(); // absolute maximum only for now

    // return the mean, average of values currently in Filter object
    long mean(); 

    // return the median, most commonly appearing value in Filter object
    long median(); 

    // return the absolute minimum, smallest value currently in Filter object 
    long minimum(); // absolute minimum only for now
/*
    // return the mode(s), the most commonly appearing value(s) in the Filter object
    DataStream<long> mode(); 

    // signal percentage, calculated from ratio of mean to standard deviation
    // WARNING: only valid if all values are positive
    long signalPercentage(); 
*/
    // return the standard deviation of values currently in Filter object
    long stdev(); 

  private:
    // private data
    long _sampleSize;
    DataStream<long> _values;

    // private methods
    long _longRound(long input, long multiplier); 
    DataStream<long>* _orderedValues(); 
};

#endif

