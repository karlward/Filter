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

#include "Arduino.h"
#include "Filter.h"

// Constructor
Filter::Filter(int sampleSize) {
  // sample size indicates how many values to store for mean, median, etc. 
  _sampleSize = sampleSize; 
  // object will store values in array (well, pointer) of specified size, plus 1
  _values = (int *) malloc(sizeof(int) * (_sampleSize+1)); 
  _valuesFirst = _sampleSize; // index to oldest value, initialize to last index
  _valuesLast = _sampleSize; // index to newest value, initialize to last index
  _valuesCount = 0; // no values stored yet
}

void Filter::put(int value) {
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

int Filter::mean() { 
  long sum = 0;
  // sum all values
  // NOTE: we're doing floating point math in long rather than using floats
  for (int i=0; i < _valuesCount; i++) { 
    sum = sum + ((long)_values[i] * 100); // multiply by 100 to do FP math
  }
  _mean = sum / (long)_valuesCount; // FIXME: is long cast necessary?
  // figure out rounding, then divide by 100 to correct floating point
  if (_mean % 100 < 50) { 
    _mean = _mean / 100; // round down
  } 
  else { 
    _mean = (_mean / 100) + 1; // round up
  }
  return((int)_mean); 
}

float Filter::stdev() { 
  // make sure we have the most recent mean calculated
  mean();

  // standard deviation calculation  
  long sum = 0; 
  for (int i=0; i < _valuesCount; i++) { 
    sum += sq(_values[i] - _mean); 
  } 
  _stdev = sqrt(sum / (float) _valuesCount); 
  
  return(_stdev); 
} 

int Filter::maximum() { 
  for (int i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] > _maximum)) { 
      _maximum = _values[i]; 
    } 
  } 
  return(_maximum); 
} 

int Filter::minimum() { 
  for (int i=0; i < _valuesCount; i++) { 
    if ((i == 0) || (_values[i] < _minimum)) { 
      _minimum = _values[i]; 
    } 
  } 
  return(_minimum); 
} 

String Filter::describe() { 
  String description = String("stored values count: "); 
  description.concat(_valuesCount); 
  description.concat(' of '); 
  description.concat(_sampleSize); 
  description.concat('\n'); 

  description.concat("values: "); 
  for (int i=0; i < _valuesCount; i++) { 
    description.concat(_values[i]); 
    description.concat(' '); 
  } 
  description.concat('\n'); 
  return(description);  
}
