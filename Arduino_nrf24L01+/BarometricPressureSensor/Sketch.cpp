/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/*
 SCP1000 Barometric Pressure Sensor Display

 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/

 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip

 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13

 created 31 July 2010
 modified 14 August 2010
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>
#include "include/nRF24L01.h"
#include "include/RF24.h"
#include <stdio.h>

#define X 0
#define Y 1
#define Z 2
RF24 radio(9, 10); // CE, CSN
uint8_t register_value;
//const byte address[6] = "00001";
const byte address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
//const char text[500] = "Anthony!! you are awesome!!";
char text[500] = "Radio connection Established. Msg sent from base station";
int incomingByte = 0; // for incoming serial data
uint8_t payload_size, payload_ack_buffer[32],i, prev_msg;
int16_t Accel[3], Gyro[3], Mag[3]; 
float realGyro[3],realAccel[3];
char string[200] = {0};

		
const char init_string[] = "\n Connected to ANTSHIV Robotics Base Wireless Station\r"
							"\n\n To start motor enter [motor addr] + speed in duty cycle.\r"
							"\n To start sensor reading stream enter [sensor addr]. To stop sensor enter 0x0f \n\r"
							"\n\t 0x01 -> Hard Stop\r"
							"\n\t 0x02 -> Fetch All Raw readings\r"
							"\n\t 0x03 -> Fetch gyro readings\r"
							"\n\t 0x04 -> Fetch Accel readings\r"
							"\n\t 0x05 -> Test Motor 1 + Real Duty Cycle - 4(0) - 10(100%)\r"
							"\n\t 0x06 -> Test Motor 2 + Real Duty Cycle - 4(0) - 10(100%)\r"
							"\n\t 0x07 -> Test Motor 3 + Real Duty Cycle - 4(0) - 10(100%)\r"
							"\n\t 0x08 -> Test Motor 4 + Real Duty Cycle - 4(0) - 10(100%)\r"
							"\n\t 0x09 -> Hover\r"
							"\n\t 0x0a -> Move Right\r"
							"\n\t 0x0b -> Move Left\r"
							"\n\t 0x0c -> Move Forward\r"
							"\n\t 0x0d -> Move Back\r"
							"\n\t 0x0e -> Lower down\r"
							"\n\t 0x0f -> Stop sensor stream\r"
							"\n\t 0x10 -> Motor 1 + Duty Cycle (0 - 100%)\r"
							"\n\t 0x11 -> Motor 2 + Duty Cycle (0 - 100%)\r"
							"\n\t 0x12 -> Motor 3 + Duty Cycle (0 - 100%)\r"
							"\n\t 0x13 -> Motor 4 + Duty Cycle (0 - 100%)\r"
							"\n\t 0x1f -> Set PWM Frequency - Prescalar\r"
							"\n\t 0x2f -> Toggle Logging ON/OFF \r";
							
uint8_t cmd[200];
int cmd_index = 0;

int radio_ack_data() {
		if(radio.isAckPayloadAvailable()) {
		//radio.read(&, 32);
		/* We can read ack and send it back as a radio command*/
		payload_size = radio.getDynamicPayloadSize();
		radio.read(payload_ack_buffer, payload_size);
		//Serial.println("ACK Received: ");
		if ((payload_size > 5) && (cmd[0] == 0x04)) {
					
			Accel[X] = ((uint8_t)payload_ack_buffer[1] << 8) | (uint8_t)payload_ack_buffer[0];
			Accel[Y] = ((uint8_t)payload_ack_buffer[3] << 8) | (uint8_t)payload_ack_buffer[2];
			Accel[Z] = ((uint8_t)payload_ack_buffer[5] << 8) | (uint8_t)payload_ack_buffer[4];		
			sprintf(string,"%x %d \t%x %d \t%x %d", Accel[X], Accel[X], Accel[Y], Accel[Y], Accel[Z], Accel[Z] );
			Serial.println(string);
		}
		if ((payload_size > 10) && (cmd[0] == 0x03)) {
			
			realGyro[X] = ((uint8_t)payload_ack_buffer[3] << 24) | ((uint8_t)payload_ack_buffer[2] << 16) | ((uint8_t)payload_ack_buffer[1] << 8) | ((uint8_t)payload_ack_buffer[0]);
			realGyro[Y] = ((uint8_t)payload_ack_buffer[7] << 24) | ((uint8_t)payload_ack_buffer[6] << 16) | ((uint8_t)payload_ack_buffer[5] << 8) | ((uint8_t)payload_ack_buffer[4]);
			realGyro[Z] = ((uint8_t)payload_ack_buffer[11] << 24) | ((uint8_t)payload_ack_buffer[10] << 16) | ((uint8_t)payload_ack_buffer[9] << 8) | ((uint8_t)payload_ack_buffer[8]);
			//sprintf(string,"\r\n%f \t %f \t %f",realGyro[X], realGyro[Y], realGyro[Z]);
			//sprintf(string,"Hello World");
			Serial.print(realGyro[X], HEX);
			Serial.print(" ");
			Serial.print(realGyro[Y], HEX);
			Serial.print(" ");
			Serial.print(realGyro[Z], HEX);
			Serial.println(" ");
			//Serial.println(string);
		}
		if ((payload_size > 10) && (cmd[0] == 0x02)) {
			Gyro[X] = ((uint8_t)payload_ack_buffer[1] << 8) | (uint8_t)payload_ack_buffer[0];
			Gyro[Y] = ((uint8_t)payload_ack_buffer[3] << 8) | (uint8_t)payload_ack_buffer[2];
			Gyro[Z] = ((uint8_t)payload_ack_buffer[5] << 8) | (uint8_t)payload_ack_buffer[4];
			Accel[X] = ((uint8_t)payload_ack_buffer[7] << 8) | (uint8_t)payload_ack_buffer[6];
			Accel[Y] = ((uint8_t)payload_ack_buffer[9] << 8) | (uint8_t)payload_ack_buffer[8];
			Accel[Z] = ((uint8_t)payload_ack_buffer[11] << 8) | (uint8_t)payload_ack_buffer[10];
			Mag[X] = ((uint8_t)payload_ack_buffer[13] << 8) | (uint8_t)payload_ack_buffer[12];
			Mag[Y] = ((uint8_t)payload_ack_buffer[15] << 8) | (uint8_t)payload_ack_buffer[14];
			Mag[Z] = ((uint8_t)payload_ack_buffer[17] << 8) | (uint8_t)payload_ack_buffer[16];
			realAccel[X] = Accel[X] /pow(2,15) *2;
			realAccel[Y] = Accel[Y] /pow(2,15) *2;
			realAccel[Z] = Accel[Z] /pow(2,15) *2;
			
			realGyro[X] = Gyro[X]/pow(2,15) *2000;
			realGyro[Y] = Gyro[Y]/pow(2,15) *2000;
			realGyro[Z] = Gyro[Z]/pow(2,15) *2000;
				
			sprintf(string,"%f \t%f \t%f \t%f \t%f \t%f", realAccel[X], realAccel[Y], realAccel[Z], realGyro[X], realGyro[Y], realGyro[Z]);
			Serial.println(string);
			/* Serial.print(Gyro[X], HEX);
			Serial.print(" ");
			Serial.print(Gyro[Y], HEX);
			Serial.print(" ");
			Serial.print(Gyro[Z], HEX);
			Serial.print(" ");
			Serial.print(Accel[X], HEX);
			Serial.print(" ");
			Serial.print(Accel[Y], HEX);
			Serial.print(" ");
			Serial.print(Accel[Z], HEX);
			Serial.print(" "); */
			/*Serial.print(Mag[X], HEX);
			Serial.print(" ");
			Serial.print(Mag[Y], HEX);
			Serial.print(" ");
			Serial.print(Mag[Z], HEX);
			Serial.println(" ");
			*/
			//Serial.println(" ");
			/* for (i=0; i < payload_size; i++ ) {
				Serial.println(payload_ack_buffer[i], HEX);
			} */
			//Serial.println(" ");
			//Serial.println(" ");
		} else {
			for (i=0; i < payload_size; i++ ) {
				Serial.println(payload_ack_buffer[i], HEX);
			}
		}
		//Serial.write(register_value);
		//radio.write(&incomingByte, sizeof(incomingByte));
		return true;
	} else {
		return false;
	}
} 
		
