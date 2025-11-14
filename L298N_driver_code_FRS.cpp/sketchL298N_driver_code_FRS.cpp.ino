// Motor A connections
int enA = 9;
int in1 = 8;
int in2 = 7;
// Motor B connections
int enB = 3;
int in3 = 5;
int in4 = 4;

void setup() {
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  
  // Initialize Serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for Serial to connect (useful for some boards)
  }
  Serial.println("Motor Control Ready");
  Serial.println("Commands: F<speed> (Forward), R<speed> (Reverse), S0 (Stop)");
  Serial.println("Example: F255, R128, S0 (speed 0-255)");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.println("Received: " + command); // Debug: show received command
    
    if (command.length() >= 2) {
      char direction = toupper(command.charAt(0)); // Convert to uppercase
      int speed = command.substring(1).toInt();
      
      // Validate speed
      speed = constrain(speed, 0, 255);
      
      // Process command
      switch (direction) {
        case 'F':
          setMotorDirection(HIGH, LOW, speed);
          Serial.println("Forward, Speed: " + String(speed));
          break;
          
        case 'R':
          setMotorDirection(LOW, HIGH, speed);
          Serial.println("Reverse, Speed: " + String(speed));
          break;
          
        case 'S':
          setMotorDirection(LOW, LOW, 0);
          Serial.println("Stopped");
          break;
          
        default:
          Serial.println("Error: Invalid direction. Use F, R, or S");
          break;
      }
    } else {
      Serial.println("Error: Invalid format. Use F<speed>, R<speed>, or S0");
    }
  }
}

void setMotorDirection(int in1State, int in2State, int speed) {
  // Set direction for both motors
  digitalWrite(in1, in2State);
  digitalWrite(in2, in1State);
  digitalWrite(in3, in1State);
  digitalWrite(in4, in2State);
  
  // Set speed for both motors
  analogWrite(enA, speed);
  analogWrite(enB, speed);
  
  // Debug: confirm pin states
  Serial.println("Pins: in1=" + String(in1State) + ", in2=" + String(in2State) +
                 ", in3=" + String(in1State) + ", in4=" + String(in2State) +
                 ", enA=" + String(speed) + ", enB=" + String(speed));
}