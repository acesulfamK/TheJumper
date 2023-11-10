void interrupt_handler()
{
	static int cnt = 0;
	volatile int *seg7_ptr = (int *)0xff14;
	cnt++;
	if (cnt % 10 == 0)
		*seg7_ptr = cnt / 10;
}
void main()
{
	for (;;);
}
