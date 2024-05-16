#include "liquidcrystal_i2c.h"
#include <inttypes.h>
#include <Print.h>
#include "stdint.h"
#include "stdlib.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NO_BACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

/************ Function Definitions ************/

void liquidcrystal_i2c_clear(LiquidCrystalI2C_t *mod); 
void liquidcrystal_home(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_set_cursor(LiquidCrystalI2C_t *mod, uint8_t col, uint8_t row); 
void liquidcrystal_i2c_no_display(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_display(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_no_cursor(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_cursor(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_no_blink(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_blink(LiquidCrystalI2C_t *mod);
void liquidcrystal_i2c_scroll_display_left(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_scroll_display_right(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_left_to_right(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_right_to_left(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_autoscroll(LiquidCrystalI2C_t *mod); 
void liquidcrystal_i2c_no_autoscroll(LiquidCrystalI2C_t *mod); 
void liquidcrystal_createchar(LiquidCrystalI2C_t *mod, uint8_t location, uint8_t charmap[]); 
void liquidcrystal_no_backlight(LiquidCrystalI2C_t *mod); 
void liquidcrystal_backlight(LiquidCrystalI2C_t *mod); 
bool liquidcrystal_back(LiquidCrystalI2C_t *mod); 

/************ low level data pushing commands **********/

void i2c_init_bus(void){

}

void i2c_start_function(uint8_t address){
    
}

void i2c_stop_function(void){

}

void i2c_write_byte(uint8_t byte){

}

void i2c_write_bytearray(uint8_t *bytes, size_t len){

}

void i2c_delay_microseconds(int microseconds){

}

void i2c_delay_milliseconds(int milliseconds){

}

void expander_write(LiquidCrystalI2C_t *mod, uint8_t _data){

    i2c_start_function(mod->_addr);
	i2c_write_byte(_data | mod->_backlightval);
	i2c_stop_function();
}

// write either command or data
void liquidcyrstal_send(LiquidCrystalI2C_t *mod, uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits(mod, (highnib)|mode);
	write4bits(mod, (lownib)|mode);
}

void write4bits(LiquidCrystalI2C_t *mod, uint8_t value) {
	expander_write(mod, value);
	pulseEnable(mod, value);
}

void pulseEnable(LiquidCrystalI2C_t *mod, uint8_t _data){
	expander_write(mod, _data | En);	// En high
	i2c_delay_microseconds(1);		// enable pulse must be >450ns

	expander_write(mod, _data & ~En);	// En low
	i2c_delay_microseconds(50);		// commands need > 37us to settle
}

/*********** mid level commands, for liquidcyrstal_sending data/cmds */

void liquidcrystal_command(LiquidCrystalI2C_t *mod, uint8_t value) {
	liquidcyrstal_send(mod, value, 0);
}


size_t liquidcrystal_write(LiquidCrystalI2C_t *mod, uint8_t value){
    liquidcyrstal_send(mod, value, Rs);
	return 1;    
}

void liquidcrystal_load_custom_character(LiquidCrystalI2C_t *mod, uint8_t char_num, uint8_t *rows){
	liquidcrystal_createchar(mod, char_num, rows);
}

void liquidcrystal_backlight_en(LiquidCrystalI2C_t *mod, uint8_t new_val){
	if (new_val) {
		backlight();		// turn backlight on
	} else {
		liquidcrystal_no_backlight();		// turn backlight off
	}
}

void liquidcrystalprintstr(LiquidCrystalI2C_t *mod, const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed
	//print(c);
}

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

bool liquid_crystal_i2c_init(LiquidCrystalI2C_t *mod, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows){
    

    mod->_addr = lcd_addr;
	mod->_cols = lcd_cols;
	mod->_rows = lcd_rows;
	mod->_charsize = 12;
	mod->_backlightval = LCD_BACKLIGHT;


    i2c_init_bus();
	mod->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (mod->_rows > 1) {
		mod->_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((mod->_charsize != 0) && (mod->_rows == 1)) {
		mod->_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before liquidcyrstal_sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	i2c_delay_milliseconds(50);

	// Now we pull both RS and R/W low to begin commands
	expander_write(mod, mod->_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(mod, 0x03 << 4);
	i2c_delay_microseconds(4500); // wait min 4.1ms

	// second try
	write4bits(mod, 0x03 << 4);
	i2c_delay_microseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(mod, 0x03 << 4);
	i2c_delay_microseconds(150);

	// finally, set to 4-bit interface
	write4bits(mod, 0x02 << 4);

	// set # lines, font size, etc.
	liquidcrystal_command(mod, LCD_FUNCTIONSET | mod->_displayfunction);

	// turn the display on with no cursor or blinking default
	mod->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	
    display();

	// clear it off
	liquidcrystal_i2c_clear(mod);

	// Initialize to default text direction (for roman languages)
	mod->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	liquidcrystal_command(mod, LCD_ENTRYMODESET | mod->_displaymode);

	liquidcrystal_home(mod);
}

/********** high level commands, for the user! */
void liquidcrystal_i2c_clear(LiquidCrystalI2C_t *mod){
	liquidcrystal_command(mod, LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	i2c_delay_microseconds(2000);  // this command takes a long time!
}

void liquidcrystal_home(LiquidCrystalI2C_t *mod){
	liquidcrystal_command(mod, LCD_RETURNHOME);  // set cursor position to zero
	i2c_delay_microseconds(2000);  // this command takes a long time!
}

void liquidcrystal_i2c_set_cursor(LiquidCrystalI2C_t *mod, uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > mod->_rows) {
		row = mod->_rows-1;    // we count rows starting w/0
	}
	liquidcrystal_command(mod,LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void liquidcrystal_i2c_no_display(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol &= ~LCD_DISPLAYON;
	liquidcrystal_command(mod, LCD_DISPLAYCONTROL | mod->_displaycontrol);
}
void liquidcrystal_i2c_display(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol |= LCD_DISPLAYON;
	liquidcrystal_command(mod,LCD_DISPLAYCONTROL | mod->_displaycontrol);
}

// Turns the underline cursor on/off
void liquidcrystal_i2c_no_cursor(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol &= ~LCD_CURSORON;
	liquidcrystal_command(mod, LCD_DISPLAYCONTROL | mod->_displaycontrol);
}
void liquidcrystal_i2c_cursor(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol |= LCD_CURSORON;
	liquidcrystal_command(mod, LCD_DISPLAYCONTROL | mod->_displaycontrol);
}

// Turn on and off the blinking cursor
void liquidcrystal_i2c_no_blink(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol &= ~LCD_BLINKON;
	liquidcrystal_command(mod, LCD_DISPLAYCONTROL | mod->_displaycontrol);
}
void liquidcrystal_i2c_blink(LiquidCrystalI2C_t *mod) {
	mod->_displaycontrol |= LCD_BLINKON;
	liquidcrystal_command(mod, LCD_DISPLAYCONTROL | mod->_displaycontrol);
}

// These commands scroll the display without changing the RAM
void liquidcrystal_i2c_scroll_display_left(LiquidCrystalI2C_t *mod) {
	liquidcrystal_command(mod, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void liquidcrystal_i2c_scroll_display_right(LiquidCrystalI2C_t *mod) {
	liquidcrystal_command(mod, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void liquidcrystal_i2c_left_to_right(LiquidCrystalI2C_t *mod) {
	mod->_displaymode |= LCD_ENTRYLEFT;
	liquidcrystal_command(mod, LCD_ENTRYMODESET | mod->_displaymode);
}

// This is for text that flows Right to Left
void liquidcrystal_i2c_right_to_left(LiquidCrystalI2C_t *mod) {
	mod->_displaymode &= ~LCD_ENTRYLEFT;
	liquidcrystal_command(mod, LCD_ENTRYMODESET | mod->_displaymode);
}

// This will 'right justify' text from the cursor
void liquidcrystal_i2c_autoscroll(LiquidCrystalI2C_t *mod) {
	mod->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	liquidcrystal_command(mod, LCD_ENTRYMODESET | mod->_displaymode);
}

// This will 'left justify' text from the cursor
void liquidcrystal_i2c_no_autoscroll(LiquidCrystalI2C_t *mod) {
	mod->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	liquidcrystal_command(mod, LCD_ENTRYMODESET | mod->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void liquidcrystal_createchar(LiquidCrystalI2C_t *mod, uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	liquidcrystal_command(mod, LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		liquidcrystal_write(mod, charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void liquidcrystal_no_backlight(LiquidCrystalI2C_t *mod) {
	mod->_backlightval=LCD_NO_BACKLIGHT;
	expander_write(mod, 0);
}

void liquidcrystal_backlight(LiquidCrystalI2C_t *mod) {
	mod->_backlightval=LCD_BACKLIGHT;
	expander_write(mod, 1);
}


bool liquidcrystal_back(LiquidCrystalI2C_t *mod) {
  return mod->_backlightval == LCD_BACKLIGHT;
}