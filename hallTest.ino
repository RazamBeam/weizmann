const int hallPins[] = {2, 3, 4, 5};  // Array of hall sensor pins
const int numPins = 4;          // Number of pins in the array

void setup() {
  Serial.begin(9600);
  
  // Set all hall pins as input
  for (int i = 0; i < numPins; i++)
    pinMode(hallPins[i], INPUT);
}

void loop() {
  // Read and print values of all hall pins in one line
  for (int i = 0; i < numPins; i++) {
    int hallValue = digitalRead(hallPins[i]);
    Serial.print(hallValue);
    Serial.print(" ");
  }

  Serial.println();  // Move to the next line after printing all values
}
