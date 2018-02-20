#include <Ethernet.h>
#include <SD.h>
// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   60
int light = 13;
int fan = 6;
int ltsnr = 3;
int intrd = 4;
int button = 2;
int temp = 0;
int buzz = 7;
int lt, itd, btstate;



// MAC address from Ethernet shield sticker under board
byte mac[] = { 0x98, 0x4F, 0xEE, 0x01, 0xAE, 0x54 };
IPAddress ip(192, 168, 0, 107); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File webFile;               // the web page file on the SD card
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer
boolean LED_state[4] = {0}; // stores the states of the LEDs

void setup()
{
    // disable Ethernet chip
    //pinMode(10, OUTPUT);
    //digitalWrite(10, HIGH);
    
    Serial.begin(9600);       // for debugging
    
    // initialize SD card
  /*  Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
    // switches on pins 2, 3 and 5
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(5, INPUT);*/
    // LEDs
    pinMode(light, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(ltsnr, INPUT);
    pinMode(intrd, INPUT);
    pinMode(light, OUTPUT);
    pinMode(button, INPUT);
    pinMode(buzz, OUTPUT);
    /*pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);*/
    
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
}

void loop()
{
  btstate = digitalRead(button);
  if(btstate == HIGH)
  {
    lt = digitalRead(ltsnr);
    if(lt == HIGH)
      digitalWrite(light, HIGH);
    else
      digitalWrite(light, LOW);
    int rawvoltage= analogRead(temp);
    float millivolts= (rawvoltage/1024.0) * 5000;
    float celsius= millivolts/10;
    Serial.println(celsius);
    if(celsius > 35)
      digitalWrite(buzz, HIGH);
    else
      digitalWrite(buzz, LOW);
   }
  else
  {
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (StrContains(HTTP_req, "setleds")) {
                        // send rest of HTTP header
                        
                        client.println();
                        Setappl();
                        // send XML file containing input states
                        //XML_response(client);
                    }
                    else {  // web page request
                           itd = digitalRead(intrd);
                           client.println();
                        // send web page
                           client.println("<html>");
                           client.println("<head>");
                           client.println("<script language=\"javascript\">");
                           //client.println("alert(\"Movement Detected\");");
                           if(itd == HIGH && curst == LOW)
                           {
                             client.println("alert(\"Movement Detected\");");
                             curst = HIGH
                           }
                           if(itd == LOW)
                             curst == LOW; 
                           //client.println("document.getElementById(\"itrd\").value=\"off\";"); 
                           client.println("</script>");
                           client.println("</head>");
                           client.println("<body>");
                           client.println("<input type=\"text\" id=\"itrd\" value=\"hello\"/><br/>");
                           client.println("</body>");
                           client.println("</html>");   
                        } 
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
            }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
      /* EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println("<html>");
                    client.println(" <head>");
                    client.println("<style type=\"text/css\">");
                    client.println("<p><font color=\"red\" font size=\"6\">INTRUDER ALERT!</font></p>");
                }
            }
        }
    }*/
      
}

// checks if received HTTP request is switching on/off LEDs
// also saves the state of the LEDs
void Setappl(void)
{
    if (StrContains(HTTP_req, "lgt=1")) {
       // LED_state[0] = 1;  // save LED state
        digitalWrite(light, HIGH);
    }
    else if (StrContains(HTTP_req, "lgt=0")) {
        //LED_state[0] = 0;  // save LED state
        digitalWrite(light, LOW);
    }
    
    if (StrContains(HTTP_req, "fan=1")) {
        //LED_state[1] = 1;  // save LED state
        digitalWrite(fan, HIGH);
    }
    else if (StrContains(HTTP_req, "fan=0")) {
        //LED_state[1] = 0;  // save LED state
        digitalWrite(fan, LOW);
    }
    // LED 3 (pin 8)
  /*  if (StrContains(HTTP_req, "LED3=1")) {
        LED_state[2] = 1;  // save LED state
        digitalWrite(8, HIGH);
    }
    else if (StrContains(HTTP_req, "LED3=0")) {
        LED_state[2] = 0;  // save LED state
        digitalWrite(8, LOW);
    }
    // LED 4 (pin 9)
    if (StrContains(HTTP_req, "LED4=1")) {
        LED_state[3] = 1;  // save LED state
        digitalWrite(9, HIGH);
    }
    else if (StrContains(HTTP_req, "LED4=0")) {
        LED_state[3] = 0;  // save LED state
        digitalWrite(9, LOW);
    }*/
}

// send the XML file with analog values, switch status
//  and LED status
/*void XML_response(EthernetClient cl)
{
    int analog_val;            // stores value read from analog inputs
    int count;                 // used by 'for' loops
    int sw_arr[] = {2, 3, 5};  // pins interfaced to switches
    
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    // read analog inputs
    for (count = 2; count <= 5; count++) { // A2 to A5
        analog_val = analogRead(count);
        cl.print("<analog>");
        cl.print(analog_val);
        cl.println("</analog>");
    }
    // read switches
    for (count = 0; count < 3; count++) {
        cl.print("<switch>");
        if (digitalRead(sw_arr[count])) {
            cl.print("ON");
        }
        else {
            cl.print("OFF");
        }
        cl.println("</switch>");
    }
    // checkbox LED states
    // LED1
    cl.print("<LED>");
    if (LED_state[0]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
    cl.println("</LED>");
    // LED2
    cl.print("<LED>");
    if (LED_state[1]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
     cl.println("</LED>");
    // button LED states
    // LED3
    cl.print("<LED>");
    if (LED_state[2]) {
        cl.print("on");
    }
    else {
        cl.print("off");
    }
    cl.println("</LED>");
    // LED4
    cl.print("<LED>");
    if (LED_state[3]) {
        cl.print("on");
    }
    else {
        cl.print("off");
    }
    cl.println("</LED>");
    
    cl.print("</inputs>");
}
*/
// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
