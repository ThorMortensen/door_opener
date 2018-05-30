#include <Arduino.h>

#define BUTTON_PIN 2
#define OPEN_DOOR_RELAY_PIN 3
#define BUZZER_RELAY_PIN 4


#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define IS_BUTTON_HOLDED (isButtenPressed())
// (digitalRead(BUTTON_PIN) == HIGH)

#define CODE_TYPES(CODE_TYPE)\
            CODE_TYPE(SHORT_HOLD)\
            CODE_TYPE(LONG_HOLD)


typedef enum holdType_e{
  CODE_TYPES(GENERATE_ENUM)
  ALL_HOLD_TYPES
}holdType_t;

const char* codeTypeToStr[] = { CODE_TYPES(GENERATE_STRING) };

#define HOLD_TIME_LONG 1000
#define HOLD_TIME_SHORT 100

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(OPEN_DOOR_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_RELAY_PIN, OUTPUT);
  Serial.println("starting");
}

bool isButtenPressed(){
  uint16_t debounceMask = 0;
  for (uint8_t i = 0; i < 16; i++){
    debounceMask |= (digitalRead(BUTTON_PIN) << i);
    delay(1);
  }
  return debounceMask == 0xFFFF;
}

bool detectPress(holdType_t type){
  unsigned long timeStart = millis();
  while (digitalRead(BUTTON_PIN) == HIGH){}
  if (type == LONG_HOLD ? (millis() - timeStart) < HOLD_TIME_LONG : (millis() - timeStart) > HOLD_TIME_LONG){
    return false;
  }else {
    return true;
  }
}

bool checkCode(holdType_t* codeToCheck, size_t size){
  for (int i = 0; i < size; i++){
    unsigned long timeDelay = millis();
    while (!isButtenPressed()){
      if (millis() - timeDelay > 3000){
        return false;
      }
    }
    Serial.print("Should be ");
    Serial.print(codeTypeToStr[codeToCheck[i]]);
    Serial.println();
    if (!detectPress(codeToCheck[i])){
      Serial.println("Wrong code! try again");
      return false;
    }

  }
  Serial.println("Correct code");
  return true;
}

holdType_t accesCode[] = {
  LONG_HOLD,

  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,

  LONG_HOLD,

  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,

  LONG_HOLD,

  SHORT_HOLD,
  SHORT_HOLD,

  LONG_HOLD,

  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,
  SHORT_HOLD,

  LONG_HOLD

};


holdType_t startupCode[] = {
  LONG_HOLD,
  LONG_HOLD,
  SHORT_HOLD,
  SHORT_HOLD
};


void loop() {
  digitalWrite(BUZZER_RELAY_PIN, LOW);
  digitalWrite(OPEN_DOOR_RELAY_PIN, LOW);

  if (isButtenPressed()) {
    if (checkCode(startupCode,4)){
      Serial.println("Stop the noice!!");
      digitalWrite(BUZZER_RELAY_PIN, HIGH);
      if (checkCode(accesCode,26)){
        Serial.println("Open the gate!!");
        digitalWrite(OPEN_DOOR_RELAY_PIN, HIGH);
        delay(3000);
        digitalWrite(OPEN_DOOR_RELAY_PIN, LOW);
        Serial.println("Close the gate!!");
      }
    }
    Serial.println("Something is wrong wait for reset");
    delay(1000);
    Serial.println("RESET!");
  }
}
