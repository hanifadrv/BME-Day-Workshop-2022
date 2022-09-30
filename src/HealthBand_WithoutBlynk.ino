/**************************************************************************
  This work is provided for BmE Day Workshop 2022
  
  Designed specifically to work with the project of 
  "Real-time Monitoring Health Monitoring Band using IoT"
  
  which includes the use of the following sensors:
  1. MAX30102 Pulse oximeter and heart rate sensor
  2. MLX90614 GY906 Infrared temperature sensor

  These sensors use I2C to communicate, 2 pins are required to
  interface: SCL (GPIO22) and SDA (GPIO21)

  Written by BmE Day Workshop 2022 academics division
*************************************************************************/

// Import libraries required
#include <Wire.h>                 // I2C library
#include "MAX30105.h"             // MAX30102 library
#include "heartRate.h"            // MAX30102 heart rate algorithm 
#include <Adafruit_MLX90614.h>    // GY906 library
// #include <Adafruit_I2CDevice.h>   // I2C library

// Set common alieses for the function to simplify the code
MAX30105 particleSensor;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Comment this if you want to detect potential errors (debugging)
#define DEBUG

// Heart rate variables
const byte RATE_SIZE = 4; // Increase this for more averaging (4 is good enough)
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;        // Time at which the last beat occurred
long irValue;

// Assign variables 
double beatsPerMinute;    // A variable to calculate heart rate based on time interval
int beatAvg;              // Average the two measurements of heart rates
double temp;              // A variable for temperature reading
long lastMsg = 0;

void tempReading(){
  temp = mlx.readObjectTempC();          // Read temperature in celsius from GY906 temperature sensor
}

void bpmReading(){
  irValue = particleSensor.getIR();

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

// A function for sending sensor reading to Blynk datastream
void sendSensor(){
  // Heart Rate measurement
  Serial.print("IR = ");
  Serial.print(irValue);
  
  Serial.print("\tAvg BPM = ");
  Serial.print(beatAvg);

  // Temperature measurement       
  Serial.print("\tTemperature = ");
  Serial.print(temp);
  Serial.println(" C");
  
  // A conditional to determine the patient's status based on the acquired heart rate reading
  Serial.print("Heart Rate Status: ");

  if (irValue < 50000){
    Serial.print("Not detected");
  }

  else{
    if(beatAvg<60 || beatAvg>100){
      Serial.print("Abnormal");
    }
    else{
      Serial.print("Normal");
    }
  }
  
  // A conditional to determine the patient's status based on the acquired temperature reading
  Serial.print("\tTemperature Status: ");

  if(temp<36 || temp>38){
    Serial.println("Abnormal");
  }
  else{
    Serial.println("Normal");
  }
}

void setup(){
  // Begin serial at the speed of 115200 bauds
  Serial.begin(115200);
  while (!Serial);

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
  Serial.println("Place your skin on the sensor with steady pressure");
  delay(500);

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
}

void loop(){
  // Begin measurements
  tempReading();
  bpmReading();
  sendSensor();

  /*
  long now = millis();
  if (now - lastMsg > 1000){
    lastMsg = now;
    sendSensor();
  }
  */
}
