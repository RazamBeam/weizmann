const int hallPins[] = {A4, A3, A2, A1, A0, 2, 3, 4, 5, 6, 7, 8, 12, 11, 10, 9}; // Array of hall sensor pins
const int numPins = 16; // Number of pins in the array

String prev = "";
String line = "";

void setup() {
  Serial.begin(9600);

  // Set all hall pins as input WITH internal pullup
  for (int i = 0; i < numPins; i++) {
    pinMode(hallPins[i], INPUT_PULLUP);
  }
}

void loop() {
  line = "";
  // Read values of all hall pins and construct the line
  for (int i = 0; i < numPins; i++) {
    int hallValue = digitalRead(hallPins[i]);
    line += String(hallValue); // Append each hall sensor value to the string
  }

  // Send the entire line at once, followed by a newline character
  if (line != prev) {
    Serial.println(line);
    prev = line;
  }
  
  delay(10);
}
