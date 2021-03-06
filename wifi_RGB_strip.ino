#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

#define REDPIN 12
#define GREENPIN 14
#define BLUEPIN 16

int red=0, green=0, blue=0;
int pRed=0, pGreen=0, pBlue=0;
bool cycle;

//set up for color cycle
unsigned int rgbColor[3];
    
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  
  
  Serial.begin(115200);
  
  delay(10);

  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();

  if (!client) {
    if(cycle){
      colorCycle();
    }
    return;
  }

  //reset pins
  analogWrite(REDPIN, pRed);
  analogWrite(BLUEPIN, pBlue);
  analogWrite(GREENPIN, pGreen);
  
  // Wait until the client sends some data
  Serial.println("new client");

  while(!client.available()){
    
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  
  
  int first, second, third;
  int firstAnd, secondAnd, thirdAnd;

  if(req.indexOf("red=") != -1 && req.indexOf("green=") != -1 && req.indexOf("blue=") != -1){
    
    first = req.indexOf('=');
    second = req.indexOf('=', first + 1);
    third = req.indexOf('=', second + 1);

    firstAnd = req.indexOf('&');
    secondAnd = req.indexOf('&', firstAnd + 1);
    thirdAnd = req.indexOf('&', secondAnd + 1);
    
    red = req.substring(first + 1, firstAnd).toInt();
    green = req.substring(second + 1, secondAnd).toInt();
    blue = req.substring(third + 1, thirdAnd).toInt();

    cycle = false;

      if (red > 250) {
        red = 250;
      }
      else if (red < 0) {
        red = 0;
      }
    
      if (blue > 250) {
        blue = 250;
      }
      else if (blue < 0) {
        blue = 0;
      }
    
      if (green > 250) {
        green = 250;
      }
      else if (green < 0) {
        green = 0;
      }
      
      
      changeColor(red, pRed, blue, pBlue, green, pGreen);
        
      }

  else if (req.indexOf("cycle") != -1){
    cycle = true;

    // Start off with red.
    rgbColor[0] = 250;
    rgbColor[1] = 5;
    rgbColor[2] = 5;
    
  }

  else {
    Serial.println("No request parameters");
  }


    
  



  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "</html>\n";
  s += "<body> <form action=\"/\" method=\"get\"> Red: <input type=\"text\" name=\"red\"> <br> Green: <input type=\"text\" name=\"green\"> <br> Blue: <input type=\"text\" name=\"blue\"> <br> <input type=\"submit\" value=\"submit\"> </form> </body>";
  s += "<br><a href=\"/?red=0&blue=0&green=0\">Turn Off</a><br>";
  s += "<br><a href=\"/cycle\">Cycle</a><br>";
  s += "<br><a href=\"/?red=255&green=0&blue=0\">Red</a>";  //red link
  s += "<br><a href=\"/?red=0&green=0&blue=255\">Blue</a>";  //blue link
  s += "<br><a href=\"/?red=0&green=255&blue=0\">Green</a>";  //green link
  s += "<br><a href=\"/?red=255&green=127&blue=0\">Orange</a>";  //orange link
  s += "<br><a href=\"/?red=255&green=255&blue=0\">Yellow</a>";  //yellow link
  s += "<br><a href=\"/?red=0&green=255&blue=255\">Cyan</a>";  //cyan link
  s += "<br><a href=\"/?red=255&green=0&blue=255\">Purple</a>";  //purple link
  s += "<br><a href=\"/?red=255&green=255&blue=255\">White</a>";  //white link
  
  // Send the response to the client
  
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  pRed = red;
  pBlue = blue;
  pGreen = green;
  
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


void changeColor(int r, int pR, int b, int pB, int g, int pG) {
  
  for (int i = 0; i<255; i++){
    if(pR > r){
      pR--;
      analogWrite(REDPIN, pR);
    }
    else if (pR < r){
      pR++;
      analogWrite(REDPIN, pR);
    }
    
    if(pB > b){
      pB--;
      analogWrite(BLUEPIN, pB);
    }
    else if (pB < b){
      pB++;
      analogWrite(BLUEPIN, pB);
    }

    if(pG > g){
      pG--;
      analogWrite(GREENPIN, pG);
    }
    else if (pG < g){
      pG++;
      analogWrite(GREENPIN, pG);
    }

    delay(12);
  }
  
}

void setColorRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(REDPIN, red);
  analogWrite(GREENPIN, green);
  analogWrite(BLUEPIN, blue);
 }

void colorCycle() {
  
    // Choose the colors to increment and decrement.
    for (int decColor = 0; decColor < 3; decColor += 1) {
      int incColor = decColor == 2 ? 0 : decColor + 1;
  
      // cross-fade the two colors.
      for(int i = 5; i < 250; i += 1) {
        rgbColor[decColor] -= 1;
        rgbColor[incColor] += 1;
        
        setColorRgb(rgbColor[0], rgbColor[1], rgbColor[2]);
        delay(40);
       }
    }
}

