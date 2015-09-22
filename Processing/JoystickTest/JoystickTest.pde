import processing.serial.*;

int bgcolor=0;           // Background color
int fgcolor=255;           // Fill color
Serial myPort;                       // The serial port
int[] serialInArray = new int[20];   // Where we'll put what we receive
int packetIndex = 0;                 // A count of how many bytes we receive
int xpos, ypos;                      // Starting position of the ball
String xVal = "nada";
String yVal = "nada";
int east, west, north, south;

void setup() 
{
  size(768, 768);  // Stage size

  // Set the starting position of the ball (middle of the stage)
  xpos = width/2;
  ypos = height/2;

  textSize(18);
  println(Serial.list()[1]);

  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 57600);
  myPort.bufferUntil('#');
}

void draw() 
{
  background(bgcolor);
  fill(fgcolor);
  noStroke();

  ellipse(xpos, ypos, 20, 20);

  stroke(fgcolor);
  strokeWeight(3);

  line(0,height/2,width,height/2);
  line(width/2,0,width/2,height);

  strokeWeight(1);
  textAlign(LEFT);
  text(xpos-(width/2), 4, (height/2)+18);
  text(east, 4, (height/2)+36);
  text(west, 4, (height/2)+54);

  text(ypos-(height/2),  (width/2)+4, 14);
  text(south, (width/2)+4, 32);
  text(north, (width/2)+4, 50);
}

void serialEvent(Serial myPort) 
{
  
  // incoming packets should look like this:
  //  $N0,N1,N2,...,Nn# where N is some signed value. At present, here's the 
  //  list of values and their indices:
  //  N0 = x offset from 0
  //  N1 = y offset from 0
  //  N2 = raw west sensor value
  //  N3 = raw east sensor value
  //  N4 = raw north sensor value
  //  N5 = raw south sensor value
  String inString = myPort.readStringUntil('#');
  
  // If we find a # but there's no $ at the start, we've got a broken packet
  //  and we should just discard it.
  if (inString.charAt(0) != '$')
  {
    myPort.clear();
    return;
  }
  
  // Now let's split the string by commas. That will yield n substrings. The
  //  first one will be only the SOP character ('$') and the last will be only
  //  the EOP character ('#') so those may be ignored.
  String items[] = split(inString, ',');
  int n = items.length;
  int strLen = 0;
  int[] data = new int[n-2]; // we only need n-2 data values, since we know
                             //  that the first and last items were not data.
  
  for (int x = 1; x < n-1; x++)
  {
    data[x-1] = int(items[x]);
  }
  
  // Now we can deal with the various data as specified above. Note that data
  //  which is unused will simply be ignored.
  int DEADZONE = 0;
  if (data[0] > 0)
  {
    xpos = width/2 + data[0] - DEADZONE;
  }
  else if (data[0] < 0)
  {
    xpos = width/2 + data[0] + DEADZONE;
  }
  else
  {
    xpos = width/2;
  }
  if (data[1] > 0)
  {
    ypos = height/2 + data[1] - DEADZONE;
  }
  else if (data[1] < 0)
  {
    ypos = height/2 + data[1] + DEADZONE;
  }
  else
  {
    ypos = height/2;
  }

  if ((n-2) > 2)
  {
    east = data[2];
    west = data[3];
    north = data[4];
    south = data[5];
  }
}

