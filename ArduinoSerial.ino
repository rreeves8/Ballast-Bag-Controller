// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       ArduinoSerial.ino
    Created:	2021-04-20 9:12:36 PM
    Author:     DESKTOP-B5SD6JD\Magnus
*/

// Define User Types below here or use a .h file
volatile int valves[] = {2,3,4,5};
volatile int levels[] = {6,7,8};
volatile int flowSensor = 9;
volatile int pump[] = {10,11,12,13};

volatile int incomingByte = 0;
volatile int direction = 1;

// Define Function Prototypes that use User Types below here or use a .h file

void setPumpDirection(int value) {
    direction = value;
}

void pumpControl(int value){
    if (direction == 1) {
        digitalWrite(pump[0], value);
        digitalWrite(pump[1], value);
    }

    if (direction == 0) {
        digitalWrite(pump[2], value);
        digitalWrite(pump[3], value);
    }
}

void openValve(int valves) {
    digitalWrite(valves, 1);
}

void closeValve(int valves) {
    digitalWrite(valves, 0);
}

int emptyBag(int bag) {
    openValve(valves[bag]);
    setPumpDirection(0);
    openValve(0);
    pumpControl(1);

    while(digitalRead(flowSensor)) {
        // wait till flow stops
    }

    closeValve(0);
    pumpControl(0);
    closeValve(valves[bag]);
}

int fillBag(int bag){   
    openValve(valves[bag]);
    setPumpDirection(1);
    openValve(0);
    pumpControl(1);
  
    while(!digitalRead(levels[bag])){
           //wait till fill 
    }

    closeValve(0);
    pumpControl(0);
    closeValve(valves[bag]);
    return 1;
}

int getBagsHalfFull() {
    int bit = 0;
    setPumpDirection(0);
    
    for(int i = 1; i < 4; i++) {
        bit = bit << 1;   
        
        openValve(valves[0]);
        openValve(valves[i]);
        pumpControl(1);

        int check = 1;
        for (int i = 0; i < 5 && check; i++) {
            if (digitalRead(flowSensor)) {
                bit += 0b1;
                check = 0;
            }
            else {
                delay(500);
            }
        }

        closeValve(valves[0]);
        pumpControl(0);
        delay(2000);
    }

    return bit;
}

int getBagsFull () {
    int bit = 0;
    for (int i = 0; i < 3; i++) {
        bit = bit << 1;
        if (digitalRead(levels[i])) {         
            bit += 0b1;
        }
    }

    return bit;
}

void setup(){
    for (int i = 0; i < 4; i++) {
        pinMode(valves[i], OUTPUT);
    }
    for (int i = 0; i < 3; i++) {
        pinMode(levels[i], INPUT);
    }
    pinMode(flowSensor, INPUT);
    Serial.begin(9600);
}

void loop(){
    
    if (Serial.read() == 0b1) {
        while (Serial.read() == 0b1) {
            //wait for instruction
        }

        int incomingByte = Serial.read();
        int returnByte;
        
        switch (incomingByte) {
            case(0b10): 
                returnByte = getBagsFull();
            
            case(0b11):
                returnByte = getBagsHalfFull();       

            case(0b0111):
                returnByte = fillBag(1);
            
            case(0b1011):
                returnByte = fillBag(2);
            
            case(0b1111):
                returnByte = fillBag(3);

            case(0b011111):
                returnByte = emptyBag(1);

            case(0b101111):
                returnByte = emptyBag(2);

            case(0b111111):
                returnByte = emptyBag(3);
        }
        Serial.write(returnByte);
    }

    delay(500);
}
