#ifndef _LIQUIDCRYSTAL_I2C_H
#define _LIQUIDCRYSTAL_I2C_H


#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "malloc.h"

typedef struct LiquidCrystalI2C_t{
    uint8_t _addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _charsize;
	uint8_t _backlightval;

}LiquidCrystalI2C_t;


bool liquid_crystal_i2c_init(LiquidCrystalI2C_t *mod, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows);
bool liquid_crystal_i2c_begin(LiquidCrystalI2C_t *mod);
bool liquid_crystal_i2c_clear(LiquidCrystalI2C_t *mod);
bool liquid_crystal_i2c_home(LiquidCrystalI2C_t *mod);
int liquid_crystal_i2c_no_display(LiquidCrystalI2C_t *mod);
#endif