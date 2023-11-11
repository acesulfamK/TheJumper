/* Do not remove the following line. Do not remove interrupt_handler(). */
#include "crt0.c"
void show_ball(int pos);
void play();
int btn_check_0();
int btn_check_1();
int btn_check_3();
void led_set(int data);
void led_blink();
void lcd_wait(int n);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
void jump_up();
void jump_down();
#define INIT 0
#define OPENING 1
#define PLAY 2
#define ENDING 3
int state = INIT, pos = 0;
int jump_state = 0;
/* interrupt_handler() is called every 100msec */
void interrupt_handler()
{
  if (state == INIT) {
  } else if (state == OPENING) {
  } else if (state == PLAY) {
    switch (jump_state) {
      case 0:
      show_character();
      break;

      case 1:
      show_character();
      jump_state = 2;
      break;

      case 2:
      show_character();
      jump_state = 0;
      break;
    }
  } else if (state == OPENING) {
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
void show_character()
{
  lcd_cmd(0x01); /* Clear display */
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
  led_set(0xf); /* Turn on */
  for (i = 0; i < 300000; i++); /* Wait */
  led_set(0x0); /* Turn off */
  for (i = 0; i < 300000; i++); /* Wait */
  led_set(0xf); /* Turn on */
  for (i = 0; i < 300000; i++); /* Wait */
  led_set(0x0); /* Turn off */
}
/*
 * LCD functions
 */
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
  lcd_wait(10417);/* Not implemented yet */
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