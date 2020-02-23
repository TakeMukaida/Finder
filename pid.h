#pragma once
class pid
{
private:
	double Kp;
	double Ki;
	double Kd;
	double En;
	double En_1;
	double En_2;
	double b;
	double integ;

public:
	pid(double, double, double);

	double get_pid(double, double);

	~pid();
};