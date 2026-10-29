#include <LiquidCrystal.h>

// ---------------- LCD ----------------
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(25, 26, 27, 16, 17, 18);

// ---------------- NTC ----------------
const int pinoNTC = 34;

const float R0 = 10000.0;       // NTC 10K
const float Rfixo = 10000.0;    // resistor de 10K

// Temperaturas usadas na calibração
const float T1 = 273.15;        // 0 °C
const float T2 = 373.15;        // 100 °C
const float T0 = 298.15;        // 25 °C

// Resistências medidas
const float RT1 = 35563.0;      // resistência a 0 °C
const float RT2 = 549.0;        // resistência a 100 °C

float beta;
float Rinf;

float temperaturaFiltrada = 25.0;

void setup() {

  Serial.begin(115200);
  analogReadResolution(12);

  // Calcula o beta do NTC
  beta = log(RT1 / RT2) /
         ((1.0 / T1) - (1.0 / T2));

  // Calcula Rinf
  Rinf = R0 * exp(-beta / T0);

  // Inicializa LCD
  lcd.begin(16, 2);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");

  delay(1000);
}

void loop() {

  long soma = 0;
  //Filtro
  for (int i = 0; i < 32; i++) {
    soma += analogRead(pinoNTC);
    delay(2);
  }

  float adc = soma / 32.0;


  float tensao = adc * 3.3 / 4095.0;
  //Resistência do NTC
  float Rterm = Rfixo * (3.3 - tensao) / tensao;

  //Calcular Temperatura em Kelvin
  float TempK = beta / log(Rterm / Rinf);
  //Calcular Temperatura em Celsius
  float TempC = TempK - 273.15;

  //Filtro de chute inicial perto
  temperaturaFiltrada =
      0.9 * temperaturaFiltrada +
      0.1 * TempC;

  //Configuração do LCD
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:    ");

  lcd.setCursor(0, 1);
  lcd.print(temperaturaFiltrada, 2);
  lcd.write(223);
  lcd.print("C       ");

  Serial.print("ADC: ");
  Serial.print(adc);

  Serial.print(" | Tensao: ");
  Serial.print(tensao, 3);

  Serial.print(" V | RNTC: ");
  Serial.print(Rterm, 1);

  Serial.print(" ohm | Temperatura: ");
  Serial.print(temperaturaFiltrada, 2);

  Serial.println(" C");

  delay(500);
}