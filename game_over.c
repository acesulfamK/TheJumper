void game_over()
{
  lcd_cmd(0x01); /* Clear display */
  lcd_cmd(0x86);
  lcd_data('G');
  lcd_data('A');
  lcd_data('M');
  lcd_data('E');
  lcd_cmd(0xc6);
  lcd_data('O');
  lcd_data('V');
  lcd_data('E');
  lcd_data('R');
  lcd_data(' ');
  lcd_data(0x04);
}

void main()
{
    unsigned int omg_face[7] = { 0x00, /* 00000 */
                              0x11, /* 10001 */
                              0x0a, /* 01010 */
                              0x11, /* 10001 */
                              0x00, /* 00000 */
                              0x04, /* 00100 */
                              0x00, /* 00000 */
};
    lcd_customchar(0x04, omg_face);
}