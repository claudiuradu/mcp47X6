/**************************************************************************/
/*! 
    @file     MCP47X6.h
    @author   C. Schnarel
	@license  BSD (see license.txt)
	
    This is part of an Arduino library to interface with the Microchip
    MCP47X6 series of Digital-to-Analog converters which are connected
    via the I2C bus.

    This file is the MCP47X6 I2C device class header file
    Based on Microchip datasheets for the following part numbers
        MCP4706 (8-bit), MCP4716 (10-bit), MCP4726 (12-bit)
    These parts share a common programming interface

    (c) Copyright 2013-Dec-24 Chip Schnarel <schnarel@hotmail.com>

    Updates should (hopefully) always be available at
        https://github.com/uchip/MCP47X6

	@section  HISTORY

    2013-Dec-25  - First release, C. Schnarel
*/
/**************************************************************************/

#ifndef _MCP47X6_H_
#define _MCP47X6_H_

#ifdef ARDUINO
    #if ARDUINO < 100
        #include "WProgram.h"
    #else
        #include "Arduino.h"
    #endif
#else
    #include "ArduinoWrapper.h"
#endif

#include <Wire.h>

// I2C Address of device
// MCP4706, MCP4716 & MCP4726 are factory programed for any of 0x60 thru 0x67
//  commonly 0x60
#define MCP47X6_DEFAULT_ADDRESS	0x60

class MCP47X6 {

    public:

        enum VREF : uint8_t {
            VDD_UNBUFFERED = 0,
            VREF_UNBUFFERED = 1,
            VREF_BUFFERED = 2
        };

        enum GAIN : uint8_t {
            X1 = 0,
            X2 = 1
        };

        enum POWER_DOWN : uint8_t {
            NOT_POWERED_NORMAL_OPERATION = 0,
            POWER_DOWN_1k_RESISTOR = 1,
            POWER_DOWN_100k_RESISTOR = 2,
            POWER_DOWN_500k_RESISTOR = 3
        };

        enum MEMORY_WRITE : uint8_t {
            WRITE_VOLATILE_DAC_REGISTER = 0,
            WRITE_VOLATILE_COMMAND = 1,
            WRITE_ALL_MEMORY = 2,
            WRITE_VOLATILE_CONFIGURATION_BITS = 3
        };

    public:
    
        MCP47X6(uint8_t address, MCP47X6::VREF vref, MCP47X6::GAIN gain, MCP47X6::POWER_DOWN power_down,  MCP47X6::MEMORY_WRITE memory_write, uint16_t level);
        
        bool init( void );

        // Set the configuration bits for the DAC
        void setGain(GAIN gain);
        void setVReference(VREF vref);
        void setPower(POWER_DOWN power_down);

        // Set the DAC
        bool setOutputLevelVolatileFast(uint8_t level);
        bool setOutputLevelVolatileFast(uint16_t level);

        bool downloadParameters (MEMORY_WRITE memory);


    private:

        bool writeCommand (MEMORY_WRITE memory);

        void saveVref(VREF vref);
        void saveGain(GAIN gain);
        void savePower(POWER_DOWN power_down);
        void saveMemory(MEMORY_WRITE memory);

        uint8_t address_;

        VREF vref_;
        GAIN gain_;
        POWER_DOWN power_down_;
        MEMORY_WRITE memory_write_;

        union VoltageLevel {
            uint16_t data;  
            struct {
                uint8_t lower_byte;
                uint8_t upper_byte;
            };
        };

        VoltageLevel voltage_level_;

        union CommandByte {
            uint8_t data;  
            struct {
                uint8_t G     : 1;  
                uint8_t PD0   : 1;
                uint8_t PD1   : 1;
                uint8_t VREF0 : 1;
                uint8_t VREF1 : 1;
                uint8_t C0    : 1;
                uint8_t C1    : 1;
                uint8_t C2    : 1;
            };
        };

        CommandByte command_byte_;
};

#endif /* _MCP47X6_H_ */
