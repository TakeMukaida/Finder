#include "pid.h"
#include "includes.h"


pid::pid( double p, double i, double d ) {
	Kp = p;
	Ki = i;
	Kd = d;
	integ = 0;
	En_1 = 0;
	En_2 = 0;
}

double pid::get_pid(double target, double now) {
	En = target - now;
	integ = Ki * En_2 + Ki * En_2 + Ki * En;
	b = integ + Kd * En + Kd * ((En - En_1) - (En_1 - En_2));
	En_1 = En;
	En_2 = En_1;
	if (fabs(b) > 127) {
		if (b < 0) {
			b = -127;
		}
		else {
			b = 127;
		}
	}
	return b;
}

pid::~pid() {

}
