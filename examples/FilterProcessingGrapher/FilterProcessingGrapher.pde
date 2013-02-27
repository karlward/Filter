/*
  Serial Graphing Sketch
 by Tom Igoe
 language: Processing
 
 This sketch takes ASCII values from the serial port 
 at 9600 bps and graphs them.
 Expects CSV values from FilterBasicStatsMinimal in the following order:
 
 raw value, mean, maximum, minimum, std. dev.
 
 Created 20 April 2005
 Updated 19 Feb 2010
 */

import processing.serial.*;

int maxnumberOfValues = 6;       // Arduino has 6 analog inputs, so I chose 6
int numberOfValues = 5;
Serial myPort;                    // The serial port
boolean started;
String label[] = {
  "raw value: ", 
  "mean :", 
  "maximum :", 
  "minimum :", 
  "std. dev. :"
};

float[] previousValue = new float[maxnumberOfValues];  // array of previous values
float[] currentValues =  new float[maxnumberOfValues];
int xpos = 0;                     // x position of the graph
PFont myFont;                     // font for writing text to the window

void setup () {
  // set up the window to whatever size you want:
  size(800, 600);        
  // List all the available serial ports:
  println(Serial.list());


  // set inital background:
  background(0);
  // turn on antialiasing:
  smooth();
  // pick a font:
  PFont myFont = createFont(PFont.list()[3], 14);
  textFont(myFont, 14);
  resetScreen();
}

void draw () {
  if (!started) {
    background(0);
    fill(255);
    // if not started, list the serial ports:
    int textPos = 20;
    text("Pick your serial port's number", 10, textPos);
    textPos += 20;
    for (int i= 0; i < Serial.list().length; i++) {
      text(i + ":  " + Serial.list()[i], 10, textPos);
      textPos += 20;
    }
  }
  else {
    for (int i = 0; i < numberOfValues; i++) {
      if (numberOfValues > i) {
        // map the incoming values (0 to  1023) to an appropriate
        // graphing range (0 to window height/number of values):
        float ypos = map(currentValues[i], 0, 1023, height/numberOfValues - 10, 0);
        float   graphBottom = i * height/numberOfValues + 5;
        ypos = ypos + graphBottom;

        // make a black block to erase the previous text:
        noStroke();
        fill(0);
        rect(10, graphBottom+1, 120, 20);
        // print the sensor numbers to the screen:
        fill(255);
        int textPos = int(graphBottom) + 14;
        text(label[i] + currentValues[i], 10, textPos);

        // change colors to draw the graph line in light blue:
        stroke(#34A3EC);
        line(xpos, previousValue[i], xpos+1, ypos);
        // save the current value to be the next time's previous value:
        previousValue[i] = ypos;
      }
    }
    // if you've drawn to the edge of the window, start at the beginning again:
    if (xpos >= width) {
      resetScreen();
    } 
    else {
      xpos++;
    }
  }
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  // if it's not empty:
  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);

    // convert to an array of ints:
    float incomingValues[] = float(split(inString, ","));
    for (int i = 0; i < incomingValues.length; i++) {
      currentValues[i] = incomingValues[i];
    }
  }
}


void keyReleased() {
  println(key);
  if (key >= '0' && key < (Serial.list().length + 48)) {
    int portNum = key - 48;
    openPort(portNum);
    resetScreen();
    started = true;
  }
}


void openPort (int portNum) {
   // I know that the first port in the serial list on my mac
  // is always my  Arduino or Wiring module, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  String portName = Serial.list()[portNum];
  myPort = new Serial(this, portName, 9600);
  // don't generate a serialEvent() until you get a newline (\n) byte:
  myPort.bufferUntil('\n');
  myPort.clear(); 
}
void resetScreen() {
  xpos = 0;
  background(0);
  // draw graph lines:
  for (int i = 0; i < numberOfValues; i++) {
    // figure out the y position for this particular graph:
    float graphBottom = i * height/numberOfValues;
    // draw a line at the bottom of each graph:
    stroke(127);
    line(0, graphBottom, width, graphBottom);
  }
  // rest the current values array:
  for (int i = 0; i < maxnumberOfValues; i++) {
    currentValues[i] = 0;
  }
}

