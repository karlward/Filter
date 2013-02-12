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

class Filter {
  public:
    // constructor, create a Filter that contains up to specified number of values
    Filter(int sampleSize); 

    // put a new value into the Filter object, discarding oldest value if necessary
    void put(int value); 

    // return the mean, average of values currently in Filter object
    int mean(); 

    // return the median, middle value in ordered set of values in Filter object
    int median(); 

    // return the mode, most common value currently in Filter object
    int mode(); 

    // return the standard deviation of values currently in Filter object
    float stdev(); 

    // return the absolute maximum, largest value currently in Filter object 
    int maximum(); // absolute maximum only for now

    // return the absolute minimum, smallest value currently in Filter object 
    int minimum(); // absolute minimum only for now

    // UNIMPLEMENTED methods 
    // int get(); // return oldest value in Filter
    // bool available(); // like Serial.available()
    // lowpass(); // Q arg? filter algorithm? return type? 
    // bandpass(); // Q etc.
    // highpass(); // Q etc. 
    // totalMean(); // mean of all values ever stored, not just moving average
                    // could be an argument to mean() as well 
    // signalToNoise(); 
    // char * describe();
  private:
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
    void _distinct(); // populate _distinctValues
    int _distinctValues[]; // set of unique elements present in _values array
};

#endif

