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

/* Version 0.6.1 */ 

#include "Arduino.h"
#include "../DataStream/DataStream.h"
#include "Filter.h"

// CONSTRUCTORS
// Constructor: no-arg version
Filter::Filter() { 
  _sampleSize = 0; 
  _values.resize(0); // if you use no-arg constructor, you must call resize() yourself
  _callback = 0;
} 

// Constructor
Filter::Filter(long sampleSize) {
  _sampleSize = sampleSize; 
  // ensure that Filter object has correct capacity to hold sampleSize elements
  _values.resize(sampleSize); 
  _callback = 0;
}

// Copy constructor
Filter::Filter(const Filter& other) { 
  _sampleSize = other.capacity();
  _values.resize(_sampleSize); 
  unsigned long otherSize = other.available();
  for (unsigned long i = 0; i < otherSize; i++) {
    write(other.peek(i));
  }
  _callback = other._callback;
} 

// Operator assignment overload 
Filter& Filter::operator= (const Filter& other) {
  _sampleSize = other.capacity(); 
  _values.resize(_sampleSize); 
  unsigned long otherSize = other.available();
  for (unsigned long i = 0; i < otherSize; i++) {
    write(other.peek(i));
  }
  _callback = other._callback;
  return(*this);
}


// DATA STRUCTURE METHODS
unsigned long Filter::available() const {
  return(_values.available());
}

unsigned long Filter::capacity() const {
  return(_values.capacity());
}

bool Filter::contains(long value) const {
  unsigned long count = available();
  for (unsigned long i=0; i<count; i++) {
    if (value == peek(i)) {
      return(true);
    }
  }
  return(false); // if we get this far, the value is not present
}

long Filter::peek() const {
  return(_values.peek());
}

long Filter::peek(const long index) const {
  return(_values.peek(index));
}

long Filter::read() {
  return(_values.read());
}

/*long Filter::read(const long index) {
  return(_values.read(index));
}*/

void Filter::resize(long newMaxSize) { 
  _values.resize(newMaxSize); 
} 

void Filter::write(long value) {
  _values.write(value);
  _runCallback(); // FIXME: is this the right place to run callback?
}


// GENERAL PURPOSE METHODS
String Filter::describe() const { 
  String description = String("stored values count: "); 
  description.concat(_values.available()); 
  description.concat(" of "); 
  description.concat(_values.capacity()); 
  description.concat("\n"); 

  // show the first ten values
  description.concat("values: "); 
  long i = 0; 
  while ((i < _values.available()) && (i < 10)) { 
    description.concat(_values.peek(i)); 
    description.concat(' '); 
    i++; 
  } 
  if (i < _values.available()) { 
    description.concat('...');
  }
 
  description.concat("\n");
  return(description);
}


// BASIC STATISTICS METHODS
long Filter::maximum() const {
  long maximum = _values.peek(0); // slightly redundant, done to avoid comparison against undefined value
  long i = 0;
  while(i < _values.available()) {
    if (_values.peek(i) > maximum) {
      maximum = _values.peek(i);
    }
    i++;
  }
  return(maximum);
}

long Filter::mean() const {
  long sum = 0; 
  long i = 0;
  while (i < _values.available()) {
    sum = sum + (_values.peek(i) * 10);
    i++;
  }
  long mean = sum / _values.available();
  mean = _longRound(mean, 10);
  return(mean);
}

long Filter::median() const { 
  DataStream<long>* medianValues = _orderedValues(); 
  long median; 
 
  // median is the element in the middle of the ordered list of values
  long midpoint = 0; 
  if (_values.available() > 1) { 
    midpoint = (medianValues->available() - 1) / 2; 
  }
  if (_values.available() % 2 == 1) { // we have an odd number of values
    median = medianValues->peek(midpoint); 
  } 
  else { // we have an even number of values, so get mean of midpoint pair
    // NOTE: we're doing floating point math in long rather than using floats
    median = ((medianValues->peek(midpoint) + medianValues->peek(midpoint+1)) * 10) / 2;
    median = _longRound(median, 10); 
  }
  return(median); 
}

long Filter::minimum() const { 
  long minimum = _values.peek(0); // slightly redundant, done to avoid comparison against undefined value
  long i = 0;
  while(i < _values.available()) {
    if (_values.peek(i) < minimum) {
      minimum = _values.peek(i);
    }
    i++;
  }
  return(minimum);
}

long Filter::stDevPopulation() const {
  return(_stDev(0));
}

long Filter::stDevSample() const {
  if (_values.available() > 1) {
    return(_stDev(1));
  } 
  else { 
    return(-1); // FIXME: this silently avoids divide by zero error, revisit
  }
}

// private methods

long Filter::_longRound(long input, long multiplier) const {
  if (input % multiplier < (multiplier/2)) {
    input = input / multiplier; // round down 
  }
  else {
    input = (input / multiplier) + 1; // round up
  }
  return(input);
}

DataStream<long>* Filter::_orderedValues() const {
  DataStream<long>* medianValues = (DataStream<long>*) malloc(sizeof(DataStream<long>));
  medianValues->begin();
  medianValues->resize(_values.available());
  long i = 0;
  while (i < _values.available()) {
    medianValues->writeOrdered(_values.peek(i));
    i++;
  }
  
  return(medianValues);
}

long Filter::_stDev(bool type) const {
  // standard deviation calculation  
  long sum = 0;
  for (long i = 0; i < _values.available(); i++) {
    sum += sq(_values.peek(i) - mean()) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
  }
  long denominator;
  if (type == 0) {
    denominator = _values.available(); // population
  } 
  else {
    denominator = _values.available() - 1; // sample
  }
  long stDev = sqrt(sum / denominator);
  stDev = _longRound(stDev, 10); // round and undo that multiplier of 10
  return(stDev); 
}

void Filter::_runCallback() {
  if (_callback != 0) {
    _callback(this);
  }
}

void Filter::attachFilter(void (*fptr)(Filter* f)) {
  _callback = fptr;
}

void filterEvent(long v) __attribute__((weak));
void filterEvent(long v) {}

