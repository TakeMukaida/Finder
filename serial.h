#pragma once

typedef void *HANDLE;

class Serial
{
private:
	HANDLE com_port;
public:
	Serial(const char *ComNum);
	~Serial();
	
	void baud(unsigned long baudrate);

	virtual size_t write(char byte);

	virtual size_t write(const char buf[], int len);

	virtual int get_byte();

	virtual int getc();

	virtual int clength(const char buf[]);
};

