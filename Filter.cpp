/**
 * Filter - Arduino data filtering library
 * 
 * The Filter library provides Arduino programmers with data filtering 
 * operations via basic statistic methods (e.g. mean, median, standard 
 * deviation) for a configurable number of recent values. 
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

/* Version 0.4-alpha */ 

#include "Arduino.h"
#include "Filter.h"

// Constructor
Filter::Filter(int sampleSize) {
  _sampleSize = sampleSize; // how many values to use for mean, median, etc. 
  int _values[_sampleSize+1]; // create array of specified size, plus 1
  _valuesFirst = _sampleSize; // index to oldest value, initialize to last index
  _valuesLast = _sampleSize; // index to newest value, initialize to last index
  _valuesCount = 0; // no values stored yet
  int _medianValues[_sampleSize]; // create array of specified size, FIXME: plus 1
  _medianValuesCount = 0; // no values stored yet for median calculation 
}

void Filter::put(int value) {
  if ((_valuesFirst == _sampleSize) || (_valuesLast == _sampleSize)) { // no values yet 
    _values[0] = value; 
    Serial.println("inserting first");  
    _valuesFirst = 0; 
    _valuesLast = 0; 
    _valuesCount++; 
  } 
  else if (_valuesCount < _sampleSize) { 
    Serial.println("inserting value"); 
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
    _values[_valuesLast] = value; 
    _valuesCount++; 
  } 
  else { 
    Serial.println("overwriting a value"); 
    _values[_valuesFirst] = value; 
    _valuesFirst = (_valuesFirst + 1) % _sampleSize; 
    _valuesLast = (_valuesLast + 1) % _sampleSize; 
  }
  Serial.print("value: "); 
  Serial.println(value); 
  Serial.print("_valuesCount: "); 
  Serial.println(_valuesCount); 
  Serial.print("_sampleSize: "); 
  Serial.println(_sampleSize); 

}

int Filter::mean() { 
  long sum = 0;
  // sum all values
  // NOTE: we're doing floating point math in long rather than float
  for (int i=0; i < _valuesCount; i++) { 
    sum = sum + (_values[i] * 100); // multiply by 100 to do FP math
  }
  _mean = (long) (sum / _valuesCount); // FIXME: is long cast necessary?
  // figure out rounding, then divide by 100 to correct floating point
  if (_mean % 100 < 50) { 
    _mean = _mean / 100; // round down
  } 
  else { 
    _mean = (_mean / 100) + 1; // round up
  }
  return((int)_mean); // FIXME: cast to int? 
}

int Filter::median() { 
  // erase old readings, we only want the freshness
  _medianValuesCount = 0; // reset the counter
  // NOTE: There is no simple way to delete values within an array in C++,
  //   without using a vector or a pointer. This means _median_values 
  //   can contain old/garbage data, so do not access it directly.

  // create an ordered array of the latest values 
  for (int i=0; i < _valuesCount; i++) { 
    _orderedInsert(_values[i]); // insert into _median_values array
  } 
  
  // median is the element in the middle of the ordered list of values
  // FIXME: should we use _medianValuesCount instead of _valuesCount ?
  int midpoint = (int) _valuesCount / 2; 
  if (_valuesCount % 2 == 1) { // we have an odd number of values
    _median = _medianValues[midpoint]; 
  } 
  else { // we have an odd number of values, so get mean of midpoint pair
    _median = (_medianValues[midpoint] + _medianValues[midpoint+1]) / 2; 
  }
  return(_median); 
}

float Filter::stdev() { 
  // make sure we have the most recent mean calculated
  mean();

  // standard deviation calculation  
  long sum = 0; 
  for (int i=0; i < _valuesCount; i++) { 
    sum = sum + (_values[i] - (int) _mean)^2; 
  } 
  _stdev = sqrt(sum / (float) _valuesCount); 
  
  return(_stdev); 
} 

// NOTE: overloaded method
void Filter::_orderedInsert(int value) { 
  _orderedInsert(value, 0); // call overloaded method with position arg
}
 
// NOTE: overloaded method, and recursive too
void Filter::_orderedInsert(int value, int pos) { 
  for (int i=pos; i < _valuesCount; i++) { 
    if (_medianValuesCount < _valuesCount) { 
      if (i == _medianValuesCount) { 
        _medianValues[i] = value; 
        _medianValuesCount++; 
      }
      else if (value < _medianValues[i]) {
        _orderedInsert(_medianValues[i], i+1); 
        _medianValues[i] = value;   
      } 
      else if (value >= _medianValues[i]) { 
        _orderedInsert(value, i+1); 
      }
    }
  }
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
  for (int i=0; i > _valuesCount; i++) { 
    if ((i == 0) || (_values[i] < _minimum)) { 
      _minimum = _values[i]; 
    } 
  } 
  return(_minimum); 
} 

String Filter::describe() { 
  String description = String("count: "); 
  description.concat(_valuesCount); 
  description.concat('/'); 
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
