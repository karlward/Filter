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

/* DataStream<long> Filter::mode() { 
  DataStream<long> mode(0); // FIXME: malloc? 
  if (_values.currentSize() > 0) { 
    DataStream<long> uniqueIndex(0); 
    DataStream<long> uniqueCount(0); 
    DataStream<long>* ordered = _orderedValues(); 
    StreamItem<long>* cur = ordered->_head;
    long index = 0; 
    long seen; 
    // store the index of the first appearance of each value
    while (cur != NULL) { 
      Serial.println("mode stage 1"); 
      if ((index == 0) || (seen != cur->read())) { 
        uniqueIndex.resize(uniqueIndex.currentSize() + 1); 
        uniqueIndex.write(index); 
        seen = cur->read(); 
      } 
      index++; 
      cur = cur->_next; 
    } 
    // now use the indexes to count how many times each value appears 
    cur = uniqueIndex._head; // reusing cur
    index = 0; // reusing index
    long mostFreq;
    while (cur != NULL) { 
      Serial.println("mode stage 2"); 
      long count; 
      if (cur->_next != NULL) { 
        count = cur->_next->read() - cur->read(); // FIXME: test
        //Serial.print("count: "); 
        //Serial.println(count); 
      } 
      else { 
        count = ordered->currentSize() - cur->read(); // FIXME: test
        //Serial.print("count: "); 
        //Serial.println(count); 
      } 
      uniqueCount.resize(uniqueCount.currentSize() + 1); 
      uniqueCount.write(count);
      
      // and find the most frequent value
      if ((index == 0) || (count >= mostFreq)) { 
        mostFreq = count; 
        index++; 
      } 
      cur = cur->_next; 
    } 
    // now find the mode or modes and write them into the mode DataStream
    cur = uniqueCount._head; // reusing cur
    index = 0; // reusing cur
    while (cur != NULL) { 
      Serial.println("mode stage 3"); 
      if (cur->read() == mostFreq) { 
        Serial.print("writing into mode, index: "); 
        Serial.print(index); 
        Serial.print(" value: "); 
        Serial.println(ordered->read(uniqueIndex.read(index))); 
        mode.resize(mode.currentSize() + 1); 
        mode.write(ordered->read(uniqueIndex.read(index))); 
      }
      index++; 
      cur = cur->_next;
    } 
  } 
  return(mode); 
} */

/* 
// signal percentage, defined as mean divided by standard deviation
long Filter::signalPercentage() { 
  long sp; // signal percentage
  long sd = stdev(); // NOTE: to avoid calling mean() twice, this method  
                     //   relies on stdev() calling mean()
  if (sd == 0) { 
    sp = 100; 
  } 
  else { 
    sp = sd * 1000 / mean(); // using long rather than float for math
    sp = _longRound(sp, 10); // removing only 1 decimal place here, on purpose
  }
  return(sp); 
} */

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
  DataStream<long>* medianValues;
  medianValues = (DataStream<long>*) malloc(sizeof(DataStream<long>));
  medianValues->resize(_values.available());
  long i = 0;
  while (i < _values.available()) {
    medianValues->writeOrdered(_values.peek(i));
    i++;
  }
  
  return(medianValues);
}

