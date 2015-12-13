// Pines correspondientes a los segmentos 'a' al 'g' del display
int displayPin[7] = { A2, 10, 8, 4, 7, A3, 12 };

//pin correspondiente e intensidad para pwm
byte rojo[] = {6, 64};
byte azul[] = {5, 16};
byte verde[] = {9, 32};

byte botones[] = {2, 3};

byte estado[] = {0, 0};
byte estadoAnt[] = {0, 0};

int buttonState[2];             // the current reading from the input pin
int lastButtonState[] = {LOW, LOW};   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime[] = {0, 0};  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int contador[] = {1, 0};
int unidades = 0;
int decenas = 0;

long ultimoDescenso = 0;

bool primerDescenso = 0;

bool parpadeo = 0;


// Inicialización
void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(displayPin[i], OUTPUT);
  }
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  pinMode(rojo, OUTPUT);
  pinMode(azul, OUTPUT);
  pinMode(verde, OUTPUT);

}

void comprobarBotones(int boton) {
  boton--; //se pide boton 1 o 2, restamos 1 para leer del array
  // read the state of the switch into a local variable:
  int reading = digitalRead(botones[boton]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[boton]) {
    // reset the debouncing timer
    lastDebounceTime[boton] = millis();
  }

  if ((millis() - lastDebounceTime[boton]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[boton]) {
      buttonState[boton] = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState[boton] == HIGH) {
        contador[boton] += 1;
      }
    }
  }
  lastButtonState[boton] = reading;
}

// Esta función (a completar) escribe el caracter 'val'
// en el display, en el digito 'dig' (0 derecha, 1 izquierda)
void writeDisplay(char val, int digit) {
  // Primero apagamos todos los segmentos del digito que estuviera activo
  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPin[i], LOW);
  }
  // Ahora activamos el digito deseado
  if (digit == 0) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
  }

  // Calculamos los segmentos que toca activar para mostrar 'val'
  // Para ello, codificaremos los segmentos activos con un byte (8 bits,
  // del 7 al 0) donde los bits 6..0 corresponden con los segmentos a..g
  char segments = 0; // Byte con los segmentos a activar (inicialmente ninguno)
  switch (val) {
    // abcdefg
    case '0': segments = 0b01111110; break; // segmentos del '0'
    case '1': segments = 0b00110000; break; // segmentos del '1'
    case '2': segments = 0b01101101; break; // segmentos del '2'
    case '3': segments = 0b01111001; break; // segmentos del '3'
    case '4': segments = 0b00110011; break; // segmentos del '4'
    case '5': segments = 0b01011011; break; // segmentos del '5'
    case '6': segments = 0b01011111; break; // segmentos del '6'
    case '7': segments = 0b01110000; break; // segmentos del '7'
    case '8': segments = 0b01111111; break; // segmentos del '8'
    case '9': segments = 0b01111011; break; // segmentos del '9'
    default : segments = 0b00000001; // si no es ninguno, ponemos un guion
  }
  // Y por último activamos los segmentos previamente obtenidos
  char currSegment = 0b01000000; // empezaremos por mirar el segmento 'a'
  for (int i = 0; i < 7; i++) {
    if ((currSegment & segments) != 0) { // si el bit del segmento es 1
      digitalWrite(displayPin[i], HIGH);
    }
    currSegment = currSegment >> 1; // desplazamos un lugar a derecha
  }
}

void write2Numbers(int cuenta) {

  unidades = (cuenta) % 10;
  decenas = (cuenta - unidades) / 10;

  writeDisplay(decenas + 48, 1);
  writeDisplay(unidades + 48, 0);

}

void luces(byte[2] color){
	analogWrite(color[0], color[1]);
}

void loop() {

  
  //polling del boton 2
  comprobarBotones(2);
  
  //apagamos la alarma aqui
    noTone(11);


  switch (contador[1]) {

    case 0:
      digitalWrite(verde, LOW); //apagamos luces anteriores
      write2Numbers(0);
      contador[0] = 1;
      break;

    case 1:
      comprobarBotones(1);

      //resetea el contador si se pasa de 99
      if (contador[0] > 100) {
        contador[0] = 0;
      }

      // el contador tiene 1 numero mas siempre para que en la cuenta atras no salte el primero
      write2Numbers(contador[0] - 1);
      
      analogWrite(rojo, 64); //leds con PWM para no deslumbrarnos (no los habia menos brillantes??)
      break;
      
    case 2:
      digitalWrite(rojo, LOW);
      analogWrite(azul, 16); //PWM menor por la mayor luminosidad del azul
      
      if (contador[0] > 0 && (millis() - ultimoDescenso) > 1000) {
        contador[0]--;
        ultimoDescenso = millis();
      }

      write2Numbers(contador[0]);

      if (contador[0] == 0) {
        contador[1] = 3;
      }

      break;

    case 3:
      digitalWrite(azul, LOW);
      analogWrite(verde, 32);
      if ((millis() - ultimoDescenso) < 1000 && parpadeo == 0) {
        tone(11, 1000); 
        write2Numbers(0, 0);
      } else if ((millis() - ultimoDescenso) < 1000 && parpadeo == 1) {
        write2Numbers(-1, -1); //caso default, salen rayitas
      } else {
        ultimoDescenso = millis();
        parpadeo = !parpadeo;
      }
      break;
      
      //reset del contador del BT2
    default:        
      contador[1] = 0;
      break;
  }
    

  }
}
