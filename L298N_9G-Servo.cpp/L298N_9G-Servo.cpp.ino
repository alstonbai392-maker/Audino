// ===================================================================
// Dual DC Motor + 9g Servo Control (L298N + SG90/Micro servo)
// Arduino Uno/Nano/Mega compatible
// ===================================================================

#include <Servo.h>                     // Include Servo library

// Motor A connections (L298N or similar driver)
int enA = 9;
int in1 = 8;
int in2 = 7;

// Motor B connections
int enB = 3;
int in3 = 5;
int in4 = 4;

// Servo connection
int servoPin = 10;                     // Change if you use another pin (must be PWM-capable)
Servo myServo;                         // Create servo object
int currentAngle = 90;                 // Current servo position (default center)
const int HOME_ANGLE = 90;             // Define home/center position

// Variables for non-blocking servo sweep demo
unsigned long previousMillis = 0;
int sweepAngle = 0;
int sweepDirection = 1;                // 1 = increasing, -1 = decreasing
bool sweeping = false;

void setup() {
  // ------ DC Motor pins ------
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Turn off motors initially
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);

  // ------ Servo setup ------
  myServo.attach(servoPin);            // Attach servo to pin
  myServo.write(HOME_ANGLE);           // Move to center position at startup
  delay(500);                          // Short settle time
  currentAngle = HOME_ANGLE;

  // ------ Serial communication ------
  Serial.begin(9600);
  while (!Serial) { ; }                // Wait for serial port (useful on some boards)

  Serial.println(F("=== Dual Motor + Servo Control Ready ==="));
  Serial.println(F("DC Motor commands:"));
  Serial.println(F("   F<speed>  → Forward  (0-255)"));
  Serial.println(F("   R<speed>  → Reverse  (0-255)"));
  Serial.println(F("   S0        → Stop"));
  Serial.println(F("\nServo commands (9g micro servo on pin ") 
                 + String(servoPin) + F("):"));
  Serial.println(F("   P<angle>  → Set position 0-180°   (e.g. P0, P90, P180)"));
  Serial.println(F("   PH        → Return to home (90°)"));
  Serial.println(F("   PS        → Start/Stop continuous sweep demo"));
  Serial.println();
}

void loop() {
  // Handle non-blocking servo sweep if enabled
  if (sweeping) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 15) {   // Update every 15 ms → smooth motion
      previousMillis = currentMillis;
      sweepAngle += sweepDirection * 2;           // Step size (adjust for speed)
      if (sweepAngle >= 180 || sweepAngle <= 0) {
        sweepDirection = -sweepDirection;         // Reverse direction
        sweepAngle = constrain(sweepAngle, 0, 180);
      }
      myServo.write(sweepAngle);
      currentAngle = sweepAngle;
    }
  }

  // Process incoming serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() == 0) return;

    Serial.print(F("Received: "));
    Serial.println(command);

    char firstChar = toupper(command.charAt(0));

    // ====================== DC MOTOR COMMANDS ======================
    if (firstChar == 'F' || firstChar == 'R' || firstChar == 'S') {
      int speed = 0;
      if (command.length() > 1) {
        speed = command.substring(1).toInt();
      }
      speed = constrain(speed, 0, 255);

      if (firstChar == 'F') {
        setMotorDirection(HIGH, LOW, speed);
        Serial.println("Motors → Forward, Speed: " + String(speed));
      }
      else if (firstChar == 'R') {
        setMotorDirection(LOW, HIGH, speed);
        Serial.println("Motors → Reverse, Speed: " + String(speed));
      }
      else if (firstChar == 'S') {
        setMotorDirection(LOW, LOW, 0);
        Serial.println("Motors → Stopped");
      }
    }

    // ====================== SERVO COMMANDS ======================
    else if (firstChar == 'P') {
      if (command.length() > 1) {
        int angle = command.substring(1).toInt();
        angle = constrain(angle, 0, 180);
        myServo.write(angle);
        currentAngle = angle;
        Serial.println("Servo → " + String(angle) + "°");
      } else {
        Serial.println("Error: Missing angle for P command (0-180)");
      }
    }
    else if (command.equalsIgnoreCase("PH")) {
      myServo.write(HOME_ANGLE);
      currentAngle = HOME_ANGLE;
      Serial.println("Servo → Home position (" + String(HOME_ANGLE) + "°)");
    }
    else if (command.equalsIgnoreCase("PS")) {
      sweeping = !sweeping;
      if (sweeping) {
        Serial.println("Servo sweep demo STARTED");
        sweepAngle = currentAngle;
        sweepDirection = 1;
        previousMillis = millis();
      } else {
        Serial.println("Servo sweep demo STOPPED");
      }
    }

    // ====================== UNKNOWN COMMAND ======================
    else {
      Serial.println("Error: Unknown command");
      Serial.println("Use: F<speed>, R<speed>, S0, P<angle>, PH, PS");
    }
  }
}

// ===================================================================
// Function to control both DC motors (direction + speed)
// ===================================================================
void setMotorDirection(int in1State, int in2State, int speed) {
  digitalWrite(in1, in2State);
  digitalWrite(in2, in1State);
  digitalWrite(in3, in1State);
  digitalWrite(in4, in2State);

  analogWrite(enA, speed);
  analogWrite(enB, speed);

  Serial.println("Pin states → in1:" + String(in1State) +
                 " in2:" + String(in2State) +
                 " in3:" + String(in1State) +
                 " in4:" + String(in2State) +
                 " | PWM A:" + String(speed) +
                 " B:" + String(speed));
}