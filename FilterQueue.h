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

#ifndef FilterQueue_h
#define FilterQueue_h 

#include "Arduino.h"

// FilterElement is a simple struct-based type
typedef struct FilterElement { 
  long value; // the value currently stored within the FilterElement
  FilterElement *_next; 
  FilterElement *_prev; 
} FilterElement; 

class FilterQueue { 
  public: 
    FilterQueue(); // no arg constructor
    FilterQueue(long maxSize); // constructor, arg is max size
    FilterQueue(const FilterQueue& other); // copy constructor
    FilterQueue& operator= (const FilterQueue& other); // assignment operator

    long currentSize() const; // current number of elements within FilterQueue object
    long maxSize() const; // largest number of elements that can be stored within object
    void orderedWrite(long value); // insert value into object in ascending order
    long read() const; // return oldest element
    long read(long index) const; // return oldest element
    void setMaxSize(long newMaxSize); // set number of elements that can be stored within object
    void write(long value); // add newest element
    friend class Filter; 

  private: 
    long _currentSize; 
    FilterElement *_head; 
    long _maxSize; 
    FilterElement *_tail; 
}; 

#endif

