// See https://github.com/mmurdoch/arduinounit
#include <ArduinoUnit.h>
// See https://github.com/karlward/DataStream
#include <DataStream.h>
// See http://github.com/karlward/Filter
#include <Filter.h>

test(instantiate) {
  Filter f0 = Filter();
  assertEqual(0, f0.available());
  Filter f1 = Filter(1);
  assertEqual(0, f1.available());
  Filter f2 = Filter(2);
  assertEqual(0, f2.available());
}

test(write) {
  Filter f0 = Filter(10);
  f0.write(145);
  assertEqual(1, f0.available());
  assertEqual(145, f0.peek());
  for (int i = 0; i < 100; i++) { 
    f0.write(i); 
  }

  for (int j = 0; j < 10; j++) {
    assertEqual(10, f0.available());
    assertEqual((j + 90), f0.peek(j));
  }
}

test(available) {
  Filter f0 = Filter(1);
  assertEqual(0, f0.available());
  assertEqual(0, f0.available());
  f0.write(18);
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());
  f0.write(1790);
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());
  f0.write(17648);
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());
  f0.write(24);
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());
  f0.write(2478);
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());
  assertEqual(1, f0.available());

  Filter f1 = Filter(2);
  assertEqual(0, f1.available());
  assertEqual(0, f1.available());
  f1.write(18);
  assertEqual(1, f1.available());
  assertEqual(1, f1.available());
  f1.write(1790);
  assertEqual(2, f1.available());
  assertEqual(2, f1.available());
  f1.write(17648);
  assertEqual(2, f1.available());
  assertEqual(2, f1.available());
  f1.write(24);
  assertEqual(2, f1.available());
  assertEqual(2, f1.available());
  f1.write(2478);
  assertEqual(2, f1.available());
  assertEqual(2, f1.available());
  assertEqual(2, f1.available());
  
  Filter f2 = Filter(3);
  assertEqual(0, f2.available());
  assertEqual(0, f2.available());
  f2.write(18);
  assertEqual(1, f2.available());
  assertEqual(1, f2.available());
  f2.write(1790);
  assertEqual(2, f2.available());
  assertEqual(2, f2.available());
  f2.write(17648);
  assertEqual(3, f2.available());
  assertEqual(3, f2.available());
  f2.write(24);
  assertEqual(3, f2.available());
  assertEqual(3, f2.available());
  f2.write(2478);
  assertEqual(3, f2.available());
  assertEqual(3, f2.available());
  assertEqual(3, f2.available());

  Filter f3 = Filter(10);
  for (int i = 1; i < 100; i++) {
    f3.write(i);
    if (i < 10) { 
      assertEqual(i, f3.available());
    }
    else { 
      assertEqual(10, f3.available());
    }
  }
}

test(peek) {
  Filter f = Filter(3);
  f.write(1023);
  f.write(310000);
  f.write(99);
  
  assertEqual(3, f.available());
  assertEqual(1023, f.peek());
  assertEqual(3, f.available());
  assertEqual(1023, f.peek());
  assertEqual(3, f.available());
  assertEqual(1023, f.peek(0));
  assertEqual(3, f.available());
  assertEqual(1023, f.peek(0));
  assertEqual(3, f.available());

  assertEqual(3, f.available());
  assertEqual(310000, f.peek(1));
  assertEqual(3, f.available());
  
  assertEqual(3, f.available());
  assertEqual(99, f.peek(2));
  assertEqual(3, f.available());
  assertEqual(99, f.peek(2));
  assertEqual(3, f.available());
}

test(resize) {
  Filter f = Filter(3);
  f.write(1023);
  f.write(310000);
  f.write(99);
  assertEqual(3, f.available());
  f.resize(4);
  f.write(100);
  assertEqual(4, f.available());
  assertEqual(1023, f.peek(0));
  assertEqual(310000, f.peek(1));
  assertEqual(99, f.peek(2));
  assertEqual(100, f.peek(3));
  f.resize(5);
  f.write(4568);
  assertEqual(5, f.available());
  assertEqual(1023, f.peek(0));
  assertEqual(310000, f.peek(1));
  assertEqual(99, f.peek(2));
  assertEqual(100, f.peek(3));
  assertEqual(4568, f.peek(4));
  f.resize(4);
  assertEqual(4, f.available());
  //assertEqual(1023, f.peek(0));
  assertEqual(310000, f.peek());
  assertEqual(99, f.peek(1));
  assertEqual(100, f.peek(2));
  assertEqual(4568, f.peek(3));
  f.resize(1);
  assertEqual(1, f.available());
  assertEqual(4568, f.peek());
}


test(mean) {
  Filter f = Filter(1); 
  f.write(1); 
  assertEqual(1, f.mean());
  f.resize(2);
  f.write(1); 
  assertEqual(1, f.mean());
  f.write(2);
  assertEqual(2, f.mean());
  f.resize(3);
  f.write(1);
  assertEqual(1, f.mean());
  f.write(6);
  assertEqual(3, f.mean());
  f.write(200);
  assertEqual(69, f.mean());
  f.write(2);
  assertEqual(69, f.mean());
}

test(maximum) {
  Filter f0 = Filter(5);
  for (int i = 0; i < 5; i++) {
    f0.write(i);
    assertEqual(i, f0.maximum());
  }
  Filter f1 = Filter(5);
  for (int i = 5; i > 0; i--) {
    f1.write(i);
    assertEqual(5, f1.maximum());
  }
  Filter f2 = Filter(5);
  long data[5] = {45, -4, 1024, 310000, 0};
  for (int i = 0; i < 5; i++) { 
    f2.write(data[i]);
  }
  assertEqual(310000, f2.maximum());
}

test(minimum) {
  Filter f0 = Filter(5);
  for (int i = 0; i < 5; i++) {
    f0.write(i);
    assertEqual(0, f0.minimum());
  }
  Filter f1 = Filter(5);
  for (int i = 5; i > 0; i--) {
    f1.write(i);
    assertEqual(i, f1.minimum());
  }
  Filter f2 = Filter(5);
  long data[5] = {45, -4, 1024, 310000, -200};
  for (int i = 0; i < 5; i++) { 
    f2.write(data[i]);
  }
  assertEqual(-200, f2.minimum());
}

test(stdev) {
  Filter f0 = Filter(1);
  f0.write(1);
  assertEqual(0, f0.stdev());
  assertEqual(0, f0.stdev());
  f0.write(10000);
  assertEqual(0, f0.stdev());
  
  Filter f1 = Filter(2);
  f1.write(1);
  assertEqual(0, f1.stdev());
  assertEqual(0, f1.stdev());
  f1.write(1);
  assertEqual(0, f1.stdev());
  f1.write(500);
  assertEqual(250, f1.stdev());
  f1.write(200000);
  assertEqual(99750, f1.stdev());
}
/*

test(mode) {
  
}
*/

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
