#include <Servo.h>

Servo servo;   
Servo positional_servo;  


int potentiometer_val = 0;       // Variable to store potentiometer value
//int pos_positional_servo = 0;      // Variable to store positional servo position
int buttonState = 0;  // Variable to store the button state
int lastButtonState = HIGH; // Start with HIGH since we're using INPUT_PULLUP

bool arm_locked = false;

void setup() {
  servo.attach(9);  // Attach the potentiometer-controlled servo to Pin 9
  positional_servo.attach(A1); // Attach the positional servo to Pin A1
  pinMode(8, INPUT_PULLUP); // Use internal pull-up resistor for button pin

  reset_arm();
}

void set_rubber_servo(int val) {
  // Set the potentiometer-controlled servo position
  servo.write(val);
}

void set_hold_servo(int val) {
  // Rotate the positional servo by 90 degrees each time the button is clicked
  positional_servo.write(val);  // Set the positional servo to the new position
  delay(200);  // Debounce delay

}

void release_rubber() {
  set_rubber_servo(0);
}

void reset_arm() {
  release_arm();
  delay(800);
  release_rubber();
  delay(800);
  hold_arm();

  delay(800);
  set_rubber_servo(potentiometer_val);
}

void toggle_hold_arm() {
  if (arm_locked)
    release_arm();
  else 
    hold_arm();
}

void hold_arm() {
  arm_locked = true;
  set_hold_servo(90);
}

void release_arm() {
  arm_locked = false;
  set_hold_servo(189);
}

void loop() {
  // Read the potentiometer value (0-1023)
  potentiometer_val = analogRead(A0);
  
  // Map potentiometer value (0-1023) to servo position (0-180)
  int potentiometer_new_val = map(potentiometer_val, 0, 1023, 0, 180);

  set_rubber_servo(potentiometer_new_val);


  // Read the button state (pressed or not)
  buttonState = digitalRead(8);

  // Check for button press (detect HIGH-to-LOW transition)
  if (buttonState == LOW && lastButtonState == HIGH){ 
    release_arm();
    delay(500);
    reset_arm();
  }

  // Update the last button state
  lastButtonState = buttonState;
}
