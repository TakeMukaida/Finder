#include "includes.h"

#include <tchar.h>

Serial::Serial(const char *ComNum){
	TCHAR com[10];
	_stprintf_s(com, 10, _T("%s"), ComNum);

	printf("\nConnecting to %s...\n", com);

	// Open COM port
	com_port = CreateFile((LPCSTR)com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// Cannot opened
	if (com_port == INVALID_HANDLE_VALUE) {
		printf("Cannot opened %s.\n", com);
	}
}

Serial::~Serial(){
	// Close COM port
	CloseHandle(com_port);
}

void Serial::baud(unsigned long baudrate){
	// Struct contains detailed informations
	DCB dcb;
	// Read current settings
	GetCommState(com_port, &dcb);
	
	if (com_port != INVALID_HANDLE_VALUE) {
		printf("Baudrate is %d.\n", baudrate);
	}
	
	// Change settings
	dcb.BaudRate = baudrate; // baoud rate
	dcb.ByteSize = 8; // data length
	dcb.Parity = NOPARITY; // parity
	dcb.StopBits = ONESTOPBIT; // stop bit length
	dcb.fOutxCtsFlow = FALSE; // flow cts when it's sending
	dcb.fRtsControl = RTS_CONTROL_ENABLE; //RTS flow

	// Apply settings
	SetCommState(com_port, &dcb);
}

size_t Serial::write(char byte){
	char* SendData = &byte; // send data
	DWORD SentDataLength; // actual sent data

	// Send to port
	WriteFile(com_port, SendData, 1, &SentDataLength, NULL);

	return size_t();
}

size_t Serial::write(const char buf[], int len){
	DWORD SendDataLength = len; // send data
	DWORD SentDataLength; // actual sent data

	// Send to port
	WriteFile(com_port, buf, SendDataLength, &SentDataLength, NULL);

	return SentDataLength;
}

int Serial::get_byte(){
	// Checking receive data length
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(com_port, &errors, &comStat);

	// Get length of receive data length
	int RecvDataLength = comStat.cbInQue;
	return RecvDataLength;
}

int Serial::getc(){
	// if not received...
	if (get_byte() < 1) {
		return -1;
	}
	
	char buf[1]; // receive data
	DWORD RecvLength; // actual received data length

	// get data
	bool result = ReadFile(com_port, buf, 1, &RecvLength, NULL);

	if (result == FALSE) {
		return -1;
	}
	else if ((buf[0] & 0xF0) != 0xD0) {
		return -1;
	}
	else {
		return (buf[0] & 0x0F);
	}
	return 0;
	
	/*
	char buf[2]; // receive data
	DWORD RecvLength; // actual received data length

	// get data
	bool result = ReadFile(com_port, buf, 2, &RecvLength, NULL);

	if (result == FALSE) {
		return -1;
	}
	else if (buf[0] != 0xF5) {
		return -1;
	}
	else {
		return buf[1];
	}
	return 0;
	*/
}

int Serial::clength(const char buf[]){
	char len = 0;
	for (len = 0; buf[len]; len++);
	return len;
}
