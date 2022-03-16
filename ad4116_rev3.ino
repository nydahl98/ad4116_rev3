#include <SPI.h>
#include <AD7173.h>

#define DATAOUT 11      // COPI/MOSI
#define DATAIN 12       // CIPO/MISO
#define spiCLOCK 13     // SCK/CLK
#define CHIPSELECT 5  // SS/CS
#define SPIMODE3 3

#if defined(ARDUINO_AVR_UNO)
    SPIClass spi;
//ARDUINO MEGA 1256
#elif defined(ARDUINO_AVR_MEGA)
    SPIClass spi;
//ARDUINO MEGA 2560
#elif defined(ARDUINO_AVR_MEGA2560)
    SPIClass spi;
//ARDUINO UNO WIFI
#elif defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
    SPIClass spi;
//ARDUINO UNO WIFI Rev 2
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2)
    SPIClass spi(DATAIN, spiCLOCK, DATAOUT, CHIPSELECT, SPIMODE3);
#endif


void setup()
{
    /* initiate serial communication */
    Serial.begin(19200);

    /* Set the pin modes */
    #if defined(ARDUINO_AVR_UNO)
    {
        pinMode(MOSI, OUTPUT);
        pinMode(MISO, INPUT);
        pinMode(SCK, OUTPUT);
        pinMode(CHIPSELECT, OUTPUT);
    }
    //ARDUINO UNO WIFI Rev 2
    #elif defined(ARDUINO_AVR_UNO_WIFI_REV2)
    {
        pinMode(MOSI, OUTPUT);
        pinMode(MISO, INPUT);
        pinMode(SCK, OUTPUT);
        pinMode(CHIPSELECT, OUTPUT);
    }
    #endif

	/* initialize SPI connection to the ADC */
	AD7173.init();

	/* sync the ADC */
    // Don't call Sync
	//AD7173.sync();

    /* toggle the chip select */
    digitalWrite(CHIPSELECT, HIGH); // disable device
    delay(10);
    digitalWrite(CHIPSELECT, LOW); // disable device
    /* allow the LDOs to power up */
	delay(10);

	

	/* reset the ADC registers to default */
	AD7173.reset();

    /* check if the ID register of the ADC is valid */
	if (AD7173.is_valid_id()) {
        Serial.println("AD7173 ID is valid");
    }
	else {
        Serial.println("AD7173 ID is invalid");
    }

    /* set ADC input channel configuration */
	/* enable channel 0 and channel 1 and connect each to 2 analog inputs for bipolar input */
	/* CH0 - CH15 */
	/* true/false to enable/disable channel */
	/* SETUP0 - SETUP7 */
	/* AIN0 - AIN16 */
	AD7173.set_channel_config(CH0, true, SETUP0, AIN0, AIN1);
	AD7173.set_channel_config(CH1, true, SETUP1, AIN2, AIN3);

    /* set the ADC SETUP0 coding mode to BIPLOAR output */
	/* SETUP0 - SETUP7 */
	/* BIPOLAR, UNIPOLAR */
	/* AIN_BUF_DISABLE, AIN_BUF_ENABLE */
	/* REF_EXT, REF_AIN, REF_INT, REF_PWR */
	AD7173.set_setup_config(SETUP0, BIPOLAR, AIN_BUF_DISABLE, REF_INT);
    AD7173.set_setup_config(SETUP1, BIPOLAR, AIN_BUF_DISABLE, REF_INT);

    /* set ADC OFFSET0 offset value */
	/* OFFSET0 - OFFSET7 */
	AD7173.set_offset_config(OFFSET0, 0);
    AD7173.set_offset_config(OFFSET1, 1);

    /* set the ADC FILTER0 ac_rejection to false and samplingrate to 1007 Hz */
	/* FILTER0 - FILTER7 */
	/* SPS_1, SPS_2, SPS_5, SPS_10, SPS_16, SPS_20, SPS_49, SPS_59, SPS_100, SPS_200 */
	/* SPS_381, SPS_503, SPS_1007, SPS_2597, SPS_5208, SPS_10417, SPS_15625, SPS_31250 */
	AD7173.set_filter_config(FILTER0, SPS_100);

    /* set the ADC data and clock mode */
	/* CONTINUOUS_CONVERSION_MODE, SINGLE_CONVERSION_MODE */
	/* in SINGLE_CONVERSION_MODE after all setup channels are sampled the ADC goes into STANDBY_MODE */
	/* to exit STANDBY_MODE use this same function to go into CONTINUOUS or SINGLE_CONVERSION_MODE */
	/* INTERNAL_CLOCK, INTERNAL_CLOCK_OUTPUT, EXTERNAL_CLOCK_INPUT, EXTERNAL_CRYSTAL */
	/* REF_DISABLE, REF_ENABLE */
	AD7173.set_adc_mode_config(CONTINUOUS_CONVERSION_MODE, INTERNAL_CLOCK, REF_DISABLE);

    /* enable/disable CONTINUOUS_READ_MODE and appending STATUS register to data */
	/* to exit CONTINUOUS_READ_MODE use AD7173.reset(); */
	/* AD7173.reset(); returns all registers to default state, so everything has to be setup again */
	/* true / false to enable / disable appending status register to data, 4th byte */
	AD7173.set_interface_mode_config(false, true);

	/* wait for ADC */
	delay(10);





    spi.begin();
}

/* ADC conversion data and STATUS register */
byte data[4];


void loop()
{
    	/* when ADC conversion is finished */
	if (DATA_READY) {
		/* get ADC conversion result */
		AD7173.get_data(data);

		/* send result via serial */
		Serial.print(data[0], HEX);
		Serial.print(data[1], HEX);
		Serial.println(data[2], HEX);
		Serial.println(data[3], HEX);
		delay(100);
	}

}