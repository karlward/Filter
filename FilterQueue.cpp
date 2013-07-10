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
#include "FilterQueue.h"

FilterQueue::FilterQueue() { 
  _head = NULL; 
  _tail = NULL; 
  _maxSize = 0; 
  _currentSize = 0; 
}

FilterQueue::FilterQueue(long maxSize) {
  _head = NULL; 
  _tail = NULL; 
  _maxSize = maxSize; 
  _currentSize = 0; 
} 

// copy constructor
FilterQueue::FilterQueue(const FilterQueue& other) {
  _head = NULL; 
  _tail = NULL; 
  _currentSize = 0; 
  _maxSize = other.maxSize();
  FilterElement * cur = other._head;
  while(cur != NULL) {
    write(cur->value);
    cur = cur->_next;
  }
} 

// assignment operator
FilterQueue& FilterQueue::operator= (const FilterQueue& other) {
  _head = NULL; 
  _tail = NULL; 
  _currentSize = 0; 
  _maxSize = other.maxSize(); 
  FilterElement * cur = other._head;
  while(cur != NULL) {
    write(cur->value);
    cur = cur->_next;
  }
  return(*this); 
}


long FilterQueue::currentSize() const { 
  return(_currentSize); 
} 

long FilterQueue::maxSize() const { 
  return(_maxSize); 
} 

void FilterQueue::orderedWrite(long value) { 
  FilterElement *fe;
  fe = (FilterElement *) malloc(sizeof(FilterElement)); 
  fe->value = value; 

  if (_head == NULL) { // first element 
    _head = fe;
    _head->_next = NULL; 
    _head->_prev = NULL; 
    _tail = _head; 
    _currentSize++; 
  } 
  else { 
    FilterElement *cur = _head; 
    while (cur != NULL) { 
      if (value < cur->value) { 
        if (cur == _head) { // special case, reset _head
          fe->_prev = NULL;
          fe->_next = _head; 
          _head->_prev = fe; 
          _head = fe; 
        } 
        else { 
          fe->_prev = cur->_prev; 
          fe->_next = cur; 
          cur->_prev->_next = fe; 
          cur->_prev = fe; 
        } 
        _currentSize++; 
        cur = NULL; // break out of the method
      } 
      else if (cur == _tail) { //special case, reset _tail
        fe->_next = NULL;
        fe->_prev = _tail; 
        _tail->_next = fe; 
        _tail = fe; 
        _currentSize++; 
        cur = NULL;; // break out of the method
      } 
      else { 
        cur = cur->_next; 
      } 
    }
  }

} 

// read the oldest value (the head element)
long FilterQueue::read() const { 
  if (_currentSize > 0) { 
    return(_head->value); 
  } 
}

// read the value at a specific index
long FilterQueue::read(long index) const { // overloaded method
  FilterElement *cur = _head; 
  long i = 0;
  while (cur != NULL) { 
    if (i == index) { 
      return(cur->value); 
    } 
    cur = cur->_next; 
    i++;
  }
}  

void FilterQueue::setMaxSize(long newMaxSize) { // FIXME: shrinking untested
  _maxSize = newMaxSize; 
  if (_currentSize > _maxSize) { // this means we have to shrink _values
    FilterElement *forward = _head;
    while ((forward != NULL) && (_currentSize > _maxSize)) { 
      FilterElement *old = forward; 
      forward = forward->_next; 
      free(old); 
      _currentSize--; 
    }
    _head->_prev = NULL; // set the new head
  }
} 

void FilterQueue::write(long value) { 
  FilterElement *fe;
  fe = (FilterElement *) malloc(sizeof(FilterElement)); 
  fe->value = value; 
  
  if (_currentSize == 0) { // first item
    _head = fe; 
    //_head->value = value; 
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

