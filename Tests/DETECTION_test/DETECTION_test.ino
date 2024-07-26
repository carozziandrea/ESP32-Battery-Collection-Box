petconst int trigPin = 13;
const int echoPin = 14;

long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Imposta il pin Trigger come uscita
  pinMode(echoPin, INPUT);  // Imposta il pin Echo come ingresso
  Serial.begin(9600);       // Avvia la comunicazione seriale a 9600 baud
}

void loop() {
  // Pulisci il pin Trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Imposta il pin Trigger su HIGH per 10 microsecondi
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Leggi il tempo di ritorno del segnale Echo
  duration = pulseIn(echoPin, HIGH);
  
  // Calcola la distanza in cm
  distance = duration * 0.034 / 2;
  
  // Stampa la distanza sul monitor seriale
  Serial.print("Distanza: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Se la distanza è inferiore a una certa soglia, accendi il LED
  if (distance < 10.0) { // Puoi regolare questa soglia in base alla tua applicazione
    Serial.println("Pila rilevata!");
    delay(3000);
  } else {
  }

  // Piccolo ritardo prima della prossima lettura
  delay(20);
}