void setup() {
	radio.begin();
	radio.openWritingPipe(address);
	radio.enableAckPayload();
	radio.enableDynamicPayloads();
	radio.setRetries(5,5); 
	//radio.setCRCLength(RF24_CRC_16);
	Serial.begin(57600);
	Serial.println(init_string);
//	radio.write(&text, sizeof(text));
//	radio.setPALevel(RF24_PA_MIN);
	radio.stopListening();

}

/*
 * NRF24L01 - ACK Payload Code help:
 * https://forum.arduino.cc/index.php?topic=472339.0
 * https://forum.arduino.cc/index.php?topic=421081
 *
 */

void send_motor_command(int count) {
	if (cmd_index < (count - 1)) {
		cmd_index++;
	} else {
		radio.write(&cmd[0], sizeof(cmd[0]) * (cmd_index + 1));
		Serial.println(sizeof(cmd[0]) * (cmd_index + 1), DEC);
		cmd_index = 0;
	}
}

/* 
 *	Count : It is the number of data points we need to capture. 
*/
void getdata() {
	if (cmd_index < 1) {
		cmd_index++;
		} else {
			int i=0;
			for (i =0; i <= cmd[1]; i++) {
				radio.write(&cmd[0], sizeof(cmd[0]));
				int count = 5;
				while(!radio_ack_data()) {
					count++;
					delay(1);
					if (count > 10) {
						break;
					}
				}
				//Serial.println(sizeof(cmd[0]) * (cmd_index + 1), DEC);
			}
			cmd_index = 0;
	}
}

