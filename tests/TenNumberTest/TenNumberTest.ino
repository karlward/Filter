/** 
 * TenNumberTest: simple test code that generates random numbers, stores 
 * up to 10 of them in a Filter object, and does some basic statistic 
 * operations on them.  This sketch requires no sensors, but you do need 
 * to run it on an Arduino board. 
 *
 * Author: Karl Ward
 * License: this example code is released into the public domain
 * (note that the Filter library itself is not public domain)
 */

#include "Filter.h"

// we're going to store 10 values for analysis
Filter filteredData(10); 

void setup() { 
  // open serial port to see debugging output
  Serial.begin(9600);
} 

void loop() { 
  int value = random(0, 1024); 
  // put that sensor value into the Filter object
  filteredData.write(value); 
  Serial.print(filteredData.describe()); 

  // calculate mean, standard deviation, etc.
  int mean = filteredData.mean(); 
  int median = filteredData.median(); 
  int minimum = filteredData.minimum(); 
  int maximum = filteredData.maximum(); 
  int stdev = filteredData.stdev(); 
  int signalPercentage = filteredData.signalPercentage(); 

  // output the results to the serial port so you can see them
  Serial.print("  mean is "); 
  Serial.println(mean); 
  Serial.print("  median is "); 
  Serial.println(median); 
  Serial.print("  minimum is "); 
  Serial.println(minimum); 
  Serial.print("  maximum is "); 
  Serial.println(maximum); 
  Serial.print("  standard deviation is "); 
  Serial.println(stdev); 
  Serial.print("  signal percentage is "); 
  Serial.println(signalPercentage); 

  delay(1000); // short delay so we don't flood the serial monitor
}
