#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Parth iphone (2802)";
const char* password = "hotspot123$";

AsyncWebServer server(80);

// Hardware pins
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int LED_PIN = 32;
const int MOTO_PIN1 = 2;
const int MOTO_PIN2 = 15;
const int MOTO_PIN3 = 26;
const int MOTO_PIN4 = 27;

float distance_cm, duration_us;

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, 16, 2); //set the display params


// Timing and session management
unsigned long totalOnTime = 0;        // Total active time (ms)
unsigned long sessionStart = 0;       // Current session start time
unsigned long lastActivation = 0;     // Last valid motion detection
volatile long timeremaining = 0;      // Countdown timer (ms)
bool sessionActive = false;           // Current session state
bool motorRunning = false;            // Motor control flag
unsigned long motorStopTime = 0;      // Motor auto-stop time'

//crane code
int inc = 0;

const int DISTANCE_THRESHOLD = 50; // cm


bool isLeft = false;

void setup() {
  Serial.begin(9600);

  // Initialize hardware
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTO_PIN1, OUTPUT);
  pinMode(MOTO_PIN2, OUTPUT);
  pinMode(MOTO_PIN3, OUTPUT);
  pinMode(MOTO_PIN4, OUTPUT);
  digitalWrite(LED_PIN, LOW);


  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Waiting....");
 

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  inc = timeremaining / 5;

  // Configure HTTP endpoints
  server.on("/total-ontime", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"totalOnTime\":" + String(getCurrentTotalTime()) + "}";
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/timer-remaining", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"timeremaining\":" + String(timeremaining) + "}";
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/set-timer", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value", true)) {
      String valueStr = request->getParam("value", true)->value();
      timeremaining = valueStr.toInt() * 1000;
      Serial.print("Timer set to: ");
      Serial.println(timeremaining / 1000);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();

  while(timeremaining == 0) {
    delay(500);
    Serial.println("Waiting for Time");

  }
}

void loop() {
  
 

  unsigned long currentTime = millis();
  
  // Update sensor reading
  updateDistanceSensor();

  // set the display on the lcd

  
  // Handle motion detection
  if (distance_cm < DISTANCE_THRESHOLD) {
    handleMotion(currentTime);
  } else {
    handleNoMotion(currentTime);
  }

  // Manage motor operation
  updateMotorState(currentTime);
  
  // Update countdown timer
  updateTimer(currentTime);
  
  // Periodic status logging
  logStatus(currentTime);
}

void updateDistanceSensor() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = duration_us * 0.017;
}

void handleMotion(unsigned long currentTime) {
  digitalWrite(LED_PIN, HIGH);
  
  // Start new session or extend existing one
  if (!sessionActive || (currentTime - lastActivation > 10000)) {
    if (sessionActive && ((lastActivation - sessionStart) > 0)) {
      totalOnTime += lastActivation - sessionStart;
    }
    sessionStart = currentTime;
    sessionActive = true;
    Serial.println("Session started/renewed");
  }

  // Update active time
  // totalOnTime += currentTime - lastActivation;
  lastActivation = currentTime;

  // Activate motor if not running
  if (!motorRunning) {
    if(isLeft) {
      goLeft();
      delay(3000);
      motoStop();
      isLeft = false;
    }
    else {
      goRight();
      delay(3000);
      motoStop();
      isLeft = true;
    }
  } 
  // if(distance_cm < 20) {
  //   goRight();
  //   delay(5000);
  //   motoStop();
  // }
  // else {
  //   goLeft();
  //   delay(5000);
  //   motoStop();
  // }

  motorRunning = true;
  motorStopTime = currentTime + 1000;
}


void handleNoMotion(unsigned long currentTime) {
  digitalWrite(LED_PIN, LOW);
  
  // Check for session timeout
  if (sessionActive && currentTime - lastActivation > 10000) {
    // Add the session duration up to the timeout, not beyond
    unsigned long sessionDuration = (lastActivation + 10000) - sessionStart;
    if (sessionDuration > 0) {
      totalOnTime += sessionDuration;
    }
    sessionActive = false;
    Serial.print("Session ended. Total: ");
    Serial.println(totalOnTime / 1000.0, 1);
  }
}

void updateMotorState(unsigned long currentTime) {
  if (motorRunning && currentTime >= motorStopTime) {
    digitalWrite(MOTO_PIN1, LOW);
    digitalWrite(MOTO_PIN2, LOW);
    motorRunning = false;
  }
}

void updateTimer(unsigned long currentTime) {
  static unsigned long lastTimerUpdate = 0;
  
  if (timeremaining > 0 && sessionActive) {
    if (currentTime - lastTimerUpdate >= 1000) {
      timeremaining -= 1000;
      lastTimerUpdate = currentTime;
      Serial.print("Timer remaining: ");
      Serial.println(timeremaining / 1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time Remaining:");
      lcd.setCursor(0, 1);
      lcd.print(timeremaining / 60000);
      lcd.print(":");
      lcd.print((timeremaining % 60000) / 1000);
    

    }
  }
  else if(timeremaining == 0) {
    craneMotor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Excercise Done!");

    while(timeremaining == 0) {
      Serial.println("Waiting for time");
    }
  }
}

unsigned long getCurrentTotalTime() {
  unsigned long currentTotal = totalOnTime;
  if (sessionActive && millis() - sessionStart > 0) {
    currentTotal += millis() - sessionStart;
  }
  return currentTotal;
}

// unsigned long getTimeRemaining() {

//   return timeremaining;
// }

void goRight() {
  digitalWrite(MOTO_PIN1, HIGH);
  digitalWrite(MOTO_PIN2, LOW);
}


void goLeft() {
  digitalWrite(MOTO_PIN1, LOW);
  digitalWrite(MOTO_PIN2, HIGH);
}

void motoStop() {
  digitalWrite(MOTO_PIN1, LOW);
  digitalWrite(MOTO_PIN2, LOW);
}

void craneMotor() {

  digitalWrite(MOTO_PIN3, HIGH);
  digitalWrite(MOTO_PIN4, LOW);
  delay(2500);
  digitalWrite(MOTO_PIN3, LOW);
  digitalWrite(MOTO_PIN4, LOW);
  delay(7000);
  digitalWrite(MOTO_PIN3, LOW);
  digitalWrite(MOTO_PIN4, HIGH);
  delay(3750);
  digitalWrite(MOTO_PIN3, LOW);
  digitalWrite(MOTO_PIN4, LOW);

}

void logStatus(unsigned long currentTime) {
  static unsigned long lastLog = 0;
  
  if (currentTime - lastLog >= 1000) {
    lastLog = currentTime;
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.print("cm | Session: ");
    Serial.print(sessionActive ? "Active" : "Inactive");
    Serial.print(" | Total: ");
    Serial.print(getCurrentTotalTime() / 1000.0, 1);
    Serial.println("s");
  }
}