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
  // sample size indicates how many values to store for mean, median, etc. 
  _sampleSize = sampleSize; 
  // ensure that Filter object has correct capacity to hold sampleSize elements
  _values.resize(sampleSize); 
}

// Copy constructor
Filter::Filter(const Filter& other) { 
  _sampleSize = other._sampleSize; 
  _values.resize(_sampleSize); 
  StreamItem<long>* cur = other._values._head; 
  while(cur != NULL) { 
    _values.write(cur->read()); 
    cur = cur->_next; 
  }
} 

// Operator assignment overload 
Filter& Filter::operator= (const Filter& other) {
  _sampleSize = other._sampleSize; 
  _values.resize(_sampleSize); 
  StreamItem<long>* cur = other._values._head;
  while(cur != NULL) {
    _values.write(cur->read());
    cur = cur->_next;
  }
  return(*this); 
}


// DATA STRUCTURE METHODS
void Filter::resize(long newMaxSize) { 
  _values.resize(newMaxSize); 
} 

void Filter::write(long value) {
  _values.write(value); 
}


// GENERAL PURPOSE METHODS
String Filter::describe() { 
  String description = String("stored values count: "); 
  description.concat(_values.currentSize()); 
  description.concat(" of "); 
  description.concat(_values.maxSize()); 
  description.concat("\n"); 

  // show the first ten values
  description.concat("values: "); 
  StreamItem<long>* cur; 
  cur = _values._head; 
  int i = 0; 
  while ((cur != NULL) && (i < 10)) { 
    description.concat((long) cur->read()); 
    description.concat(' '); 
    cur = cur->_next; 
    i++; 
  } 
  if (cur != NULL) { 
    description.concat('...'); 
  } 
    
  description.concat("\n"); 
  return(description);  
}


// BASIC STATISTICS METHODS
long Filter::maximum() { 
  StreamItem<long>* cur; 
  cur = _values._head; 
  long maximum = cur->read(); // slightly redundant, done to avoid comparison against undefined value
  while(cur != NULL) { 
    if (cur->read() > maximum) { 
      maximum = cur->read(); 
    } 
    cur = cur->_next; 
  }
  return(maximum); 
} 

long Filter::mean() { 
  StreamItem<long>* cur;
  cur = _values._head;
  long sum = 0; 
  while (cur != NULL) {
    sum = sum + (cur->read() * 10);
    cur = cur->_next;
  }
  long mean = sum / _values.currentSize();
  mean = _longRound(mean, 10); 
  return(mean); 
}

long Filter::median() { 
  DataStream<long>* medianValues = _orderedValues(); 
  long median; 
 
  // median is the element in the middle of the ordered list of values
  long midpoint = 0; 
  if (_values.currentSize() > 1) { 
    midpoint = (medianValues->currentSize() - 1) / 2; 
  }
  if (_values.currentSize() % 2 == 1) { // we have an odd number of values
    median = medianValues->read(midpoint); 
  } 
  else { // we have an even number of values, so get mean of midpoint pair
    // NOTE: we're doing floating point math in long rather than using floats
    median = ((medianValues->read(midpoint) + medianValues->read(midpoint+1)) * 10) / 2;
    median = _longRound(median, 10); 
  }
  return(median); 
}

long Filter::minimum() { 
  StreamItem<long>* cur;
  cur = _values._head;
  long minimum = cur->read(); // slightly redundant, done to avoid comparison against undefined value
  while(cur != NULL) {
    if (cur->read() < minimum) {
      minimum = cur->read();
    }
    cur = cur->_next;
  }
  return(minimum);
}

DataStream<long> Filter::mode() { 
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
} 

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
}

long Filter::stdev() { 
  // standard deviation calculation  
  long sum = 0; 
  StreamItem<long>* cur; 
  cur = _values._head;
  while(cur != NULL) { 
    sum += sq(cur->read() - mean()) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
    cur = cur->_next; 
  } 
  long stdev = sqrt(sum / _values.currentSize());
  stdev = _longRound(stdev, 10); // round and undo that multiplier of 10
  return(stdev); 
} 

// private methods

long Filter::_longRound(long input, long multiplier) { 
  if (input % multiplier < (multiplier/2)) { 
    input = input / multiplier; // round down 
  }
  else { 
    input = (input / multiplier) + 1; // round up
  } 
  return(input); 
}

DataStream<long>* Filter::_orderedValues() { 
  DataStream<long>* medianValues; 
  medianValues = (DataStream<long>*) malloc(sizeof(DataStream<long>));
  // FIXME: messy
  medianValues->_head = NULL;
  medianValues->_tail = NULL;
  medianValues->_maxSize = _values.currentSize();
  medianValues->_currentSize = 0;
 
  StreamItem<long>* cur = _values._head; 
  while (cur != NULL) {  
    medianValues->writeOrdered((cur->read())); 
    cur = cur->_next; 
  } 
  
  return(medianValues); 
}

