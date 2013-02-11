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

/* Version 0.3 */

#ifndef Filter_h
#define Filter_h 

#include "Arduino.h"

const byte FILTER_ANALOG = 1; 
const byte FILTER_DIGITAL = 2; 
// what about float v int samples, return types? 

class Filter {
  public:
    Filter(int sampleSize); // what about type? 
    void prepend(int value); // not in love with the name "prepend"
    int mean(); 
    int median(); 
    int mode(); 
    float stdev(); 
    int maximum(); // relative and absolute, or just absolute?
    int minimum(); // relative and absolute, or just absolute?
    // lowpass(); // Q arg? filter algorithm? return type? 
    // bandpass(); 
    // highpass(); 
    // totalMean(); // mean of all values ever stored, not just moving average
    // signalToNoise(); 
    // what about chaining filters? probably not essential
  private:
    //byte _type;
    int _sampleSize;
    int _values[];
    int _valuesCount;
    long _mean;
    int _median; // may not need to retain this in object
    int _mode; // may not need to retain this in object 
    float _stdev; 
    int _medianValues[]; // perhaps an ordered array of indices into _values[] would be better
    int _medianValuesCount;
    void _orderedInsert(int value);
    void _orderedInsert(int value, int pos);
    int _maximum; 
    int _minimum; 
    void _distinct(); // return distinct values with no duplicates 
    int _distinctValues[];
};

#endif

