/* Do not remove the following line. Do not remove interrupt_handler(). */
#include "crt0.c"

void show_ball(int pos);
void play();
int  btn_check_0();
int  btn_check_1();
int  btn_check_3();
void led_set(int data);
void led_blink();
void lcd_wait(int n);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
void jump_up();
void jump_down();
void lcd_customchar(unsigned int addr, unsigned int *bitmap);

#define INIT	0
#define OPENING	1
#define PLAY	2
#define ENDING	3

#define FLOOR_MAX 16
#define HEIGHT 2
#define OBST_MAX 3

int state = INIT, pos = 0;
int counter;
int jump_state = 0;


int lcgs()
{
	static int key = 10;
	int m = 11213;
	int a = 200;
	int b = 4;
	return key = (key * a + b) % m;
}


void show_rand_floor(){
	/* array must not be defined in global */
	static unsigned long floor[FLOOR_MAX];
	int is_obstacle;
	int i;

	lcd_cmd(0xc0);	/* Set cursor position */

	/* initialize floor */
	if (counter == 0){
		for (i = 0; i < FLOOR_MAX; i++) 
			floor[i] = 0;
	}

	/* update floor */
	if (counter % 10 == 0) {
    	for (i = 1; i < FLOOR_MAX; i++) {
    	    floor[i - 1] = floor[i];
    	}

		/* deny damage object next to damage object */
		is_obstacle = 0;
		for (i = 0; i < OBST_MAX; i++) {
			if (floor[FLOOR_MAX - i - 2] == 1) {
				is_obstacle = 1;
				floor[FLOOR_MAX - 1] = 0;
				break;
			}
		}
		if (is_obstacle == 0) {
			floor[FLOOR_MAX- 1] = (lcgs() / 11) % 2;
		}
	}

	/* show floor */
	for (i = 0; i < FLOOR_MAX; i++) {
		if(floor[i] == 0){
			lcd_data(' ');
		} else {
			lcd_data('x');
		}
	}
	counter++;
	
}

/* interrupt_handler() is called every 100msec */
void interrupt_handler()
{
	static int cnt;
	int i;
	if (state == INIT) {
	} else if (state == OPENING) {
		cnt = 0;
	} else if (state == PLAY) {
    lcd_cmd(0x01);		/* Clear display */
		switch (jump_state) {
			case 0:
			show_rand_floor();
      show_character();
			break;

			case 1:
			show_rand_floor();
      show_character();
			jump_state = 2;
			break;

			case 2:
			show_rand_floor();
			show_character();
      jump_state = 0;
			break;
		}
	} else if (state == ENDING) {
	}
}
void main()
{
	unsigned int bitmap[7] = { 0x0e, /* 01110 */
                              0x0e, /* 01110 */
                              0x0e, /* 01110 */
                              0x04, /* 00100 */
                              0x1f, /* 11111 */
                              0x0a, /* 01010 */
                              0x11, /* 10001 */
};
	while (1) {
		if (state == INIT) {
			lcd_init();
			lcd_customchar(0x03, bitmap);
			state = OPENING; 
		} else if (state == OPENING) {
			state = PLAY;
		} else if (state == PLAY) {
			play();
			state = ENDING;
		} else if (state == ENDING) {
			state = OPENING;
		}
	}
}
void play()
{
	while (1) {
		/* Button0 is pushed when the ball is in the left edge */
		if (jump_state == 0 && btn_check_3()) {
			jump_state = 1;
		}
	}
}
void show_ball(int pos)
{
	lcd_cmd(0x01);		/* Clear display */
	lcd_cmd(0x80 + pos);	/* Set cursor position */
	lcd_data('*');
}

void show_ball_line2(int pos)
{
	lcd_cmd(0x01);		/* Clear display */
	lcd_cmd(0xc0 + pos);	/* Set cursor position */
	lcd_data('*');
}

void show_character()
{
  switch(jump_state){
    case 0:
    lcd_cmd(0xc0);
    lcd_data(0x03);
    break;

    case 1:
    jump_up();
    break;

    default:
    jump_down();
    break;
  }
}
/*
 * Switch functions
 */
int btn_check_0()
{
	volatile int *sw_ptr = (int *)0xff04;;
	return (*sw_ptr & 0x10) ? 1 : 0;
}
int btn_check_1()
{
	volatile int *sw_ptr = (int *)0xff04;;
	return (*sw_ptr & 0x20) ? 1 : 0;
}
int btn_check_3()
{
	volatile int *sw_ptr = (int *)0xff04;;
	return (*sw_ptr & 0x80) ? 1 : 0;
}
/*
 * LED functions
 */
void led_set(int data)
{
	volatile int *led_ptr = (int *)0xff08;
	*led_ptr = data;
}
void led_blink()
{
	int i;
	led_set(0xf);			/* Turn on */
	for (i = 0; i < 300000; i++);	/* Wait */
	led_set(0x0);			/* Turn off */
	for (i = 0; i < 300000; i++);	/* Wait */
	led_set(0xf);			/* Turn on */
	for (i = 0; i < 300000; i++);	/* Wait */
	led_set(0x0);			/* Turn off */
}
void lcd_wait(int n)
{
	int i;
	for (i = 0; i < n; i++);
}
void lcd_cmd(unsigned char cmd)
{
	/* E, RS, RW, DB[7:0] */
	volatile int *lcd_ptr = (int *)0xff0c;
	*lcd_ptr = (0x000000ff & cmd) | 0x00000000; /* E=0,RS=0,RW=0 */
	lcd_wait(1);
	*lcd_ptr = (0x000000ff & cmd) | 0x00000400; /* E=1,RS=0,RW=0 */
	lcd_wait(2);
	*lcd_ptr = (0x000000ff & cmd) | 0x00000000; /* E=0,RS=0,RW=0 */
	lcd_wait(11389);
}
void lcd_data(unsigned char data)
{
	/* E, RS, RW, DB[7:0] */
	volatile int *lcd_ptr = (int *)0xff0c;
	*lcd_ptr = (0x000000ff & data) | 0x00000200; /* E=0,RS=1,RW=0 */
	lcd_wait(1);
	*lcd_ptr = (0x000000ff & data) | 0x00000600; /* E=1,RS=1,RW=0 */
	lcd_wait(2);
	*lcd_ptr = (0x000000ff & data) | 0x00000200; /* E=0,RS=1,RW=0 */
	lcd_wait(278);
}
void lcd_init()
{
	lcd_wait(104167);
	lcd_cmd(0x38); /* 8-bit, 2-line mode */
	lcd_cmd(0x06); /* Cursor auto increment */
	lcd_cmd(0x0c); /* Display ON */
	lcd_cmd(0x01); /* Clear display */
	lcd_wait(10417);
}

void lcd_customchar(unsigned int addr, unsigned int *bitmap)
{
lcd_cmd((addr << 3) | 0x40); /* Set CGRAM address */
lcd_data(bitmap[0]);
lcd_data(bitmap[1]);
lcd_data(bitmap[2]);
lcd_data(bitmap[3]);
lcd_data(bitmap[4]);
lcd_data(bitmap[5]);
lcd_data(bitmap[6]);
lcd_data(0x00); /* Last line is used by cursor */
lcd_cmd(0x80); /* Set DDRAM address (write to display) */
}

void jump_up()
{   
    // move to upper
    lcd_cmd(0x80);
    lcd_data(0x03);
}

void jump_down()
{
    // move to lower
    lcd_cmd(0xc0);
    lcd_data(0x03);
}