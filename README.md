# BME Day Workshop 2022
This work is provided for the BME Day Workshop 2022, organized by the Biomedical Engineering department at Swiss German University. This repository involves the guideline to create Real-time Health Monitoring Band using IoT. Details about this project are written on BME Day Workshop 2022 Day 1 Module (available on the reference link below) and on the README section on this repository.

## Real-time Health Monitoring Band using IoT
This device undergoes heart rate and temperature measurements based on two sensor modules of MAX30102 pulse oximeter sensor and MLX90614 GY906 temperature sensor.

## Block Diagram
![HealthBand Block Diagram](https://user-images.githubusercontent.com/108512595/185776050-f7f194a8-dae0-4d06-a610-88c9fefe6d6e.png)

## Flowchart
![HealthBand Flowchart](https://user-images.githubusercontent.com/108512595/185775979-5e2248bc-db67-4e23-99f2-3221da1f7202.png)

## Circuit Diagram
![Fix Schematic](https://user-images.githubusercontent.com/108512595/185776716-091eb823-3116-40f1-9c2e-25cf9ee5230e.png)

## Required Libraries
Install the following libraries to your Arduino libraries folder before executing the src program:
1. Wire (Available on Arduino)
2. [SparkFun_MAX3010x_Sensor](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)
3. [Adafruit-MLX90614](https://github.com/adafruit/Adafruit-MLX90614-Library)
4. Blynk (Available on Arduino library manager)

## Directories
/examples : Example sketches for the blynk connection and temperature and heart rate measurements <br />
/src      : Source file for the HealthBand program (.ino) 

