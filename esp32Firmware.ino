#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
#define ELM_PORT SerialBT
#define DEBUG_PORT Serial

#define red 13
#define green 14
#define blue 12
#define buzzer 33


esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;  // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role = ESP_SPP_ROLE_SLAVE;   // or ESP_SPP_ROLE_MASTER

uint8_t Address[6] = { 0x00, 0x10, 0xcC, 0x4F, 0x36, 0x03 };

int HexToDec(char* n);

void setup() {

  delay(2000);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzzer, OUTPUT);

  DEBUG_PORT.begin(115200);
  // SerialBT.setPin("1234");
  ELM_PORT.begin("ESP32", true);

  if (!ELM_PORT.connect(Address, 2, sec_mask, role)) {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
    while (1)
      ;
  }
  DEBUG_PORT.println("Connected to OBDII");
}

void loop() {

  String buff = "";
  delay(2000);
  ELM_PORT.flush();
  ELM_PORT.println("0105");

  if (ELM_PORT.available()) {

    Serial.print("rx: ");
    while (ELM_PORT.available()) {
      int c = ELM_PORT.read();
      if (c >= 0) {        
        buff += (char)c;     
      }
    }
    Serial.println(buff);
  }

  buff.replace(" ", NULL);
  buff.replace("\r", NULL);
  buff.replace("\n", NULL);
  buff.replace("\t", NULL);
  //010541055A

  Serial.println(buff.c_str()); 
  
  if (buff.length() == 11) { //count + '\0'
   
    int temperature = GetCoolantTemperature(buff);

    Serial.print("Engine temperature: ");
    Serial.println(temperature);

    if (temperature < 90) {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(blue, LOW);
    } else if (temperature < 95) {
      digitalWrite(red, HIGH);
      digitalWrite(green, HIGH);
      digitalWrite(blue, LOW);
    } else if (temperature < 100){
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
	  }
	  else{
	  digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      digitalWrite(buzzer, HIGH);
	  }
    
  }else{
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      digitalWrite(buzzer, LOW);
  }
}

int GetCoolantTemperature(String dataFromELM) {

  int A = HexToDec(&dataFromELM.substring(dataFromELM.length() - 3)[0]);

  Serial.println(A-40);
  return A - 40;
}


int HexToDec(char* n) {

  Serial.println(n);

  char cA = n[0];
  char cB = n[1];

  int dec = 0;

  switch (cA) {
    case 'A':
      dec += 10 * 16;
      break;
    case 'B':
      dec += 11 * 16;
      break;
    case 'C':
      dec += 12 * 16;
      break;
    case 'D':
      dec += 13 * 16;
      break;
    case 'E':
      dec += 14 * 16;
      break;
    case 'F':
      dec += 15 * 16;
      break;
    default:
      dec += (cA - '0') * 16;
      
      break;
  }

  switch (cB) {
    case 'A':
      dec += 10;
      break;
    case 'B':
      dec += 11;
      break;
    case 'C':
      dec += 12;
      break;
    case 'D':
      dec += 13;
      break;
    case 'E':
      dec += 14;
      break;
    case 'F':
      dec += 15;
      break;
    default:
      dec += (cB - '0');
      break;
  }
  return dec;
}
