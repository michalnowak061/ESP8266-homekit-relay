
// -------------------------------------------------------------------------

#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>

// -------------------------------------------------------------------------

#define ACCESSORY_NAME         ("ESP8266_Switch")
#define ACCESSORY_SN           ("SN_0123456")  
#define ACCESSORY_MANUFACTURER ("mnowak061")
#define ACCESSORY_MODEL        ("ESP8266")

#define BUTTON_PIN      0
#define LIGHTBULB_PIN   2

// -------------------------------------------------------------------------

bool lightbulb_status    = false;
bool switch_status       = false;

// -------------------------------------------------------------------------

homekit_value_t lightbulb_on_get() {
  
	return HOMEKIT_BOOL( lightbulb_status );
}

// -------------------------------------------------------------------------

void lightbulb_on_set(homekit_value_t value) {
  
	if (value.format != homekit_format_bool) {
    
		printf("Invalid on-value format: %d\n", value.format);
		return;
	}
 
	lightbulb_status = value.bool_value;
  lightbulb_update();
}

// -------------------------------------------------------------------------

homekit_characteristic_t name          = HOMEKIT_CHARACTERISTIC_(NAME, ACCESSORY_NAME);
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, ACCESSORY_SN);
homekit_characteristic_t lightbulb_on  = HOMEKIT_CHARACTERISTIC_(ON, false, .getter = lightbulb_on_get, .setter = lightbulb_on_set);

// -------------------------------------------------------------------------

void lightbulb_update() {
  
	if ( lightbulb_status ) {

    printf("ON\n");
    digitalWrite(LIGHTBULB_PIN, HIGH);
	} 
	else {
    
		printf("OFF\n");
		digitalWrite(LIGHTBULB_PIN, LOW);
	}
}

// -------------------------------------------------------------------------

void lightbulb_set_state(int state) {
  
	lightbulb_on.value.bool_value = state;
  lightbulb_on.setter(lightbulb_on.value);
  homekit_characteristic_notify(&lightbulb_on, lightbulb_on.value);

  lightbulb_update();
}

// -------------------------------------------------------------------------

void accessory_identify(homekit_value_t _value) {
  
	printf("accessory identify\n");
 
	for (int j = 0; j < 3; j++) {
  
		//lightbulb_status = true;
		//lightbulb_update();
		delay(100);
   
		//lightbulb_status = false;
		//lightbulb_update();
		delay(100);
	}
}

// -------------------------------------------------------------------------

homekit_accessory_t *accessories[] = {
  
  HOMEKIT_ACCESSORY( .id = 1,
                     .category = homekit_accessory_category_lightbulb,
                     .services = (homekit_service_t*[]) {
                      
                      HOMEKIT_SERVICE( ACCESSORY_INFORMATION,
                                       .characteristics = (homekit_characteristic_t*[]){
                                         &name,
                                         HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
                                         &serial_number,
                                         HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
                                         HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.1"),
                                         HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
                                         NULL
                                       }),
                      HOMEKIT_SERVICE(LIGHTBULB, .primary=true,
                                                 .characteristics = (homekit_characteristic_t*[]) {
                                                   HOMEKIT_CHARACTERISTIC(NAME, "Lightbulb"),
                                                   &lightbulb_on,
                                                   NULL
                                                 }),
                                                  NULL
                      }),
                      NULL
};

// -------------------------------------------------------------------------

homekit_server_config_t config = {
  
		.accessories = accessories,
		.password    = "111-11-111",
		.setupId     = "ABCD"
};

// -------------------------------------------------------------------------

void IRAM_ATTR Switch_State_Changed() {

  switch_status = digitalRead( BUTTON_PIN );
  lightbulb_status = switch_status;
  
  lightbulb_set_state( lightbulb_status );  
  
  printf("Switch state: %d \n",    switch_status);
  printf("Lightbulb state: %d \n", lightbulb_status);
}

// -------------------------------------------------------------------------

void accessory_init() {
  
	pinMode(LIGHTBULB_PIN, OUTPUT);
  pinMode(BUTTON_PIN,    OUTPUT);

  attachInterrupt(digitalPinToInterrupt( BUTTON_PIN ), Switch_State_Changed, CHANGE);

  switch_status    = digitalRead( BUTTON_PIN );
  lightbulb_status = switch_status;
}

// -------------------------------------------------------------------------

int accessory_get_lightbulb_status_on_esp() {

  return lightbulb_status;
}

// -------------------------------------------------------------------------

void accessory_update_state() {

  lightbulb_status = lightbulb_on_get().bool_value;
  lightbulb_update();
}

// -------------------------------------------------------------------------
