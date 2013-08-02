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

#include "Arduino.h"
#include "DataStream.h"
#include "Filter.h"

// CONSTRUCTORS
// Constructor: no-arg version
Filter::Filter() { 
  _sampleSize = 0; 
  _values.resize(0); // if you use no-arg constructor, you must call resize() yourself
} 

// Constructor
Filter::Filter(long sampleSize) {
  _sampleSize = sampleSize; 
  // ensure that Filter object has correct capacity to hold sampleSize elements
  _values.resize(sampleSize); 
}

// Copy constructor
Filter::Filter(const Filter& other) { 
  _sampleSize = other.capacity();
  _values.resize(_sampleSize); 
  unsigned long otherSize = other.available();
  for (unsigned long i = 0; i < otherSize; i++) {
    write(other.peek(i));
  }
} 

// Operator assignment overload 
Filter& Filter::operator= (const Filter& other) {
  _sampleSize = other.capacity(); 
  _values.resize(_sampleSize); 
  unsigned long otherSize = other.available();
  for (unsigned long i = 0; i < otherSize; i++) {
    write(other.peek(i));
  }
  return(*this);
}


// DATA STRUCTURE METHODS
unsigned long Filter::available() const {
  return(_values.available());
}

unsigned long Filter::capacity() const {
  return(_values.capacity());
}

long Filter::peek() const {
  return(_values.peek());
}

long Filter::peek(const long index) const {
  return(_values.peek(index));
}

void Filter::resize(long newMaxSize) { 
  _values.resize(newMaxSize); 
} 

void Filter::write(long value) {
  _values.write(value); 
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

DataStream<long>* Filter::mode() const { // FIXME: should this return a pointer? 
  DataStream<long>* mode = (DataStream<long>*) malloc(sizeof(DataStream<long>));
  mode->begin();
  if (_values.available() > 0) {
    DataStream<long> uniqueIndex = DataStream<long>(0);
    DataStream<long> uniqueCount = DataStream<long>(0);
    DataStream<long>* ordered = _orderedValues();

    long seen;
    for (long i = 0; i < ordered->available(); i++) {
      if (seen != ordered->peek(i)) { // FIXME: NULL comparison
        uniqueIndex.resize(uniqueIndex.capacity() + 1);
        uniqueIndex.write(i);
        seen = ordered->peek(i);
      }
    }

    // now use the indexes to count how many times each value appears 
    long mostFreq;
    for (long i = 0; i < uniqueIndex.available(); i++) {
      long count;
      if (i <= (uniqueIndex.available() - 2)) {
        count = uniqueIndex.peek(i + 1) - uniqueIndex.peek(i); // FIXME: NULL comparison? special cases?
      }
      else { 
        count = ordered->available() - uniqueIndex.peek(i);
      }
      uniqueCount.resize(uniqueCount.capacity() + 1);
      uniqueCount.write(count);

      if ((i == 0) || (count < mostFreq)) {
        mostFreq = count;
      }
    }

    // now find the mode or modes and write them into the mode DataStream
    for (long i = 0; i < uniqueCount.available(); i++) {
      if (uniqueCount.peek(i) == mostFreq) {
        mode->resize(mode->capacity() + 1);
        mode->write(ordered->peek(uniqueIndex.peek(i)));
      }
    }
    ordered->flush();
    free(ordered);
  }
  return(mode); 
}

// FIXME: stdev() vs stdevp() etc? 
long Filter::stdev() const {
  // standard deviation calculation  
  long sum = 0;
  for (long i = 0; i < _values.available(); i++) {
    sum += sq(_values.peek(i) - mean()) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
  }
  long stdev = sqrt(sum / _values.available());
  stdev = _longRound(stdev, 10); // round and undo that multiplier of 10
  return(stdev); 
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

