#pragma once
#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include <Arduino.h>
#include <functional>

#define BUTTON_PIN 0

class ButtonHandler {

private:

public:

  void setup() {
    
    pinMode( BUTTON_PIN, INPUT );
    //digitalWrite( BUTTON_PIN, LOW );
  }

	void loop() {
  
    if( digitalRead( BUTTON_PIN ) == LOW ) {

       printf("Przycisk wcisniety ! \n");
    }
    else {

       
    }
	}
};

#endif /* BUTTONHANDLER_H_ */