/*
 * The code will listen to readings from UART and send those signals wireless to the quadcopter
 * The 1st goal is to test the Motors - Motor signal + Speed
 */

void send_command() {
	switch (cmd[0]) {
		case 0x01:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x02:
			Serial.println(cmd[cmd_index], HEX);
			getdata();
			return;
		break;
		case 0x03:
			Serial.println(cmd[cmd_index], HEX);
			getdata();
			return;
		break;
		case 0x04:
			Serial.println(cmd[cmd_index], HEX);
			getdata();
			return;
		break;
		case 0x05: /* Motor control 1 */
			Serial.println("Executing Motor 1 command");
			send_motor_command(2);
			return;
		break;
		case 0x06:/* Motor control 2 */
			Serial.println("Executing Motor 2 command");
			send_motor_command(2);
			return;
		break;
		case 0x07:/* Motor control 3 */
			Serial.println("Executing Motor 3 command");
			send_motor_command(2);
			return;
		break;
		case 0x08:/* Motor control 4 */
			Serial.println("Executing Motor 4 command");
			send_motor_command(2);
			return;
		break;
		case 0x09:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0a:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0b:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0c:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0d:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0e:
			Serial.println(cmd[cmd_index], HEX);
		break;
		case 0x0f:
			Serial.println(cmd[cmd_index], HEX);
		break;	
		case 0x10:/* Motor control 1 */
			Serial.println("Executing Motor 1 command");
			send_motor_command(2);
			return;
		break;
		case 0x11:/* Motor control 2 */
			Serial.println("Executing Motor 2 command");
			send_motor_command(2);
			return;
		break;
		case 0x12:/* Motor control 3 */
			Serial.println("Executing Motor 3 command");
			send_motor_command(2);
			return;
		break;
		case 0x13:/* Motor control 4 */
			Serial.println("Executing Motor 4 command");
			send_motor_command(2);
			return;
		break;
		case 0x1f:
			send_motor_command(3);
			return;
		break;
		case 0x2f:
			Serial.println("Toggling log on Quadcopter");
			radio.write(cmd, sizeof(cmd[0]) * (cmd_index + 1));
			return;
		break;
		default: {
			Serial.print("This command does not exist: 0x");
			Serial.println(cmd[cmd_index], HEX);
		}
	}
	radio.write(cmd, sizeof(cmd[0]) * (cmd_index + 1));
	//prev_msg = cmd[0];
	 
}
 
void loop() {
	/*
	radio.read_register(NRF_CONFIG);
	radio.read_register(SETUP_RETR);
	radio.read_register(RF_CH);
	radio.read_register(RF_SETUP);
	radio.read_register(SETUP_AW);
	radio.read_register(TX_ADDR);
	radio.print_address_register(text, TX_ADDR, 5);
	*/
	while(!Serial.available()) {};
	  if (Serial.available() > 0) {
		  // read the incoming byte:
		  //incomingByte = Serial.read();
		  cmd[cmd_index] = (int8_t) Serial.read();
		  // say what you got:
		  Serial.print("\n\rI received: 0x");
		  Serial.println(cmd[cmd_index], HEX);
		   Serial.print("Command Index is: ");
		  Serial.println(cmd_index, HEX);
		  send_command();
	  }
	//radio.write(&text, sizeof(text));
	 radio_ack_data();

//	delay(50);
//	register_value = radio.read_register(NRF_STATUS);

	//delay(1200);
	
	//TODO:
	//Take inputs from serial and send to drone
}

