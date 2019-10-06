/*
Usage: 
main.cpp in declaration
Gpioswitch ledGpioswitch(4); - will use pin 4, default OFF value is 0, ON - is 1
Gpioswitch ledGpioswitch(5,false); - will use pin 5, default OFF value is 1, ON - is 0. Inverse.

start(int Count, int Duration, int Delay)
ledGpioswitch.start(3,500,100) - do 3 cycles, ON*500ms + OFF*100+ON*500ms + OFF*100+ON*500ms + OFF*100
ledGpioswitch.start(2,1000) - do 2 cycles, ON*1000+OFF*1000+ON*1000+OFF*1000.
ledGpioswitch(-1,1000) - endless cycle ON*1000+OFF*1000...

You may stop anytime with ledGpioswitch.stop() or start() with new values.

Dont forget to put ledGpioswitch.handle() inside loop()

*/
#ifndef GPIOSWITCH_H
#define GPIOSWITCH_H

#include <vector>
#include "config.h"

//#define DEBUGTOSERIAL

class Gpioswitch {
  public:
    static std::vector<Gpioswitch *> instances;
    int gpioNum, swCount, swDuration,swDelay; //GPIO #, delay 
    bool onValue; //default true, sometimes we need to invert logic - false for ON statue
    bool working = false; // do we work?
    unsigned long startTime; //started time
    bool cState; //current GPIO state, to avoid too many digitalWrite, which slows down ESP
    unsigned int cycleDuration;
    
    
    static Gpioswitch built_in_led;

    Gpioswitch(int gpioNum, bool onValue = true);
    
    void start(int swCount, int swDuration, int swDelay = -1);
    void stop();
    void on();
    void off();
    void invert();
    void handleInternal();//should be in loop() for specific instance
    void static handle(); //should be in loop() to handle all instances
};
std::vector<Gpioswitch *> Gpioswitch::instances;


Gpioswitch::Gpioswitch(int gpioNum, bool onValue){
    #ifdef DEBUGTOSERIAL
        Serial.println("Gpioswitch: Gpioswitch");
    #endif
    
    pinMode(gpioNum, OUTPUT);
    digitalWrite(gpioNum, !onValue);
    this->onValue=onValue;
    this->gpioNum=gpioNum;
    this->cState = !this->onValue;     
    instances.push_back(this);
}
void Gpioswitch::handleInternal() {
    if (!this->working) return;
    if (this->swCount!=-1 && millis()-this->startTime > this->cycleDuration*this->swCount - this->swDelay) { // we completed work
        #ifdef DEBUGTOSERIAL
            Serial.println("Gpioswitch: TimeOut");
        #endif
        this->working = false;
        this->cState=!this->onValue;
        digitalWrite(this->gpioNum,!this->onValue);
        return;
    } else if (this->cState!=this->onValue && ((millis()-this->startTime) % (this->cycleDuration) < (unsigned int)this->swDuration)) { //set onValue
        #ifdef DEBUGTOSERIAL
            Serial.println("Gpioswitch: OnValue");
        #endif
        this->cState=this->onValue;
        digitalWrite(this->gpioNum,this->onValue);
        return;
    } else if (this->cState!=!this->onValue &&  ((millis()-this->startTime) % (this->cycleDuration) > (unsigned int)this->swDuration)) { //set offValue
        #ifdef DEBUGTOSERIAL
            Serial.println("Gpioswitch: OffValue");
        #endif
        this->cState=!this->onValue;
        digitalWrite(this->gpioNum,!this->onValue);
        return;
    }
}
void Gpioswitch::start(int swCount, int swDuration, int swDelay) {
    #ifdef DEBUGTOSERIAL
            Serial.println("Gpioswitch: Start");
    #endif
    if ((swCount<1 && swCount!=-1) || swDuration <1 || !(swDelay==-1 || swDelay>1)) return;
    this->working = true;
    this->swDuration=swDuration;
    swDelay==-1 ? this->swDelay = swDuration : this->swDelay=swDelay;
    this->cycleDuration=this->swDuration+this->swDelay;
    this->swCount = swCount;
    this->startTime = millis();

};
void Gpioswitch::on() {
    if (this->working) this->stop();
    digitalWrite(this->gpioNum, this->onValue);
    this->cState=this->onValue;
}
void Gpioswitch::off() {
    if (this->working) this->stop();
    digitalWrite(this->gpioNum, !this->onValue);
    this->cState=!this->onValue;
}
void Gpioswitch::invert() {
    if (this->working) this->stop();
    digitalWrite(this->gpioNum, !this->cState);
    this->cState=!this->cState;
}

void Gpioswitch::stop() {
    #ifdef DEBUGTOSERIAL
            Serial.println("Gpioswitch: Stop");
    #endif
    this->working = false;
    digitalWrite(this->gpioNum,!this->onValue);
    this->cState = !this->onValue; 
};
void Gpioswitch::handle() {
    for (auto &instance : instances) {
        instance->handleInternal();
    }
}


Gpioswitch Gpioswitch::built_in_led(ONBOARDLEDPIN);


#endif