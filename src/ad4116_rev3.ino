#include <SPI.h>
#include "Array.h"
#include "AD4116.h"

int probePin = A0;

// #define DATAOUT 11      // COPI/MOSI
// #define DATAIN 12       // CIPO/MISO
// #define spiCLOCK 13     // SCK/CLK
//#define SPIMODE3 3
#define CS_PIN       5

	// uint8_t CHIPSELECT = 5; // SS/CS ~5 PIN
	// SPIClass spi;

// #define DATAOUT 23      // COPI/MOSI
// #define DATAIN 19       // CIPO/MISO
// #define spiCLOCK 18     // SCK/CLK
// #define SPIMODE3 3


// 	uint8_t CHIPSELECT = 16; // SS/CS ~5 PIN
// 	SPIClass spi;



void setup()
{


    pinMode(probePin, OUTPUT);
    /* initiate serial communication */
    Serial.begin(115200);

    /* Set the pin modes */
    
        // pinMode(MOSI, OUTPUT);
        // pinMode(MISO, INPUT);
        // pinMode(SCK, OUTPUT);
        // pinMode(CHIPSELECT, OUTPUT);
    
	    pinMode(CS_PIN, OUTPUT);

	/* sync the ADC */


		/* initialize SPI connection to the ADC */
		/* initiate SPI communication */
		SPI.begin();
		/* use SPI mode 3 */
		// spi.setDataMode(SPI_MODE3);
		//SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE3)); //14Mhz clock
		SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3)); //4Mhz clock
		/* allow the LDOs to power up */
		delay(100);

	/* reset the ADC registers to default */

		// for (int i = 0; i < 8; i++) {
		// 	SPI.transfer(0xFF);
		// }
		// /* allow the LDOs to power up */
		// delay(10);

	/* Address: 0x07, Reset: 0x30DX, Name: ID */

		/* get the ADC device ID */
		byte id[2];
		AD4116.get_register(ID_REG, id, 2);
		/* check if the id matches 0x34DX, where X is don't care */
		id[1] &= 0xF0;
		bool valid_id = id[0] == 0x34 && id[1] == 0xD0;


    /* check if the ID register of the ADC is valid */
	if (valid_id) {
        Serial.println("AD4116 ID is valid");
    }
	else {
        Serial.println("AD4116 ID is invalid");
    }

    /* set ADC input channel configuration */
	/* enable channel 0 and channel 1 and connect each to 2 analog inputs for bipolar input */
	/* CH0 - CH15 */
	/* true/false to enable/disable channel */
	/* SETUP0 - SETUP7 */
	/* AIN0 - AIN16 */
	AD4116.set_channel_config(CH0, true, SETUP0, AIN0, AIN1);


    /* set the ADC SETUP0 coding mode to BIPLOAR output */
	/* SETUP0 - SETUP7 */
	/* BIPOLAR, UNIPOLAR */
	/* AIN_BUF_DISABLE, AIN_BUF_ENABLE */
	/* REF_EXT, REF_AIN, REF_INT, REF_PWR */
	AD4116.set_setup_config(SETUP0, BIPOLAR, AIN_BUF_ENABLE, REF_EXT);


    /* set ADC OFFSET0 offset value */
	/* OFFSET0 - OFFSET7 */
	AD4116.set_offset_config(OFFSET0, 0x00);
 

    /* set the ADC FILTER0 ac_rejection to false and samplingrate to 1007 Hz */
	/* FILTER0 - FILTER7 */
	/* SPS_1, SPS_2, SPS_5, SPS_10, SPS_16, SPS_20, SPS_49, SPS_59, SPS_100, SPS_200 */
	/* SPS_381, SPS_503, SPS_1007, SPS_2597, SPS_5208, SPS_10417, SPS_15625, SPS_31250 */
	//AD4116.set_filter_config(FILTER0, SPS_1007);

	/* prepare the configuration value */
	byte value[2] = {0x00, 0x00};
	/* SINC3_MAP0 [15], RESERVED [14:12], ENHFILTEN0 [11], ENHFILT0 [10:8], RESERVED [7], ORDER0 [6:5], ORD0 [4:0] */
	value[0] = 0x0E;
	//value[1] = SPS_100;
	value[1] = SPS_200;


	/* update the configuration value */
	AD4116.set_register(FILTER0, value, 2);

	/* verify the updated configuration value */
	AD4116.get_register(FILTER0, value, 2);


    /* set the ADC data and clock mode */
	/* CONTINUOUS_CONVERSION_MODE, SINGLE_CONVERSION_MODE */
	/* in SINGLE_CONVERSION_MODE after all setup channels are sampled the ADC goes into STANDBY_MODE */
	/* to exit STANDBY_MODE use this same function to go into CONTINUOUS or SINGLE_CONVERSION_MODE */
	/* INTERNAL_CLOCK, INTERNAL_CLOCK_OUTPUT, EXTERNAL_CLOCK_INPUT, EXTERNAL_CRYSTAL */
	/* REF_DISABLE, REF_ENABLE */
	//AD4116.set_adc_mode_config(CONTINUOUS_CONVERSION_MODE, INTERNAL_CLOCK, REF_DISABLE);
	AD4116.set_adc_mode_config(SINGLE_CONVERSION_MODE, INTERNAL_CLOCK, REF_DISABLE);

    /* enable/disable CONTINUOUS_READ_MODE and appending STATUS register to data */
	/* to exit CONTINUOUS_READ_MODE use AD4116.reset(); */
	/* AD4116.reset(); returns all registers to default state, so everything has to be setup again */
	/* true / false to enable / disable appending status register to data, 4th byte */
	AD4116.set_interface_mode_config(false, true);

	/* wait for ADC */
	delay(10000);

    // spi.begin();
}

/* ADC conversion data and STATUS register */
byte data[4];


void loop()
{
    	/* when ADC conversion is finished */
	if (DATA_READY || true) {
		/* get ADC conversion result */
		AD4116.get_data(data);

		/* send result via serial */
		long int data0 = data[0];
		long int data1 = data[1];
		long int data2 = data[2];

		long long int id3 = (data0 << 16) | (data1 << 8) | data2;

		double InputResultsScaled = 0.000001 * (5000000 * (id3 - 8387126) / 1676914);
		//int InputResults = id4;
		Serial.println(InputResultsScaled, 6);


		if (InputResultsScaled < -10) {
        Serial.print("led on");
        Serial.println();
        digitalWrite(probePin, HIGH);
  
  
 }
 else {
  Serial.print("led off");
  Serial.println();
  digitalWrite(probePin, LOW);
 }


		//delay(250);
	}


}



