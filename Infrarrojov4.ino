#include <IRremote.h> // v 2.6.0

const int RECV_PIN = 11; // Pin donde está conectado el pin OUT del HX-M121
const int LED_PIN = 9;   // Pin donde está conectado el LED
IRrecv irrecv(RECV_PIN); // Crear objeto receptor IR
decode_results results;  // Objeto para almacenar resultados decodificados

// Códigos específicos
const unsigned long STORE_CODE = 0x61708C73; // Código para iniciar el almacenamiento
const unsigned long RESET_CODE = 0x6170946B; // Código para reiniciar almacenamiento
const unsigned long TOGGLE_CODE = 0x6170F807; // Código para alternar el LED
const unsigned long VERIFY_CODE = 0x6170708F; // Código para verificar la copia
const int MAX_CODES = 4; // Máximo de códigos a almacenar

// Lista de códigos válidos
unsigned long validCodes[] = {
  0x6170807F, 0x617040BF, 0x6170C03F, 0x617020DF, 
  0x6170A05F, 0x6170609F, 0x6170E01F, 0x617010EF, 
  0x6170906F, 0x617000FF
};

// Variables globales
unsigned long storedCodes[MAX_CODES] = {0}; // Array para almacenar los códigos (inicializado a 0)
unsigned long cstoredCodes[MAX_CODES] = {0};
int codeCount = 0;                          // Contador de códigos almacenados
bool storing = false;                       // Flag para saber si se están almacenando códigos
bool ledState = false;                      // Estado del LED
int Ccount = 0;                          // Contador de códigos almacenados
bool cstoring = false; 

void setup() {
  pinMode(LED_PIN, OUTPUT);   // Configura el pin del LED como salida
  digitalWrite(LED_PIN, LOW); // Asegúrate de que el LED comience apagado
  Serial.begin(9600);         // Inicia la comunicación serial
  irrecv.enableIRIn();        // Inicia el receptor IR
  Serial.println("Esperando señales IR...");
}

void loop() {
  if (irrecv.decode(&results)) { // Si recibe una señal IR
    unsigned long code = results.value;

    // Ignorar códigos repetidos (FFFFFFFF)
    if (code == 0xFFFFFFFF) {
      irrecv.resume();
      return;
    }

    Serial.print("Código recibido: ");
    Serial.println(code, HEX);

    if (code == STORE_CODE && codeCount < MAX_CODES) { // Iniciar almacenamiento solo si hay espacio
      storing = true;
      codeCount = 0; // Reinicia el contador de códigos almacenados
      Serial.println("Inicio de almacenamiento de códigos...");
    }
    

    else if (code == RESET_CODE) { // Reiniciar almacenamiento
      storing = false;
      codeCount = 0; // Limpia los códigos almacenados
      for (int i = 0; i < MAX_CODES; i++) storedCodes[i] = 0; // Borra el array
      Serial.println("Códigos almacenados reiniciados.");
    } 
    else if (code == TOGGLE_CODE) { // Alternar LED
      Serial.println("Código para alternar LED rápidamente recibido");
      for (int i = 0; i < 6; i++) { // Alterna el LED 5 veces
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        delay(200);
      }
    }
    else if (code == VERIFY_CODE ) { // Verificar copia
      cstoring = true;
      Ccount = 0; // Reinicia el contador de códigos almacenados
      Serial.println("Comprobar Contraseña");
    } 
    else if (cstoring && Ccount < MAX_CODES) { // Almacenar códigos
      if (isValidCode(code)) {
        cstoredCodes[Ccount++] = code; // Guarda el código en la posición actual
        Serial.print("Código almacenado: ");
        Serial.println(code, HEX);
        if (Ccount == MAX_CODES) {
          cstoring = false; // Detiene el almacenamiento si ya se han guardado 4 códigos
          Serial.println("Se han almacenado los 4 códigos.");
          Comprobador(); // Verifica si el LED debe encenderse o apagarse
        }
      } else {
        Serial.println("Código inválido, ignorado.");
      }
    }
    else if (storing && codeCount < MAX_CODES) { // Almacenar códigos
      if (isValidCode(code)) {
        storedCodes[codeCount++] = code; // Guarda el código en la posición actual
        Serial.print("Código almacenado: ");
        Serial.println(code, HEX);
        if (codeCount == MAX_CODES) {
          storing = false; // Detiene el almacenamiento si ya se han guardado 4 códigos
          Serial.println("Se han almacenado los 4 códigos.");
          toggleLedBasedOnCodes(); // Verifica si el LED debe encenderse o apagarse
        }
      } else {
        Serial.println("Código inválido, ignorado.");
      }
    }

    irrecv.resume(); // Prepara para recibir el próximo código
  }
}

// Verifica si el código recibido es válido
bool isValidCode(unsigned long code) {
  for (int i = 0; i < sizeof(validCodes) / sizeof(validCodes[0]); i++) {
    if (validCodes[i] == code) {
      return true;
    }
  }
  return false;
}

// Determina el estado del LED basado en los códigos almacenados
void toggleLedBasedOnCodes() {
  Serial.println("Verificando códigos almacenados para alternar el LED...");
  for (int i = 0; i < MAX_CODES; i++) {
    Serial.print("Código almacenado: ");
    Serial.println(storedCodes[i], HEX);
  }
  // Implementa tu lógica personalizada para encender/apagar el LED
  // Por ahora, simplemente alterna el estado
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  Serial.println(ledState ? "Encendiendo el LED basado en códigos" : "Apagando el LED basado en códigos");
}

void Comprobador() {
  Serial.println("Verificando Contraseña...");
  for (int i = 0; i < MAX_CODES; i++) {
    if(storedCodes[i] != cstoredCodes[i]){
      //Serial.println(storedCodes[i], HEX);
      //Serial.println("|");
      //Serial.println(cstoredCodes[i], HEX);
      Serial.println("Ocurrio un error...");
      return;
    }
  }
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  Serial.println(ledState ? "Encendiendo el LED basado en códigos" : "Apagando el LED basado en códigos");
}

