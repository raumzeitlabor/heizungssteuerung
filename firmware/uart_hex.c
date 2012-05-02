char uart_put_hex_helper(const char c) {
	switch (c) {
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		case 10: return 'a';
		case 11: return 'b';
		case 12: return 'c';
		case 13: return 'd';
		case 14: return 'e';
	}
	return 'f';
}

void uart_put_hexc(const char c) {
	usart0_putc(uart_put_hex_helper((c & 0xf0) >> 4));
	usart0_putc(uart_put_hex_helper(c & 0x0f));
}

void uart_put_hex(const char buf[], const int length) {
	for (int i = 0; i < length; i++) {
		uart_put_hexc(buf[i]);
	}
}
