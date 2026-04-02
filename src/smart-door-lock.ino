#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

//Pin & hardware setup
Servo lockServo;
const int LED_PIN   = 11;
const int SERVO_PIN = 10;

//Keypad layout (4x4 matrix)
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Password state
const int PASS_LEN = 4;
String password = "";   // loaded from EEPROM at startup
String input    = "";   // what the user is currently typing

// ════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  lockServo.attach(SERVO_PIN);
  lockServo.write(0);       // locked position
  loadPassword();
  Serial.println("System Ready. Enter code + # to unlock, A to change password.");
}

// ════════════════════════════════════════════════════════════════
void loop() {
  char key = keypad.getKey();
  if (!key) return;

  Serial.print(key);

  if (key == '*') {                   // clear current input
    input = "";
    Serial.println(" [Cleared]");

  } else if (key == '#') {            // submit password
    checkPassword();

  } else if (key == 'A') {           // change password mode
    changePassword();

  } else if (input.length() < PASS_LEN) {  // BUG FIX: cap input at 4 chars
    input += key;
  }
}

//Unlock if password matches
void checkPassword() {
  if (input == password) {
    Serial.println(" ACCESS GRANTED");
    digitalWrite(LED_PIN, HIGH);
    lockServo.write(90);    // open
    delay(5000);
    lockServo.write(0);     // lock again
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println(" WRONG PASSWORD");
  }
  input = "";
}

//Collect exactly 4 digit-only keypresses
String collectCode(String prompt) {
  Serial.println(prompt);
  String code = "";
  while (code.length() < PASS_LEN) {
    char key = keypad.getKey();
    // BUG FIX: only accept digit keys (0-9), ignore *, #, A-D
    if (key && key >= '0' && key <= '9') {
      Serial.print('*');    // mask input for security
      code += key;
    }
  }
  Serial.println();
  return code;
}

//Change password: verify old one first
void changePassword() {
  String oldInput = collectCode("\nEnter OLD password:");
  if (oldInput != password) {
    Serial.println("Wrong password. Cancelled.");
    input = "";
    return;
  }
  String newPass = collectCode("Enter NEW 4-digit password:");
  savePassword(newPass);
  password = newPass;
  Serial.println("Password updated!");
  input = "";
}

//EEPROM helpers
void savePassword(String pass) {
  for (int i = 0; i < PASS_LEN; i++) {
    EEPROM.write(i, pass[i]);
  }
}

void loadPassword() {
  password = "";
  bool valid = true;
  for (int i = 0; i < PASS_LEN; i++) {
    char c = char(EEPROM.read(i));
    // BUG FIX: if EEPROM is blank (0xFF) or non-digit, use default password
    if (c < '0' || c > '9') { valid = false; break; }
    password += c;
  }
  if (!valid) {
    password = "1234";
    savePassword(password);
    Serial.println("No saved password found. Default set to: 1234");
  }
}
