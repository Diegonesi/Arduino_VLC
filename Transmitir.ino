#include <IRremote.h>

// Pin del transmisor IR
#define IR_PIN 3

// Inicialización de los códigos válidos
unsigned long validCodes[] = {
  0x6170807F, 0x617040BF, 0x6170C03F, 0x617020DF, 
  0x6170A05F, 0x6170609F, 0x6170E01F, 0x617010EF, 
  0x6170906F, 0x617000FF
};

// Número total de códigos
const int numCodes = sizeof(validCodes) / sizeof(validCodes[0]);

// Inicializar el transmisor IR
IRsend irsend;

void setup() {
  // Configurar el pin del transmisor IR
  irsend.enableIROut(38); // Frecuencia de 38 kHz
  Serial.begin(9600);
  Serial.println("Transmisor IR listo.");
}

void loop() {
  for (int i = 0; i < numCodes; i++) {
    Serial.print("Enviando código: ");
    Serial.println(validCodes[i], HEX);

    // Enviar el código IR usando el protocolo NEC
    irsend.sendNEC(validCodes[i], 32); // Código de 32 bits

    // Esperar 2 segundos antes de enviar el siguiente código
    delay(2000);
  }
}

