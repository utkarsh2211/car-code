int acc_ped1, acc_ped2, acc_avg , brake_ped, t ;
bool curr_error, prev_error, curr_error_24, prev_error_24 ;
bool error = true;
//digital pins
int ok_24 = 2;
int interlock_out = 3;
int throttle_sw_out = 4;
int acc_sig_out = 5;
int brake_sig_out = 6;
int prech_relay_cont_sig_out = 7;
int mains_relay_cont_sig_out = 8;
int buzzer_cont_out = 9;
int tsal_ok_in = 10;
int cont_relay_cont_sig_out = 11;
int addi_sw_in = 12;
  
//inputs
int brake_ped_in = A1;
int acc_ped1_in = A2;
int acc_ped2_in = A3;
int hv_in = A0;

//deviation of more than 10% acc_pedal travel
void error_check(){
    acc_ped1 = analogRead(acc_ped1_in);
    acc_ped2 = analogRead(acc_ped2_in);
    brake_ped = analogRead(brake_ped_in);
    if (acc_ped1>0.1*1023&&acc_ped1<0.9*1023&&acc_ped2>0.1*1023&&acc_ped2<0.9*1023&&brake_ped>0.1*1023&&brake_ped<0.9*1023)
    {
        if(abs(acc_ped1 - acc_ped2) < 0.1*0.8*1023) //10% implausibility of torque encoders 
        {
          error = false;
          curr_error = error;
          digitalWrite(cont_relay_cont_sig_out, HIGH);
          if ((!curr_error && prev_error ) || (prev_error_24 && !curr_error_24)){
              precharge();
          }
          digitalWrite(throttle_sw_out,LOW);     

        }
    }   
    else {
       error = true;
       curr_error = error;           
       digitalWrite(throttle_sw_out,HIGH );
       digitalWrite(cont_relay_cont_sig_out, LOW); 
      }
    brake_implausibility();  
    prev_error = curr_error;
}

void precharge(){
    t = millis();
    while ((millis() - t) < 5000 ){
        digitalWrite(mains_relay_cont_sig_out,LOW);
        digitalWrite(prech_relay_cont_sig_out,HIGH);
      }    
    digitalWrite(prech_relay_cont_sig_out,LOW);
    digitalWrite(mains_relay_cont_sig_out,HIGH); 
    buzzer();
}

void brake_implausibility(){
    acc_ped1 = analogRead(acc_ped1_in);
    acc_ped2 = analogRead(acc_ped2_in);
    brake_ped = analogRead(brake_ped_in);   
    acc_avg = (acc_ped1 + acc_ped2) / 2; 
    
    if (brake_ped>0.1*1023 + 0.1*0.8*1023 && acc_avg >0.1*1023 +.25*0.8*1023)
        {
          while(acc_avg > 0.1*1023 + 0.05*0.8*1023){
              error = true;
              curr_error = error;
              digitalWrite(throttle_sw_out,HIGH );
              digitalWrite(cont_relay_cont_sig_out, LOW) ;
          }  
        }
    else {
          error = false;
          curr_error = error;
          digitalWrite(cont_relay_cont_sig_out, HIGH);
          if ((!curr_error && prev_error) || (prev_error_24 && !curr_error_24)){
            precharge();
          }
          digitalWrite(throttle_sw_out,LOW);
      }      
  }

void buzzer() {
     t = millis();
     while((millis() - t) < 3000){
        digitalWrite(buzzer_cont_out,HIGH);
    }
    digitalWrite(buzzer_cont_out,LOW);  
 }  
    
void error_change(){
      bool current_error = error;
  }
  
void setup() {
  
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(interlock_out,OUTPUT);
  pinMode(throttle_sw_out,OUTPUT);
  pinMode(acc_sig_out,OUTPUT);
  pinMode(brake_sig_out,OUTPUT);
  pinMode(prech_relay_cont_sig_out,OUTPUT); 
  pinMode(mains_relay_cont_sig_out,OUTPUT);
  pinMode(buzzer_cont_out,OUTPUT);  
  pinMode(tsal_ok_in,INPUT);
  pinMode(cont_relay_cont_sig_out,OUTPUT);
  pinMode(addi_sw_in,INPUT);
  
//inputs
  pinMode(brake_ped_in,INPUT);
  pinMode(acc_ped1_in,INPUT);
  pinMode(acc_ped2_in,INPUT);
  pinMode(hv_in,INPUT);

  digitalWrite(throttle_sw_out,HIGH);
  digitalWrite(acc_sig_out,LOW);
  digitalWrite(brake_sig_out,LOW);
  digitalWrite(prech_relay_cont_sig_out,LOW);
  digitalWrite(mains_relay_cont_sig_out,LOW);
  digitalWrite(buzzer_cont_out,LOW);
  digitalWrite(cont_relay_cont_sig_out,LOW);
  curr_error, prev_error = true;
  curr_error_24, prev_error_24 = false;
  
  while (!digitalRead(ok_24)){
  }
  if (digitalRead(ok_24)) {
    precharge();
  }
  
    error_check();
          while(! (!error && digitalRead(ok_24) && digitalRead(tsal_ok_in) && analogRead(brake_ped_in)>(0.1*1023+ 0.2*0.8*1023)))
                {}
                buzzer();

}

void loop() {
  // put your main code here, to run repeatedly:
    acc_ped1 = analogRead(acc_ped1_in);
    acc_ped2 = analogRead(acc_ped2_in);
    brake_ped = analogRead(brake_ped_in);   
    acc_avg = (acc_ped1 + acc_ped2) / 2; 
    curr_error_24 = ~digitalRead(ok_24);
    curr_error = error;
    
    if (!curr_error_24) {
    error_check();
      if ((!error) && ((digitalRead(tsal_ok_in)==HIGH)) ){
          acc_avg = map(acc_avg,0.1*1023 , 0.9*1023, 0, 255);
          analogWrite(acc_sig_out, acc_avg);
          brake_ped = map(brake_ped, 0.1*1023 , 0.9*1023, 0, 255); 
          analogWrite(brake_sig_out, brake_ped);  
        }  
      }  
    prev_error_24 = curr_error_24;  
}
