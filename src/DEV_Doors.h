////////////////////////////////////
//   DEVICE-SPECIFIC LED SERVICES //
////////////////////////////////////

struct DEV_GarageDoor : Service::GarageDoorOpener {     // A Garage Door Opener

  int remotePin;
  int sensorPin;

  SpanCharacteristic *current;            // reference to the Current Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *target;             // reference to the Target Door State Characteristic (specific to Garage Door Openers)  
  SpanCharacteristic *obstruction;        // reference to the Obstruction Detected Characteristic (specific to Garage Door Openers)

  DEV_GarageDoor(int remotePin, int sensorPin) : Service::GarageDoorOpener(){       // constructor() method
        
    current=new Characteristic::CurrentDoorState(1);              // initial value of 1 means closed
    target=new Characteristic::TargetDoorState(1);                // initial value of 1 means closed
    obstruction=new Characteristic::ObstructionDetected(false);   // initial value of false means NO obstruction is detected

    this->remotePin = remotePin;
    this->sensorPin = sensorPin;

    pinMode(remotePin, OUTPUT);
    pinMode(sensorPin, INPUT_PULLUP);
    
    Serial.print("Configuring Garage Door Opener");   // initialization message
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // update() method

    // see HAP Documentation for details on what each value represents
    boolean doorStatus = digitalRead(sensorPin);    // Low: 0 if door is close. High: 1 if door is open.
    // set current door status
    if (doorStatus==HIGH){
        current->setVal(0);
    } else {
        current->setVal(1);
    }


    if (target->getNewVal()==0 && current->getVal()==1){                     // if the target-state value is set to 0, HomeKit is requesting the door to be in open position
      LOG1("Opening Garage Door\n");
      current->setVal(2);                           // set the current-state value to 2, which means "opening"
      digitalWrite(remotePin, HIGH);                // enable remote for 0.3s if door is shut and target opening
      delay(333);
      digitalWrite(remotePin, LOW);                 
      obstruction->setVal(false);                   // clear any prior obstruction detection
    } else if (target->getNewVal()==1 && current->getVal()==0){
      LOG1("Closing Garage Door\n");                // else the target-state value is set to 1, and HomeKit is requesting the door to be in the closed position
      current->setVal(3);                           // set the current-state value to 3, which means "closing"
      digitalWrite(remotePin, HIGH);                // enable remote for 0.3s if door is open and target closing
      delay(333);
      digitalWrite(remotePin, LOW);          
      obstruction->setVal(false);                   // clear any prior obstruction detection
    } else if (target->getNewVal()==0 && current->getVal()==0){
        LOG1("Garage Door is open\n");  
    } else if (target->getNewVal()==0 && current->getVal()==1){
        LOG1("Garage Door is close\n"); 
    }
    
    return(true);                               // return true
  
  } // update

  void loop(){                                     // loop() method

    if(current->getVal()==target->getVal())        // if current-state matches target-state there is nothing do -- exit loop()
      return;

    if(current->getVal()==3 && current->timeVal()>20000){    // here we simulate a random obstruction, but only if the door is closing (not opening)
      current->setVal(4);                             // if our simulated obstruction is triggered, set the curent-state to 4, which means "stopped"
      obstruction->setVal(true);                      // and set obstruction-detected to true
      LOG1("Garage Door Obstruction Detected!\n");
    }

    if(current->getVal()==4)                       // if the current-state is stopped, there is nothing more to do - exit loop()     
      return;

    // This last bit of code only gets called if the door is in a state that represents actively opening or actively closing.
    // If there is an obstruction, the door is "stopped" and won't start again until the HomeKit Controller requests a new open or close action

    if(target->timeVal()>21000)                     // simulate a garage door that takes 20 seconds to operate by monitoring time since target-state was last modified
      current->setVal(target->getVal());           // set the current-state to the target-state
       
  } // loop
  
};