#include "globals.h"
#include <TinyGPS++.h>
#include <Arduino.h>
#include <math.h>
#include <SD.h>
#include <SPI.h>

// Pins
#define RXD2 3
#define TXD2 1
#define GPS_BAUD 9600

// SD Card SPI Pins
#define SD_CS   5
#define SD_MOSI 15
#define SD_CLK  18
#define SD_MISO 35


// global opjects

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

struct Waypoint {
  double lat;
  double lon;
  String name;
};

#define MAX_WAYPOINTS 50
Waypoint waypoints[MAX_WAYPOINTS];
int totalWaypoints = 0;
int currentWaypoint = 0;

// GPS HEADING VARS
double lastLat = 0;
double lastLon = 0;
bool hasLastPosition = false;
double gpsHeading = 0;  // Derived from movement

// math helpers
double toRadians(double degrees) { return degrees * (M_PI / 180.0); }
double toDegrees(double radians) { return radians * (180.0 / M_PI); }

// GEO CALCULATIONS
double calculateBearing(double gpslat, double gpslon, double waypointlat, double waypointlon) {
  double startLat = toRadians(gpslat);
  double startLon = toRadians(gpslon);
  double endLat = toRadians(waypointlat);
  double endLon = toRadians(waypointlon);
  double dLon = endLon - startLon;

  double x = sin(dLon) * cos(endLat);
  double y = cos(startLat) * sin(endLat) - sin(startLat) * cos(endLat) * cos(dLon);

  double initialBearing = atan2(x, y);
  initialBearing = toDegrees(initialBearing);
  return fmod((initialBearing + 360.0), 360.0);
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000;
  double dLat = toRadians(lat2 - lat1);
  double dLon = toRadians(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(toRadians(lat1)) * cos(toRadians(lat2)) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

// compass helpers
const char* getCompassDirection(double bearing) {
  const char* dirs[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
  int idx = (int)((bearing + 22.5) / 45.0) % 8;
  return dirs[idx];
}

// finding heading of of gps
double calculateHeadingFromMovement(double lat1, double lon1, double lat2, double lon2) {
  double dLon = toRadians(lon2 - lon1);
  double y = sin(dLon) * cos(toRadians(lat2));
  double x = cos(toRadians(lat1)) * sin(toRadians(lat2)) -
             sin(toRadians(lat1)) * cos(toRadians(lat2)) * cos(dLon);
  double heading = atan2(y, x);
  heading = toDegrees(heading);
  if (heading < 0) heading += 360.0;
  return heading;
}

// load SD card/GPX file
bool loadGPXFromSD(const char *filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial.println("Failed to open GPX file!");
    return false;
  }

  totalWaypoints = 0;
  String line;
  double lat = 0, lon = 0;
  String name = "";

  while (file.available() && totalWaypoints < MAX_WAYPOINTS) {
    line = file.readStringUntil('\n');
    line.trim();

    // Parse waypoint coordinates
    if (line.startsWith("<wpt")) {
      int latPos = line.indexOf("lat=\"");
      int lonPos = line.indexOf("lon=\"");
      if (latPos != -1 && lonPos != -1) {
        lat = line.substring(latPos + 5, line.indexOf("\"", latPos + 5)).toDouble();
        lon = line.substring(lonPos + 5, line.indexOf("\"", lonPos + 5)).toDouble();
      }
    }

    // Parse waypoint name
    if (line.startsWith("<name>")) {
      int start = line.indexOf("<name>") + 6;
      int end = line.indexOf("</name>");
      if (start != -1 && end != -1) {
        name = line.substring(start, end);
      }
    }

    // If both found, store waypoint
    if (line.startsWith("</wpt>")) {
      waypoints[totalWaypoints].lat = lat;
      waypoints[totalWaypoints].lon = lon;
      waypoints[totalWaypoints].name = name;
      totalWaypoints++;
      lat = lon = 0;
      name = "";
    }
  }

  file.close();

  Serial.printf("Loaded %d waypoints from GPX file:\n", totalWaypoints);
  for (int i = 0; i < totalWaypoints; i++) {
    Serial.printf("  %d. %s (%.6f, %.6f)\n",
                  i + 1,
                  waypoints[i].name.c_str(),
                  waypoints[i].lat,
                  waypoints[i].lon);
  }

  return totalWaypoints > 0;
}

// Find waypoint
bool gotoWaypointByName(const String &targetName) {
  for (int i = 0; i < totalWaypoints; i++) {
    if (waypoints[i].name.equalsIgnoreCase(targetName)) {
      currentWaypoint = i;
      Serial.printf("Jumped to waypoint #%d: %s (%.6f, %.6f)\n",
                    i + 1,
                    waypoints[i].name.c_str(),
                    waypoints[i].lat,
                    waypoints[i].lon);
      return true;
    }
  }
  Serial.printf("Waypoint '%s' not found!\n", targetName.c_str());
  return false;
}


void waypoint_setup() {
  Serial.begin(115200);
  Serial.println("Initializing SD and loading GPX waypoints...");

  // Initialize SPI + SD
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card mount failed!");
    while (true);
  }

  if (!loadGPXFromSD("/waypoints.gpx")) {
    Serial.println("Failed to load GPX — using fallback waypoints.");
    waypoints[0] = {48.229056, -114.327107, "Default A"};
    waypoints[1] = {34.0522, -118.2437, "Default B"};
    totalWaypoints = 2;
  }

  Serial.println("Waypoints loaded. Starting GPS...");
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("GPS Serial started.");
}

void waypoint() {
  // Handle Serial Commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("goto ")) {
      String name = cmd.substring(5);
      gotoWaypointByName(name);
    } else if (cmd.equalsIgnoreCase("list")) {
      Serial.println("Available waypoints:");
      for (int i = 0; i < totalWaypoints; i++) {
        Serial.printf("  %d. %s\n", i + 1, waypoints[i].name.c_str());
      }
    } else {
      Serial.println("Unknown command. Use 'goto <name>' or 'list'.");
    }
  }

  // Read GPS data
  while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());

  if (gps.location.isUpdated() && totalWaypoints > 0) {
    double gpslat = gps.location.lat();
    double gpslon = gps.location.lng();

    // Calculate GPS-based heading if we’ve moved enough
    if (hasLastPosition) {
      double distanceMoved = calculateDistance(lastLat, lastLon, gpslat, gpslon);
      if (distanceMoved > 0.5) { // update heading only if moved > 0.5 m
        gpsHeading = calculateHeadingFromMovement(lastLat, lastLon, gpslat, gpslon);
        lastLat = gpslat;
        lastLon = gpslon;
      }
    } else {
      lastLat = gpslat;
      lastLon = gpslon;
      hasLastPosition = true;
    }

    // Current waypoint
    Waypoint wp = waypoints[currentWaypoint];

    double distance = calculateDistance(gpslat, gpslon, wp.lat, wp.lon);
    double bearing = calculateBearing(gpslat, gpslon, wp.lat, wp.lon);

    // Compare current heading to target bearing
    turnangle = bearing - gpsHeading;
    if (turnangle < -180) turnangle += 360;
    if (turnangle > 180) turnangle -= 360;

    String turnDir = (turnangle > 0) ? "Turn Right" : "Turn Left";
    if (fabs(turnangle) < 10) turnDir = "On Course";

    // Print Info
    Serial.printf("LAT: %.6f  LON: %.6f\n", gpslat, gpslon);
    Serial.printf("Heading: %.2f° (from GPS movement)\n", gpsHeading);
    Serial.printf("Waypoint: %s (%.6f, %.6f)\n", wp.name.c_str(), wp.lat, wp.lon);
    Serial.printf("Distance: %.2f m  Bearing: %s %.2f°\n",
                  distance, getCompassDirection(bearing), bearing);
    Serial.printf("Turn: %s (Δ=%.2f°)\n", turnDir.c_str(), turnangle);
    Serial.println("----------------------");

    // Move to next waypoint if close enough
    if (distance < 1.0) {
      Serial.printf("Reached %s\n", wp.name.c_str());
      currentWaypoint++;
      if (currentWaypoint >= totalWaypoints) {
        Serial.println("All waypoints reached! Restarting...");
        currentWaypoint = 0;
      }
    }

    delay(500);
  }
}

