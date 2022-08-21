/**************************************************************************
  This work is provided for BmE Day Workshop 2022
  
  Designed specifically to work with the project of 
  "Real-time Monitoring Health Monitoring Band using IoT"
  
  which includes the use of the following sensors:
  1. MAX30102 Pulse oximeter and heart rate sensor
  2. MLX90614 GY906 Infrared temperature sensor

  These sensors use I2C to communicate, 2 pins are required to
  interface: SCL (GPIO22) and SDA (GPIO21)

  The following virtual pins are assigned to the Blynk IoT datastream
  - V0: Power
  - V1: Uptime
  - V2: Temperature
  - V3: Temperature Status
  - V4: Heart Rate Average
  - V5: Heart Rate Status

  Written by BmE Day Workshop 2022 academics division
*************************************************************************/

// Authenticate Blynk IoT connection
#define BLYNK_TEMPLATE_ID "Template_ID"
#define BLYNK_DEVICE_NAME "Device_Name"
#define BLYNK_AUTH_TOKEN "Authentication_Token"

// Import libraries required
#include <Wire.h>                 // I2C library
#include "MAX30105.h"             // MAX30102 library
#include "heartRate.h"            // MAX30102 heart rate algorithm 
#include <Adafruit_MLX90614.h>    // GY906 library
#include <WiFi.h>                 // WiFi library
#include <WiFiClient.h>           // WiFi library
#include <BlynkSimpleEsp32.h>     // Blynk library

// Set common alieses for the function to simplify the code
BlynkTimer timer;
MAX30105 particleSensor;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Comment this if you want to detect potential errors (debugging)
#define DEBUG

// Define your WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_SSID";
char pw[] = "Your_Pssword";

// Heart rate variables
const byte RATE_SIZE = 4; // Increase this for more averaging (4 is good enough)
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;        // Time at which the last beat occurred

// Assign variables 
double beatsPerMinute;    // A variable to calculate heart rate based on time interval
int beatAvg;              // Average the two measurements of heart rates
double temp;              // A variable for temperature reading
bool deviceStatus;        // A variable to determine whether the measurement is running or not
String deviceStatus2;
long lastMsg = 0;

void tempReading(){
  temp = mlx.readObjectTempC();          // Read temperature in celsius from GY906 temperature sensor
}

void bpmReading(){
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;     //Store this reading in the array
      rateSpot %= RATE_SIZE;                        //Wrap variable

      // Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
}

// Receive value from Blynk IoT based on the virtual pin
BLYNK_WRITE(V0){
  int powerSwitch = param.asInt();      // Assign incoming value from pin V0 to a variable

  if (powerSwitch > 0){
    deviceStatus = true;
    deviceStatus2 = "Running";
  }
  else {
    deviceStatus = false;
    deviceStatus2 = "Idle";
  }
  Serial.print("Measurement Status: ");
  Serial.println(deviceStatus2);
  delay(1000);
}

// A function for sending sensor reading to Blynk datastream
void sendSensor(){
  // Temperature measurement       
  Blynk.virtualWrite(V2, temp);         

  #ifdef DEBUG
    Serial.print("Temperature = ");
    Serial.println(temp);
  #endif
  
  // A conditional to determine the patient's status based on the acquired temperature reading
  if(temp<36 || temp>38){
    Blynk.virtualWrite(V3, "Abnormal");
  }
  else{
    Blynk.virtualWrite(V3, "Normal");
  }
  
  // Heart rate measurement
  Blynk.virtualWrite(V4, beatAvg);

  #ifdef DEBUG
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.println(beatAvg);
  #endif

  // A conditional to determine the patient's status based on the acquired heart rate reading
  if (irValue < 50000){
    Serial.println("No skin detected");
    Blynk.virtualWrite(V5, "Not detected");
  }
  
  else{
    if(beatAvg<60 || beatAvg>100){
      Blynk.virtualWrite(V5, "Abnormal");
    }
    else{
    Blynk.virtualWrite(V5, "Normal");
    }
  }
}

// Set an uptime on Blynk dashboard to see how long the measurement has been running
void myTimerEvent(){
  Blynk.virtualWrite(V1, millis() / 1000);  
}

void setup(){
  // Begin serial at the speed of 115200 bauds
  Serial.begin(115200);
  while (!Serial);

  // Initialize ESP32 to Blynk IoT connection
  Blynk.begin(auth, ssid, pw);
  Serial.println("Blynk IoT connected");
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, myTimerEvent);

  // Turn off the device 
  Blynk.virtualWrite(V0,0);     // Switch off the power toggle on Blynk dashboard
  deviceStatus = false;         // Set the measurement status to not running
  Serial.println("Measurement Status: Idle");

  // Initialize GY906 temperature sensor
  Serial.println("MLX90614 GY906 temperature sensor: testing");

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614 GY906 sensor. Check wiring.");
    while (1);
  };

  Serial.println("MLX90614 GY906 temperature sensor: connected");
  delay(500);

  #ifdef DEBUG
    Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  #endif

  // Initialize MAX30102 heart rate sensor
  Serial.println("MAX30102 heart rate sensor: testing");

  if (!particleSensor.begin()){
    Serial.println("Error connecting to MAX30102 sensor. Check wiring.");
    while (1);
  }
  Serial.println("MAX30102 heart rate sensor: connected");
  delay(500);
  Serial.println("Control the HealthBand on your Blynk IoT dashboard");
  delay(500);
  Serial.println("Place your skin on the sensor with steady pressure");
  delay(500);

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
}

void loop(){
  Blynk.run();      // Keep the Blynk IoT running and looping

  // If the measurement status is running, begin the measurement
  while (deviceStatus == true){
    tempReading();
    bpmReading();
    timer.run();
  }
  
  #ifdef DEBUG
    Serial.println("Your device is inactive");
    delay(2000);
  #endif
}
