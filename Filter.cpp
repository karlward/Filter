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

FilterQueue::FilterQueue(long maxSize) {
  _head = NULL; 
  _tail = NULL; 
  _maxSize = maxSize; 
  _currentSize = 0; 
} 

long FilterQueue::currentSize() { 
  return(_currentSize); 
} 

long FilterQueue::maxSize() { 
  return(_maxSize); 
} 

long FilterQueue::read() { 
  if (_currentSize > 0) { 
    return(_head->value); 
  } 
} 

void FilterQueue::write(long value) { 
  FilterElement *fe;
  fe = (FilterElement *) malloc(sizeof(FilterElement)); 
  fe->value = value; 
  
  if (_currentSize == 0) { // first item
    _head = fe; 
    _head->value = value; 
    _head->_next = NULL; // only one item, so nothing before...
    _head->_prev = NULL; // ...and nothing after
    _tail = _head; // head and tail are the same element when only one element exists
    _currentSize = 1; 
  } 
  else if (_currentSize < _maxSize) { // queue is partially populated
    //save pointer to former newest element
    FilterElement *oldTail = _tail; 
    // insert the new item at the tail
    oldTail->_next = fe; 
    _tail = fe; 
    _tail->_prev = oldTail; 
    _tail->_next = NULL; 
    _currentSize++; 
  }
  else { // queue is full
    // save pointer to oldest element
    FilterElement *oldHead = _head; 

    if (_maxSize == 1) { // special case
      _head = fe; 
      _tail = fe; 
      _head->_prev = NULL; 
      _head->_next = NULL; 
    } 
    else { 
      // save pointer to newest element
      FilterElement *oldTail = _tail; 

      if (_currentSize > 1) { 
        _head = _head->_next; // make second-oldest element the oldest element
        _head->_prev = NULL; // oldest element has no previous element 
      } 
      // insert the new item at the tail
      _tail->_next = fe; 
      _tail = fe; 
      _tail->_prev = oldTail; 
      _tail->_next = NULL; 
    } 
    // free the memory associated with oldest element
    free(oldHead); 
  }
} 
// end FilterQueue class 

/* 
 * Filter class 
 */

// Constructor
Filter::Filter(long sampleSize) {
  // sample size indicates how many values to store for mean, median, etc. 
  _sampleSize = sampleSize; 
  // object will store values in array (well, pointer) of specified size, plus 1
  _values = (long *) malloc(sizeof(long) * (_sampleSize+1)); 
  _valuesFirst = _sampleSize; // index to oldest value, initialize to last index
  _valuesLast = _sampleSize; // index to newest value, initialize to last index
  _valuesUnseenFirst = NULL; // index to oldest unseen value
  _valuesCount = 0; // no values stored yet
  _medianValues = NULL; // _medianValues is undefined until median() is called 
}

void Filter::put(long value) {
  if ((_valuesFirst == _sampleSize) || (_valuesLast == _sampleSize)) { // no values yet 
    _values[0] = value; 
    _valuesFirst = 0; 
    _valuesLast = 0; 
    _valuesCount++; 
  } 
  else if (_valuesCount < _sampleSize) { // not full of values yet
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
    _values[_valuesLast] = value; 
    _valuesCount++; 
  } 
  else { // overwriting old values now
    _values[_valuesFirst] = value; 
    _valuesFirst = (_valuesFirst + 1) % _sampleSize; 
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
  }
}

String Filter::describe() { 
  String description = String("stored values count: "); 
  description.concat(_valuesCount); 
  description.concat(" of "); 
  description.concat(_sampleSize); 
  description.concat("\n"); 

  description.concat("values: "); 
  for (long i=0; i < _valuesCount; i++) { 
    description.concat(_values[i]); 
    description.concat(' '); 
  } 
  description.concat("\n"); 
  return(description);  
}

long Filter::maximum() { 
  for (long i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] > _maximum)) { 
      _maximum = _values[i]; 
    } 
  } 
  return(_maximum); 
} 

long Filter::mean() { 
  long sum = 0;
  // sum all values
  // NOTE: we're doing floating point math in long rather than using floats
  for (long i=0; i < _valuesCount; i++) { 
    sum = sum + (_values[i] * 10); // multiply by 10 to do FP math
  }
  _mean = sum / _valuesCount;
  _mean = _longRound(_mean, 10); 
  return(_mean); 
}

long Filter::median() { 
  // erase any previous values used to determine median
  if (_medianValues != NULL) { 
    free(_medianValues); 
    _medianValues = NULL; 
    _medianValuesCount = 0; 
  } 
  // allocate memory to store ordered set of values
  _medianValues = (long *) malloc(sizeof(long) * (_sampleSize));
  // create an ordered array of the latest values 
  for (long i=0; i < _valuesCount; i++) { 
    _orderedInsert(_values[i]); 
  } 
  
  // median is the element in the middle of the ordered list of values
  long midpoint = 0; 
  if (_valuesCount > 1) { 
    midpoint = (_valuesCount - 1) / 2; 
  }
  if (_valuesCount % 2 == 1) { // we have an odd number of values
    _median = _medianValues[midpoint]; 
  } 
  else { // we have an even number of values, so get mean of midpoint pair
    // NOTE: we're doing floating point math in long rather than using floats
    _median = ((_medianValues[midpoint] + _medianValues[midpoint+1]) * 10) / 2;
    _median = _longRound(_median, 10); 
  }
  return(_median); 
}

long Filter::minimum() { 
  for (long i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] < _minimum)) { 
      _minimum = _values[i]; 
    } 
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
  for (long i=0; i < _valuesCount; i++) { 
    sum += sq(_values[i] - _mean) * 100; // i.e. a multiplier of 10 (100 is 10 squared)
  } 
  _stdev = sqrt(sum / _valuesCount);
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

void Filter::_moveOver(long start, long end) { 
  for (long i = end; i > start; i--) { 
    _medianValues[i] = _medianValues[i-1]; 
  }
} 

// non-recursive ordered insert function, CPU intensive
void Filter::_orderedInsert(long value) { 
  for (long j = 0; j <= _medianValuesCount; j++) { 
    if (j == _medianValuesCount) { 
      _medianValues[j] = value; 
      _medianValuesCount++; 
      return; // break out of the loop 
    }
    else if (value < _medianValues[j]) {
      _moveOver(j, _medianValuesCount); 
      _medianValues[j] = value; 
      _medianValuesCount++; 
      return; // break out of the loop  
    } 
  }
} 

// NOTE: recursive version of _orderedInsert removed, too memory intensive for Uno
