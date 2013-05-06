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
#include "Filter.h"
#include "FilterQueue.h"

// Constructor
Filter::Filter(long sampleSize) {
  // sample size indicates how many values to store for mean, median, etc. 
  _sampleSize = sampleSize; 
  _values.setMaxSize(sampleSize); // this is where the Filter object stores values
}

void Filter::write(long value) {
  _values.write(value); 
}

String Filter::describe() { 
  String description = String("stored values count: "); 
  description.concat(_values.currentSize()); 
  description.concat(" of "); 
  description.concat(_values.maxSize()); 
  description.concat("\n"); 

  description.concat("values: "); 
  FilterElement * cur; 
  cur = _values._head; 
  while (cur != NULL) { 
    description.concat((long) cur->value); 
    description.concat(' '); 
    cur = cur->_next; 
  } 
    
  description.concat("\n"); 
  return(description);  
}

long Filter::maximum() { 
  FilterElement *cur; 
  cur = _values._head; 
  _maximum = cur->value; // slightly redundant, done to avoid comparison against undefined value
  while(cur != NULL) { 
    if (cur->value > _maximum) { 
      _maximum = cur->value; 
    } 
    cur = cur->_next; 
  }
  return(_maximum); 
} 

long Filter::mean() { 
  FilterElement *cur;
  cur = _values._head;
  long sum = 0; 
  while (cur != NULL) {
    sum = sum + (cur->value * 10);
    cur = cur->_next;
  }
  _mean = sum / _values.currentSize();
  _mean = _longRound(_mean, 10); 
  return(_mean); 
}

long Filter::median() { 
  FilterQueue _medianValues; 
  _medianValues.setMaxSize(_values.currentSize()); // allocate memory to store ordered set of values
 
  FilterElement *cur = _values._head; 
  while (cur != NULL) {  
    _medianValues.orderedWrite((cur->value)); 
    cur = cur->_next; 
  } 
  
  // median is the element in the middle of the ordered list of values
  long midpoint = 0; 
  if (_values.currentSize() > 1) { 
    midpoint = (_medianValues.currentSize() - 1) / 2; 
  }
  if (_values.currentSize() % 2 == 1) { // we have an odd number of values
    _median = _medianValues.read(midpoint); 
  } 
  else { // we have an even number of values, so get mean of midpoint pair
    // NOTE: we're doing floating point math in long rather than using floats
    _median = ((_medianValues.read(midpoint) + _medianValues.read(midpoint+1)) * 10) / 2;
    _median = _longRound(_median, 10); 
  }
  return(_median); 
}

long Filter::minimum() { 
  FilterElement *cur;
  cur = _values._head;
  _minimum = cur->value; // slightly redundant, done to avoid comparison against undefined value
  while(cur != NULL) {
    if (cur->value < _minimum) {
      _minimum = cur->value;
    }
    cur = cur->_next;
  }
  return(_minimum);
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
    sp = sd * 1000 / _mean; // using long rather than float for math
    sp = _longRound(sp, 10); // removing only 1 decimal place here, on purpose
  }
  return(sp); 
}

long Filter::stdev() { 
  // make sure we have the most recent mean calculated
  mean();

  // standard deviation calculation  
  long sum = 0; 
  FilterElement *cur; 
  cur = _values._head;
  while(cur != NULL) { 
    sum += sq(cur->value - _mean) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
    cur = cur->_next; 
  } 
  _stdev = sqrt(sum / _values.currentSize());
  _stdev = _longRound(_stdev, 10); // round and undo that multiplier of 10
  return(_stdev); 
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
