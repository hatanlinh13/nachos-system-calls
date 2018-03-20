#include <syscall.h>

#ifndef NULL
#define NULL 0
#endif
#define SPACE 31

/*
 * if the character is not printable
 * then returns a short description for this character
 * and set the second argument into description length
 * otherwise returns NULL
 */
char *get_char_description(unsigned char, unsigned int *);

/*
 * returns a short description for the conresponding
 * ASCII control code
 * and set the second argument into description length
 * otherwise returns NULL
 */
char *ctrl_code_description(unsigned char, unsigned int *);

int
main()
{
	unsigned char c;
	unsigned int len;
	int j;
	char *str;
	PrintString("ASCII Table\n");
	for (c = 0x00; c <= 0x7f; c++) {
		/* uniform length index number */
		if (c < 10) {
			PrintChar('0');
			PrintChar('0');
		}
		else if (c < 100) {
			PrintChar('0');
		}
		PrintInt(c);
		PrintString(": ");
		str = get_char_description(c, &len);
		if (str != NULL) {
			PrintString(str);
			/* Fixed width */
			for (j = 0; j < SPACE - len; j++)
				PrintChar(' ');
		}
		else {
			PrintChar(c);
			/* Fixed width */
			for (j = 0; j < SPACE - 1; j++)
				PrintChar(' ');
		}
		/* terminate lines */
		if (c % 2 != 0)
			PrintString("\n");
	}
	return 0;
}

char *
get_char_description(unsigned char c, unsigned int *len)
{
	if      (c <= 0x1f) { /* Control codes */
		return ctrl_code_description(c, len);
	}
	else if (c == 0x20) { /* Space */
		*len = 11;
		return "    (Space)";
	}
	else if (c <= 0x7e) { /* Printable characters */
		return NULL;
	}
	else { /* c == 0x7f, Delete character */
		*len = 12;
		return "DEL (Delete)";
	}
}

char *
ctrl_code_description(unsigned char c, unsigned int *len)
{
	switch (c) {
	case 0x00:
		*len = 15;
		return "NUL (Null char)";
	case 0x01:
		*len = 22;
		return "SOH (Start of Heading)";
	case 0x02:
		*len = 19;
		return "STX (Start of Text)";
	case 0x03:
		*len = 17;
		return "ETX (End of Text)";
	case 0x04:
		*len = 25;
		return "EOT (End of Transmission)";
	case 0x05:
		*len = 13;
		return "ENQ (Enquiry)";
	case 0x06:
		*len = 20;
		return "ACK (Acknowledgment)";
	case 0x07:
		*len = 10;
		return "BEL (Bell)";
	case 0x08:
		*len = 16;
		return "BS  (Back Space)";
	case 0x09:
		*len = 20;
		return "HT  (Horizontal Tab)";
	case 0x0a:
		*len = 15;
		return "LF  (Line Feed)";
	case 0x0b:
		*len = 18;
		return "VT  (Vertical Tab)";
	case 0x0c:
		*len = 15;
		return "FF  (Form Feed)";
	case 0x0d:
		*len = 21;
		return "CR  (Carriage Return)";
	case 0x0e:
		*len = 22;
		return "SO  (Shift Out / X-On)";
	case 0x0f:
		*len = 22;
		return "SI  (Shift In / X-Off)";
	case 0x10:
		*len = 22;
		return "DLE (Data Line Escape)";
	case 0x11:
		*len = 22;
		return "DC1 (Device Control 1)";
	case 0x12:
		*len = 22;
		return "DC2 (Device Control 2)";
	case 0x13:
		*len = 22;
		return "DC3 (Device Control 3)";
	case 0x14:
		*len = 22;
		return "DC4 (Device Control 4)";
	case 0x15:
		*len = 30;
		return "NAK (Negative Acknowledgement)";
	case 0x16:
		*len = 22;
		return "SYN (Synchronous Idle)";
	case 0x17:
		*len = 27;
		return "ETB (End of Transmit Block)";
	case 0x18:
		*len = 12;
		return "CAN (Cancel)";
	case 0x19:
		*len = 19;
		return "EM  (End of Medium)";
	case 0x1a:
		*len = 16;
		return "SUB (Substitute)";
	case 0x1b:
		*len = 12;
		return "ESC (Escape)";
	case 0x1c:
		*len = 20;
		return "FS  (File Separator)";
	case 0x1d:
		*len = 21;
		return "GS  (Group Separator)";
	case 0x1e:
		*len = 22;
		return "RS  (Record Separator)";
	case 0x1f:
		*len = 20;
		return "US  (Unit Separator)";
	default:
		return NULL;
	}
}
