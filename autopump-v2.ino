// Water level monitor

#define version 103

#define PERIOD 90   // wake up and check water level each 90s
#define TIMEOUT 15000 // 15s timeout and ask for help

// Define the pins we're going to call pinMode on
int led = D7;  // You'll need to wire an LED to this one to see it blink.
int sensorLightH = D0;
int sensorLightL = D1;
int pumpSwitch = D2;
int waterSensorH = A0;
int waterSensorL = A1;

int lastRead = 0;


const char * DID_WL = "vF86B705D61B0EE3&message=Water+level+abnormal%21";

int waterLevelL;
int waterLevelH;
int state = 0;
TCPClient client;
//const char * serverName = "api.pushingbox.com";   // PushingBox API URL
const char * serverName = "www.iostation.com";


// This routine runs only once upon reset
void setup() {
  // Initialize D0 + D7 pin as output
  // It's important you do this here, inside the setup() function rather than outside it or in the loop function.
  pinMode(led, OUTPUT);
  pinMode(sensorLightL, OUTPUT);
  pinMode(sensorLightH, OUTPUT);
  pinMode(pumpSwitch, OUTPUT);
  pinMode(waterSensorL, INPUT);
  pinMode(waterSensorH, INPUT);
  Spark.variable("waterLevelH", &waterLevelH, INT);
  Spark.variable("waterLevelL", &waterLevelL, INT);
  Spark.function("pump", startPump10s);
  Spark.function("version", getVersion);
  stopPump();
//  delay(10000);
}

void sendToPushingBox(const char * devid, String mesg, String mesg2)
{
    client.stop();
    String outMesg = String("+%28L/H:"+mesg+"/"+mesg2+"%29");
    RGB.control(true);
    RGB.color(0,255,0);
    if (client.connect(serverName, 80)) {
        client.print("GET /util/pushingbox.php?devid=");
        client.print(DID_WL);
        client.print(outMesg);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverName);
        client.println("User-Agent: Spark");
        client.println("Connection: close");
        client.println();
        client.flush();
        RGB.control(false);
    } 
    else{
        RGB.color(255,0,0);
    }
}

void startPump()
{
    digitalWrite(pumpSwitch, HIGH);
}

void stopPump()
{
    digitalWrite(pumpSwitch, LOW);
}

int readWaterLevelL()
{
    int waterLevel;
    digitalWrite(sensorLightL, HIGH);
    delay(100);
    waterLevel = analogRead(waterSensorL);
    delay(100);
    digitalWrite(sensorLightL, LOW);
    return waterLevel;
}

int readWaterLevelH()
{
    int waterLevel;
    digitalWrite(sensorLightH, HIGH);
    delay(100);
    waterLevel = analogRead(waterSensorH);
    delay(100);
    digitalWrite(sensorLightH, LOW);
    return waterLevel;
}

int startPump10s(String cmd)
{
    startPump();
    delay(10000);
    stopPump();
    return 0;
}

int getVersion(String cmd)
{
    return version;
}

// This routine gets called repeatedly, like once every 5-15 milliseconds.
// Spark firmware interleaves background CPU activity associated with WiFi + Cloud activity with your code. 
// Make sure none of your code delays or blocks for too long (like more than 5 seconds), or weird things can happen.
void loop() {
  digitalWrite(led, HIGH);   // Turn ON the LED pins
  if (readWaterLevelH() > 2500) {
        startPump();
        lastRead = millis();
        while ((readWaterLevelL() > 1200) && (millis() - lastRead < TIMEOUT)) {
            delay(500);
        }
        stopPump();
        if (millis() - lastRead > TIMEOUT) {
            // Huston, we have a situation, the water level is not down
            // or pump is not working
            sendToPushingBox(DID_WL, String(readWaterLevelL()), String(readWaterLevelH()));
        }
  }
  stopPump();
  delay(500);
  digitalWrite(led, LOW);    // Turn OFF the LED pins
  Spark.sleep(SLEEP_MODE_DEEP, PERIOD);

}
