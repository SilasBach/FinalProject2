# ESP DS18B20 Temperature Monitor Web App User Guide

## Introduction
This document serves as a comprehensive guide for users of the ESP DS18B20 Temperature Monitor web application. This app is designed to provide real-time temperature monitoring using the ESP DS18B20 sensor.

## Setup and Installation
### Sensor Setup
1. Connect the ESP DS18B20 sensor to your device according to the manufacturer's instructions.
2. Ensure that the sensor is correctly powered and transmitting data.

### Application Deployment
1. Deploy the web application on a suitable web server.
2. Ensure that the server is correctly configured to receive data from the ESP DS18B20 sensor.

## User Interface Description
The web application features a simple and intuitive interface that includes:
- A real-time temperature display in degrees Celsius.
- Buttons for adjusting data fetching intervals.
- A reset button for the interval.
- An option to download the temperature data as a CSV file.
- A graphical chart showing temperature trends over time.

## Using the Web App
### Viewing Temperature Data
- The main display shows the current temperature reading from the sensor.
### Adjusting Data Fetching Interval
- Use the 'Increase Interval' and 'Decrease Interval' buttons to adjust the frequency of data updates.
### Resetting the Interval
- Click the 'Reset' button to restore the default data fetching interval.
### Downloading Data
- Click the 'Download CSV' button to download the temperature data in CSV format.

## Understanding the Temperature Chart
- The chart provides a visual representation of temperature changes over time.
- Data points on the chart correspond to temperature readings at specific times.

## Technical Details
The web app is built using HTML, CSS, and JavaScript. It uses Chart.js for data visualization and AJAX calls for real-time data fetching from the ESP DS18B20 sensor.

## Troubleshooting and Support
For common issues:
- Ensure the sensor is correctly connected and powered.
- Check if the web server is properly configured and running.

## Appendix
- ESP DS18B20 Sensor Manual
- Chart.js Documentation