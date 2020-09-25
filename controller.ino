int ECPin= A0;
int ECGround=A1;
int ECPower =A4;
float Vin = 4.95;
int R1 = 200; // kOhm
int num_samples = 100;
int period = 2960; //+ 244us = 3200us
double A = 2.63;
double offset = 3.95;

// RsxVol% Curve coefs 
const float a = 0.25579478;
const float b = 0.24802099;
const float c = 7.98099304;
double e = 2.718281828459045235360287471352; //euler constant

void setup() {
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);
  pinMode(ECGround,OUTPUT);
  digitalWrite(ECGround,LOW);
  digitalWrite(ECPower,LOW);
  Serial.begin(9600);
  Serial.println("Start Serial Comm");
  Serial.print("Using R1: ");
  Serial.println(R1);
  
}

void loop() {
  unsigned long time1;
  unsigned long time2;
  time1 = micros(); 
  double raw = 0;
  double inv_raw = 0;
  float Vdrop, VdropA, VdropB, VdropA_r, VdropB_r, Vdrop_inv;
  bool flag_cycle = true;
  for(int i = 0; i < num_samples; i++)
  {
    if (flag_cycle == true) {
      time1 = micros();
      digitalWrite(ECPower,HIGH);
      delayMicroseconds((9*period)/10);
      float holder = analogRead(ECPin);
      raw = raw + analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
      delayMicroseconds(period/10);
      digitalWrite(ECPower,LOW);
      time2 = micros();
      flag_cycle = false;
    } else {
      digitalWrite(ECGround,HIGH);
      delayMicroseconds(period/2);
      float holder = analogRead(ECPin);
      int inv_read;
      inv_read = analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
      inv_raw = inv_raw + inv_read;
      delayMicroseconds(period/2);
      digitalWrite(ECGround,LOW);
      
      flag_cycle = true;
    }
  }
  
  raw = raw/(num_samples/2);
  inv_raw = inv_raw/(num_samples/2);
  
  VdropA = (Vin*raw)/1024.0;
  VdropB = (Vin*inv_raw)/1024.0;

  VdropA_r = (VdropA + offset)/A;
  VdropB_r = (VdropB + offset)/A;
  
  
  double Rs, RsA, RsB, Rs_r, RsA_r, RsB_r;
  RsA = (R1*VdropA)/(Vin-VdropA);
  RsB = (R1/VdropB)*(Vin-VdropB);
  Rs = (RsA + RsB)/2;
  
  RsA_r = (R1*VdropA_r)/(Vin-VdropA_r);
  RsB_r = (R1/VdropB_r)*(Vin-VdropB_r);
  Rs_r = (RsA_r + RsB_r)/2;
  
  bool debug = false;
  if (Serial.available()) {
    if (debug == true) {
      Serial.print("VdropA: ");
      Serial.println(VdropA);
      Serial.print("VdropB: ");
      Serial.println(VdropB);
      Serial.print("VdropA_r: ");
      Serial.println(VdropA_r);
      Serial.print("VdropB_r: ");
      Serial.println(VdropB_r);
      Serial.print("RsA_r: ");
      Serial.println(RsA_r);
      Serial.print("RsB_r: ");
      Serial.println(RsB_r);
      Serial.print("Rs_r: ");
      Serial.println(Rs_r);
      Serial.print("RsA: ");
      Serial.println(RsA);
      Serial.print("RsB: ");
      Serial.println(RsB);
      Serial.print("Rs: ");
      Serial.println(Rs);
    }
    double vol_eth = a + b*pow(e, -c*(Rs_r/1000));
    Serial.print("Vol. Etanol: ");
    Serial.println(vol_eth*100,2);
    if (vol_eth >= 0.2828) {
      Serial.println("GASOLINA ADULTERADA!");
    } else {
      Serial.println("GASOLINA NÃO ADULTERADA");
    }
    Serial.read();
  }
}
