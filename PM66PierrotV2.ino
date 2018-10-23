/* The MIT License (MIT)

Copyright (c) 2017 by Pierre Steiblen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#define ON 1                // Saw is ON
#define OFF 0               // Saw is OFF

#define NB_SAMPLES 40       // Total samples needed for 2s analysis
#define RATE_SAMPLES 50     // Samples taken every 50ms

#define START_TRESHOLD 30  // Vibration level threshold - Saw is ON and running.       **** TO BE ADJUSTED AS NECESSARY *****
#define STOP_TRESHOLD 10   // Vibration level threshold - Saw is OFF and stopping      **** TO BE ADJUSTED AS NECESSARY *****

#define DELAY_STOP 1000     // Delay in ms after which the Dust Collection will stop (after the saw stopped) 

#define vibrationPin 9      // Connect vibration sensor to Pin 9 on Arduino
#define relayPin 7          // Connect relay to Pin 7 on Arduino

#define DEBUG               // Line to remove for final program

boolean Saw_Status = false ;  // Global Variable - Saw ON(true) or OFF(false)

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);      // Default led on Arduino board
  digitalWrite(LED_BUILTIN, LOW); 
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);     // Set relayPin HIGH to open the Relay
  
  pinMode(vibrationPin, INPUT_PULLUP);
  digitalWrite(vibrationPin, HIGH);    // Activate PULLUP Resistor

  #ifdef DEBUG
    Serial.begin(9600);
    delay(15);
  #endif
  delay(5);
}

void loop()
{
if (Saw_Status == false){
  	switch(Saw_Starting())   // Has the saw started ?
  	{
  	  case true:
  	    Saw_Status = true ;
  	    Turn_Dust_Collection(ON);
  	    break ;
  	  default:
  	    break ;
  	}
  }
  else{
    switch(Saw_Stopping())  // Is the saw stopping ?
    {
    	case true:
        #ifdef DEBUG
          Serial.print("Will stop the Dust collection in: ");
          Serial.print(DELAY_STOP);
          Serial.println("ms");
        #endif
    	  delay(DELAY_STOP);
    	  if (!Saw_Starting()){  // Simple "IF" test to NOT the turn the Dust Collection OFF if the saw has JUST restarted
    	    Saw_Status = false ;
    	    Turn_Dust_Collection(OFF);
    	  }
    	  break ;
    	default:
    	  break ;
    }
  }
}


void Turn_Dust_Collection(int Cde){
  switch(Cde)
  {
  	case ON:
  	  // Close Relay
  	  digitalWrite(relayPin, LOW);
  	  digitalWrite(LED_BUILTIN, HIGH); 
  	  break ;
  	case OFF:
  	  // Open Relay
  	  digitalWrite(relayPin, HIGH);
  	  digitalWrite(LED_BUILTIN, LOW); 
  	  break ;
  	default:
  	  #ifdef DEBUG
        Serial.println("ERR: Unexpected Command in Turn Dust Collection Subroutine");
      #endif
  	  // Open Relay
  	  digitalWrite(relayPin, HIGH);
  	  digitalWrite(LED_BUILTIN, LOW); 
  	  break ;
  }
}

boolean Saw_Starting(){
  int Start_Analysis = Vibration_read();

  #ifdef DEBUG
    Serial.print("Start Analysis (ON) = ");
    Serial.println(Start_Analysis) ;
  #endif

  if (Start_Analysis >= START_TRESHOLD)
  {
    #ifdef DEBUG
      Serial.println("Saw has STARTED");
    #endif
    return true;
  }
  else {
    #ifdef DEBUG
      Serial.println("Saw is OFF");
    #endif
    return false; 
  }
}


boolean Saw_Stopping(){
  int Stop_Analysis = Vibration_read();

  #ifdef DEBUG
    Serial.print("Stop Analysis (OFF) = ");
    Serial.println(Stop_Analysis) ;
  #endif

  if (Stop_Analysis <= STOP_TRESHOLD)
  {
    #ifdef DEBUG
      Serial.println("Saw is STOPPING");
    #endif
    return true;
  } 
  else
  {
     #ifdef DEBUG
      Serial.println("Saw is RUNNING");
    #endif
    return false;
  }
}


int Vibration_read(){
  int i = 0;
  int Samples[NB_SAMPLES], Result = 0;

   while (i <= (NB_SAMPLES -1))
   {
   	Samples[i] = digitalRead(vibrationPin);
   	Result += Samples[i] ;
    delay(RATE_SAMPLES); 
   	i++; 
   }

   #ifdef DEBUG
    Serial.print("Average: ");
    Serial.println(Result/NB_SAMPLES);
  #endif

  return Result / NB_SAMPLES ;
}
