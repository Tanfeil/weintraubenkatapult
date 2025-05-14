#include <Servo.h>

Servo servo;   
Servo positional_servo;  


int potentiometer_val = 0;       // Variable to store potentiometer value
int pos_positional_servo = 0;      // Variable to store positional servo position
int buttonState = 0;  // Variable to store the button state
int lastButtonState = HIGH; // Start with HIGH since we're using INPUT_PULLUP

void setup() {
  servo.attach(9);  // Attach the potentiometer-controlled servo to Pin 9
  positional_servo.attach(A1); // Attach the positional servo to Pin A1
  pinMode(8, INPUT_PULLUP); // Use internal pull-up resistor for button pin
}

void loop() {
  // Read the potentiometer value (0-1023)
  potentiometer_val = analogRead(A0);
  
  // Map potentiometer value (0-1023) to servo position (0-180)
  int potentiometer_new_val = map(potentiometer_val, 0, 1023, 0, 180);

  // Set the potentiometer-controlled servo position
  servo.write(potentiometer_new_val);

  // Read the button state (pressed or not)
  buttonState = digitalRead(8);

  // Check for button press (detect HIGH-to-LOW transition)
  if (buttonState == LOW && lastButtonState == HIGH){ 
    // Rotate the positional servo by 90 degrees each time the button is clicked
    pos_positional_servo += 90;
    if (pos_positional_servo > 90) {  // If it reaches 180, reset to 0
      pos_positional_servo = 0;
    }
    positional_servo.write(pos_positional_servo);  // Set the positional servo to the new position
    delay(200);  // Debounce delay to avoid multiple clicks registering
  }

  // Update the last button state
  lastButtonState = buttonState;
}
