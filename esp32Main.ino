#include <MD_MAX72xx.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Define RX/TX pins
#define RXp2 25
#define TXp2 26
#define RXp1 16
#define TXp1 17
#define BUTTON_PIN 14

String temp1 = "";
String temp2 = "";
String message1 = "";
String message2 = "";
unsigned int whites = 0x0000000F, blacks = 0xF0000000, kings = 0;
bool whiteTurn = true;
unsigned int targetChange = 0; // 0 means no target
int count = 0;

// Define hardware SPI pins
#define DATA_PIN 22
#define CLK_PIN 5
#define CS_PIN 23

// Hardware type and number of devices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Create an instance of the MD_MAX72XX class
MD_MAX72XX mx(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN);


unsigned int reverseBits(unsigned int n) {
    unsigned int result = 0;

    for (int i = 0; i < 32; ++i) {
        result <<= 1;         // Shift result left
        result |= (n & 1);    // Copy the lowest bit from n to result
        n >>= 1;              // Shift n right
    }

    return result;
}


// Function to display a 64-bit integer on the 8x8 LED matrix
void displayBits(uint64_t bits) {
    for (uint8_t row = 0; row < 8; row++) {
        uint8_t rowData = (bits >> (row * 8)) & 0xFF; // Extract 8 bits for the current row
        mx.setRow(0, row, rowData);
    }
}

String reverse(String str) {
    String reversed = "";
    for (int i = str.length() - 1; i >= 0; i--) {
        reversed += str[i];
    }
    return reversed;
}

uint64_t strToBin(String s) {
  uint64_t result = 0;

  for (uint8_t i = 0; i < s.length(); i++) {
    char c = s[i];

    if (c == '1')
      result = (result << 1) | 1;
    else
      result <<= 1;
  }

  return result;
}


uint64_t mapToCheckerboard(uint64_t blackOccupied) {
    uint64_t checkerboard = 0;
    uint8_t blackTileIndex = 0;

    // Loop over all 64 positions on the checkerboard
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {
            // Determine if this is a black tile
            if ((row + col) % 2 == 1) {
                // Check if the corresponding black tile is occupied
                if ((blackOccupied >> blackTileIndex) & 1) {
                    // Set the corresponding bit in the checkerboard representation
                    checkerboard |= (1ULL << (row * 8 + col));
                }
                blackTileIndex++; // Move to the next black tile
            }
        }
    }

    return checkerboard;
}


String toArg(uint32_t whites, uint32_t blacks, uint32_t kings) {
    String arg = "";
    uint32_t p = 1;
    
    for (int i = 0; i < 32; ++i) {
        if (p & whites) {
            arg += (p & kings) ? 'W' : 'w';
        } else if (p & blacks) {
            arg += (p & kings) ? 'B' : 'b';
        } else {
            arg += '.';
        }
        p <<= 1;
    }
    
    return arg;
}


uint64_t transpose(uint64_t input) {
    uint64_t output = 0;
    
    // Iterate through each position in the 8x8 matrix
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            // Get the bit at position [row][col]
            uint64_t bit = (input >> (row * 8 + col)) & 1ULL;
            
            // Place it in the transposed position [col][row]
            output |= (bit << (col * 8 + row));
        }
    }
    
    return output;
}


unsigned int StringToUnsignedInt(String s) {
    unsigned int result = 0;

    for (int i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        } else {
            break; // Stop at first non-digit character
        }
    }

    return result;
}



int countBits(uint64_t n) {
  int res = 0;
  while (n) {
    res += n&1;
    n >>= 1;
  }
  return res;
}

bool updateIfValid(uint32_t b) {
  uint32_t added = (~(whites | blacks)) & b;
  
  if (countBits(added) != 1)
    return false;

  uint32_t removed = (whites | blacks) & (~b);

  if (whiteTurn) {
    
    if (countBits(removed & whites) != 1)
      return false;
    
    uint32_t whiteChange = (removed & whites) | added;

    // if the moved piece was a king
    if (whiteChange & kings) {
      kings ^= whiteChange;
    }

    // remove the eaten pieces
    blacks ^= removed & (~whites);

    // move the piece
    whites ^= whiteChange;
  }

  else {
    if (countBits(removed & blacks) != 1)
      return false;
    
    uint32_t blacksChange = (removed & blacks) | added;

    // if the moved piece was a king
    if (blacksChange & kings) {
      kings ^= blacksChange;
    }

    // remove the eaten pieces
    whites ^= removed & (~blacks);

    // move the piece
    blacks ^= blacksChange;
  }

  whiteTurn = !whiteTurn; // change turn
  kings &= whites|blacks; // remove king flag from empty spots

  kings |= (whites & 0xF0000000) | (blacks & 0x0000000F); // update newly made kings

  return true;
}



