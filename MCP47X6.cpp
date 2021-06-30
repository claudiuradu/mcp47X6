/**************************************************************************/
/*! 
    @file     MCP47X6.cpp
    @author   C. Schnarel
	@license  BSD (see license.txt)
	
    This file is part of an Arduino library to interface with the
    Microchip MCP47X6 series of Digital-to-Analog converters which are
    connected via the I2C bus.

    This file is the MCP47X6 I2C device class file
    Based on Microchip datasheets for the following part numbers
        MCP4706 (8-bit), MCP4716 (10-bit), MCP4726 (12-bit)
    These parts share a common programming interface

    Copyright (c) 2013 Chip Schnarel <schnarel@hotmail.com>

    Updates should (hopefully) always be available at
        https://github.com/uchip/MCP47X6

	@section  HISTORY

    2013-Dec-25  - First release, C. Schnarel
*/
/**************************************************************************/

#include "MCP47X6.h"
//#include "Wire.h"

/**************************************************************************/

/******************************************
 * Specific address constructor.
 * @param address I2C address
 * @see MCP47X6_DEFAULT_ADDRESS
 */
MCP47X6::MCP47X6(uint8_t address, 
                MCP47X6::VREF vref, MCP47X6::GAIN gain, 
                MCP47X6::POWER_DOWN power_down,
                MCP47X6::MEMORY_WRITE memory_write,
                uint16_t level) :
  address_(address),
  vref_(vref),
  gain_(gain),
  power_down_(power_down),
  memory_write_(memory_write)
{
    saveGain(gain_);
    saveVref(vref_);
    savePower(power_down_);

    voltage_level_.data = level;

}

bool MCP47X6::init(void) {
  return writeCommand(memory_write_);
}

/******************************************
 * Set the configuration bits for the DAC
 */
void MCP47X6::setGain(GAIN gain) {
  
  gain_ = gain;

  saveGain(gain_);

}

void MCP47X6::setVReference(VREF vref) {

  vref_ = vref;

  saveVref(vref_);

}

void MCP47X6::setPower(POWER_DOWN power_down) {

  power_down_ = power_down;

  savePower(power_down_);

}


/******************************************
 * Set DAC output level value
 * Valid range is 0x000 to 0xFFF for all parts.
 * For the MCP4726 all 4096 steps are used.
 * For the MCP4716 only 1024 steps are used. The two LSBs are not used.
 * (i.e. value & 0xFFC)
 * For the MCP4706 only 256 steps are used. The four LSBs are not used.
 * (i.e. value & 0xFF0)
 */
uint8_t MCP47X6::setOutputLevelVolatileFast(uint16_t level) {

  /*Wire.beginTransmission(address_);
  Wire.write(command_byte_.data);
  Wire.write(0xff);
  Wire.write(0xff);
  Wire.endTransmission();*/
  return (voltage_level_.lower_byte);

}

// Special case for 8-bit device (MCP4706) - saves one byte of transfer
// and is therefore faster
uint8_t MCP47X6::setOutputLevelVolatileFast(uint8_t level) {
  return (command_byte_.data);
}



bool MCP47X6::downloadParameters (MEMORY_WRITE memory) {

  memory_write_ = memory;

  return writeCommand(memory_write_);

}


bool MCP47X6::writeCommand (MEMORY_WRITE memory) {

  saveMemory(memory);

  Wire.beginTransmission(address_);
  Wire.write(command_byte_.data);
  Wire.write(voltage_level_.upper_byte);
  Wire.write(voltage_level_.lower_byte);
  
  return (Wire.endTransmission() == 0);

}

void MCP47X6::saveVref(VREF vref)
{
  switch (vref)
  {
    case VREF::VDD_UNBUFFERED:
      
      command_byte_.VREF1 = 0;
      command_byte_.VREF0 = 0;
      
      break;
      
      case VREF::VREF_UNBUFFERED:
      
        command_byte_.VREF1 = 1;
        command_byte_.VREF0 = 0;

      break;
      
      case VREF::VREF_BUFFERED:
      
        command_byte_.VREF1 = 1;
        command_byte_.VREF0 = 1;
      
      break;
  }

}

void MCP47X6::saveGain(GAIN gain)
{
  switch (gain)
  {
    case GAIN::X1:
      
      command_byte_.G = 0;

      break;
      
      case GAIN::X2:
      
        command_byte_.G = 1;

      break;   
  }
}

void MCP47X6::saveMemory(MEMORY_WRITE memory)
{

  switch (memory)
  {
    case MEMORY_WRITE::WRITE_VOLATILE_DAC_REGISTER:
      
        command_byte_.C2 = 0;
        command_byte_.C1 = 0;
        command_byte_.C0 = 0;

      break;
      
      case MEMORY_WRITE::WRITE_VOLATILE_COMMAND:
      
        command_byte_.C2 = 0;
        command_byte_.C1 = 1;
        command_byte_.C0 = 0;

      break;   
            
      case MEMORY_WRITE::WRITE_ALL_MEMORY:
      
        command_byte_.C2 = 0;
        command_byte_.C1 = 1;
        command_byte_.C0 = 1;

      break;   

      case MEMORY_WRITE::WRITE_VOLATILE_CONFIGURATION_BITS:
      
        command_byte_.C2 = 1;
        command_byte_.C1 = 0;
        command_byte_.C0 = 0;

      break;   
  }
}

void MCP47X6::savePower(POWER_DOWN power)
{
  switch (power)
  {
    case POWER_DOWN::NOT_POWERED_NORMAL_OPERATION:
      
        command_byte_.PD1 = 0;
        command_byte_.PD0 = 0;

      break;
      
      case POWER_DOWN::POWER_DOWN_1k_RESISTOR:
      
        command_byte_.PD1 = 0;
        command_byte_.PD0 = 1;

      break;   
            
      case POWER_DOWN::POWER_DOWN_100k_RESISTOR:
      
        command_byte_.PD1 = 1;
        command_byte_.PD0 = 0;

      break;   

      case POWER_DOWN::POWER_DOWN_500k_RESISTOR:
      
        command_byte_.PD1 = 1;
        command_byte_.PD0 = 1;

      break;   
  }
}