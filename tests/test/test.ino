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

//  for (int j = 90; j < 100; j++) { 
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

/*
test(flushInt) {
  int data[3] = {1023,31000,99};
  DataStream<int> s = DataStream<int>(3, data, 3);
  s.flush();
  assertEqual(0, s.available());
} 

test(flushLong) {
  long data[3] = {1023,3100000000,99};
  DataStream<long> s = DataStream<long>(3, data, 3);
  s.flush();
  assertEqual(0, s.available());
} 

test(flushFloat) {
  float data[3] = {1023.5,31000.11,99.08};
  DataStream<float> s = DataStream<float>(3, data, 3);
  s.flush();
  assertEqual(0, s.available());
} 

test(resize) {
  int data[3] = {1023,31000,99};
  DataStream<int> s = DataStream<int>(3, data, 3);
  assertEqual(3, s.available());
  s.resize(4);
  s.write(100);
  assertEqual(4, s.available());
  s.resize(5);
  s.write(4568);
  assertEqual(5, s.available());
  s.resize(4);
  assertEqual(4, s.available());
  s.resize(1);
  assertEqual(1, s.available());
}

test(writeOrdered) {
  int data[6] = {1023,31000,99,0,18,-32};
  DataStream<int> s = DataStream<int>(6);
  for (int i=0; i<6; i++) {
    s.writeOrdered(data[i]);
  }
  assertEqual(-32, s.read());
  assertEqual(0, s.read());
  assertEqual(18, s.read());
  assertEqual(99, s.read());
  assertEqual(1023, s.read());
  assertEqual(31000, s.read());
}
*/

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