// WiFi credentials
String ssid = "checkers";
String password = "checkers";
String serverURL = "http://93.173.116.228:25565";


void connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

unsigned int fetchWebsite(String args) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURL + args);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String response = http.getString();
            return StringToUnsignedInt(response);
        } else {
            Serial.print("Error on HTTP request: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected!");
    }
    return 0;
}



void setup() {
  Serial.begin(115200);  // Serial monitor for debugging

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize Serial1 and Serial2
  Serial1.begin(9600, SERIAL_8N1, RXp1, TXp1);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  mx.begin();          // Initialize the matrix
  mx.control(MD_MAX72XX::INTENSITY, 0); // Set brightness (0-15)
  mx.clear();          // Clear the display


  connectWiFi();
}

void loop() {
  // Check Serial1 for data and handle it if available
  if (Serial1.available() > 0) {
    // Read data byte by byte until a newline or carriage return is encountered
    while (Serial1.available() > 0) {
      char byte = Serial1.read();

      if (byte != '1' && byte != '0' && byte != '\n' && byte != '\r')
        continue;

      if (byte == '\n' || byte == '\r') {
        if (temp1.length() == 16)
          message1 = temp1;
        temp1 = "";
        break;
      }

      temp1 += byte;
    }
  }

  // Check Serial2 for data and handle it if available
  if (Serial2.available() > 0) {
    // Read data byte by byte until a newline or carriage return is encountered
    while (Serial2.available() > 0) {
      char byte = Serial2.read();

      if (byte != '1' && byte != '0' && byte != '\n' && byte != '\r')
        continue;

      if (byte == '\n' || byte == '\r') {
        if (temp2.length() == 16)
          message2 = temp2;
        temp2 = "";
        break;
      }

      temp2 += byte;
    }
  }


  //Serial.println(~(strToBin(reverse(message1 + message2))) & 0xFFFFFFFF);
  unsigned int board = ~strToBin(reverse(message1 + message2)) & 0xFFFFFFFF;

  // if the button is pressed and the move is valid
  if (digitalRead(BUTTON_PIN) == LOW) {
    //Serial.println("bottun pressed");

    if (board != (whites|blacks)) { // if the board changed
      if (updateIfValid(board)) { // if the change was vaid, update the board and -
        Serial.println("board updated 1");
        Serial.println(toArg(whites, blacks, kings));
        String args = String("/?args=") + (whiteTurn ? "w," : "b,") + toArg(whites, blacks, kings);
        Serial.println(args);
        targetChange = fetchWebsite(args);
        Serial.println(targetChange);
        Serial.println((whiteTurn ? "w," : "b,") + toArg(whites, blacks, kings));
        Serial.println(toArg(whites, blacks, kings));
      } else { // the change wasn't valid
        displayBits(0x8142241818244281L);
      }
    }
  } else {
    unsigned int targetBoard = (whites|blacks)^targetChange;
    if (targetChange != 0) { // if there is a target mening the user is supposed to move for the bot
      if (board == targetBoard) { // if the user moved for the bot correctly
        updateIfValid(board);
        targetChange = 0;
        Serial.println("board updated 2");
        Serial.println(toArg(whites, blacks, kings));
      } else { // the user has not yet moved for the bot
        if (count++ % 100 > 50)
          displayBits(transpose(mapToCheckerboard(reverseBits(targetBoard))));
        else
          displayBits(transpose(mapToCheckerboard(reverseBits(whites|blacks))));
      }
    } else { // there is no target currently
      displayBits(transpose(mapToCheckerboard(~strToBin(message1 + message2))));
    }
  }

  // Optional: You can add a delay if needed to reduce CPU usage or for timing
  delay(10);  // Delay to prevent high CPU usage
}
