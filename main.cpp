#include <stdio.h>
#include <stdlib.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

// USER_SETTING ***************************************************************

#include "includes.h"
#define P 1.0
#define I 0.5
#define D 0.3
#define CAM 0

char FIELD_SIDE = 0; // 0: blue, 1: red

Field field(FIELD_SIDE);
pid vx(P, I, D);
pid vy(P, I, D);

int LIDAR_ANGLE = 0;

int test_phase = 0;

double LIDAR_X = 0;
double LIDAR_Y = 0;

_u8 key_flag = 1;
double x = 0, y = 0;
double buf = 0;
_u16 phase = 0;
_u8 mode_field = 0;

bool start_flag = false;

_u16 degree = 0;
signed short distance_x = 0, distance_y = 0;
signed short distance_x_send = 0, distance_y_send = 0;

signed short cnt_phase15 = 0;

char send_data[13] = { 0 };


//*****************************************************************************

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifdef _WIN32
#include <Windows.h>
#define delay(x)   ::Sleep(x)
#else
#include <unistd.h>
static inline void delay(_word_size_t ms) {
	while (ms >= 1000) {
		usleep(1000 * 1000);
		ms -= 1000;
	};
	if (ms != 0)
		usleep(ms * 1000);
}
#endif

using namespace rp::standalone::rplidar;

bool checkRPLIDARHealth(RPlidarDriver * drv)
{
	u_result     op_result;
	rplidar_response_device_health_t healthinfo;


	op_result = drv->getHealth(healthinfo);
	if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
		printf("RPLidar health status : %d\n", healthinfo.status);
		if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
			fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
			// enable the following code if you want rplidar to be reboot by software
			// drv->reset();
			return false;
		}
		else {
			return true;
		}

	}
	else {
		fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
		return false;
	}
}

#include <signal.h>
bool ctrl_c_pressed;
bool flags[10] = {false};
void ctrlc(int)
{
	ctrl_c_pressed = true;
}

int main(int argc, const char * argv[]) {

	printf("Robocon 2019 \"Run Run Lundly\"\n\n"
		"Based Ultra simple LIDAR data grabber for RPLIDAR.\n"
		"Version: \"%s\"\n\n"
		"argc: 5\n"
		"argv[0]: program name\n"
		"argv[1]: rplidar com port(com3)\n"
		"argv[2]: rplidar baudrate(115200)\n"
		"argv[3]: output com port(com2)\n"
		"argv[4]: output baudrate(115200)\n"
		"argv[5]: rplidar rotate speed(200 to 1023)\n\n"
		"type 'v': coat color change to red\n"
		"type 'b': coat color change to blue\n", RPLIDAR_SDK_VERSION);

	// USER_INIT ******************************************************************


	//PlaySound("C:/Users/Kinari/Music/robo/table.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
	
	const char * compath = NULL;
	if (argc>3) compath = argv[3];
	else compath = "com2";
	Serial serial(compath);
	//Serial serial("com4");
	if (argc>4) serial.baud(strtoul(argv[4], NULL, 10));
	else serial.baud(115200);

	_u8 test = 0;
	#define GEND 100
	
	cv::VideoCapture cap(CAM);

	if (!cap.isOpened()) {
		return -1;
	}

	cv::Scalar red_min(150, 100, 100);
	cv::Scalar red_max(180, 255, 255);
	cv::Scalar blue_min(80, 100, 100);
	cv::Scalar blue_max(120, 255, 255);
	cv::Mat source;

	char s = 0;

	//*****************************************************************************

	const char * opt_com_path = NULL;
	_u32         baudrateArray[2] = { 115200, 256000 };
	_u32         opt_com_baudrate = 0;
	u_result     op_result;

	bool useArgcBaudrate = false;

	// read serial port from the command line...
	if (argc>1) opt_com_path = argv[1]; // or set to a fixed value: e.g. "com3" 

										// read baud rate from the command line if specified...
	if (argc>2)
	{
		opt_com_baudrate = strtoul(argv[2], NULL, 10);
		useArgcBaudrate = true;
	}

	if (!opt_com_path) {
#ifdef _WIN32
		// use default com port
		opt_com_path = "com3";
#else
		opt_com_path = "/dev/ttyUSB0";
#endif
	}

	// create the driver instance
	RPlidarDriver * drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
	if (!drv) {
		fprintf(stderr, "insufficent memory, exit\n");
		exit(-2);
	}

	rplidar_response_device_info_t devinfo;
	bool connectSuccess = false;
	// make connection...
	if (useArgcBaudrate)
	{
		if (!drv)
			drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
		if (IS_OK(drv->connect(opt_com_path, opt_com_baudrate)))
		{
			op_result = drv->getDeviceInfo(devinfo);

			if (IS_OK(op_result))
			{
				connectSuccess = true;
			}
			else
			{
				delete drv;
				drv = NULL;
			}
		}
	}
	else
	{
		size_t baudRateArraySize = (sizeof(baudrateArray)) / (sizeof(baudrateArray[0]));
		for (size_t i = 0; i < baudRateArraySize; ++i)
		{
			if (!drv)
				drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
			if (IS_OK(drv->connect(opt_com_path, baudrateArray[i])))
			{
				op_result = drv->getDeviceInfo(devinfo);

				if (IS_OK(op_result))
				{
					connectSuccess = true;
					break;
				}
				else
				{
					delete drv;
					drv = NULL;
				}
			}
		}
	}
	if (!connectSuccess) {

		fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
			, opt_com_path);
		goto on_finished;
	}

	// print out the device serial number, firmware and hardware version number..
	printf("\nRPLIDAR S/N: ");
	for (int pos = 0; pos < 16; ++pos) {
		printf("%02X", devinfo.serialnum[pos]);
	}

	printf("\n"
		"Firmware Ver: %d.%02d\n"
		"Hardware Rev: %d\n"
		, devinfo.firmware_version >> 8
		, devinfo.firmware_version & 0xFF
		, (int)devinfo.hardware_version);



	// check health...
	if (!checkRPLIDARHealth(drv)) {
		goto on_finished;
	}

#ifndef SIGINT
#define SIGINT
#endif

	signal(SIGINT, ctrlc);

	drv->startMotor();
	if (argc>5) drv->setMotorPWM(atoi(argv[5]));
	//else drv->setMotorPWM(500);
	// start scan...
	drv->startScan(0, 1);
	// fetech result and print it out...

	// MAIN ***********************************************************************
	signed short log_distance_x[20] = { 1000, 0 };
	signed short log_distance_y[20] = { 0 };

	bool loop = true;

	printf("\nPlease push 'k' button.\n");

	while (start_flag) {
		key_flag = field.show_field();
		if (key_flag = 'k')
			start_flag = true;
	}

	printf("The program operation is started!\n");

	while (cap.read(source) && loop) {
		cv::Mat hsv;
		cv::Mat selected;
		cv::Mat image, result, result_copy;
		int min = 1000, px = 0, ax = 0, py = 0, ay = 0;
		int flag = 0;
		cv::GaussianBlur(source, source, cv::Size(5, 5), 0);
		cv::cvtColor(source, hsv, CV_BGR2HSV);
		if (s)
			cv::inRange(hsv, red_min, red_max, selected);
		else
			cv::inRange(hsv, blue_min, blue_max, selected);
		image = source;
		source.copyTo(result, selected);

		//輪郭の座標リスト
		std::vector< std::vector< cv::Point >> contours;
		//輪郭取得
		cv::findContours(selected, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		/*//輪郭描画
		for (auto contour = contours.begin(); contour != contours.end(); contour++) {
			cv::polylines(result, *contour, true, cv::Scalar(0, 255, 0), 2);
		}*/

		for (auto contour = contours.begin(); contour != contours.end(); contour++) {

			std::vector< cv::Point > approx;
			//輪郭の直線近似
			cv::approxPolyDP(cv::Mat(*contour), approx, 0.01 * cv::arcLength(*contour, true), true);
			//面積を求める
			double area = cv::contourArea(approx);

			if (area > 3000.0) {
				cv::polylines(result, approx, true, cv::Scalar(0, 200, 0), 2);
				std::stringstream sst;
				sst << "area : " << area;
				cv::putText(result, sst.str(), approx[0], CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255));

				//approx[0]のx座標を保存
				cv::Point o = approx[0];
				px = o.x;
				py = o.y;
				min = px;

				if (px >= 0 && py >= 0) {
					//approx[1]からのx座標とpxの比較
					//printf("00 : px = %d\n", px);
					for (int i = 1; i < approx.size(); ++i) {
						cv::Point p = approx[i];
						ax = p.x;
						ay = p.y;

						//printf("%.2d : ax = %d\n", i, ax);

						if (ay >= 0 && min > ax) {
							min = ax;
						}

					}
					//printf("min = %d\n", min);
					if ((ay >= 0 && ax < px || ay >= 0 && ax > px) && min < GEND) {
						flag = 1;
					}
				}
			}
		}
		if (flag == 1) {
			//printf("x座標がGEND以下になりました : %d\n", min);
		}
		cv::line(result, cv::Point(GEND, 0), cv::Point(GEND, 500), cv::Scalar(200, 200, 200), 3, 4);
		cv::imshow("img", result); 

		field.make_field();
		/*************************************************************************** SHOW */

		switch (phase) {	//Deg:90&-90, Robot:INUI
		case 0:	//バスタオル，シーツ回収
		case 2:	//シャツ回収
			if (FIELD_SIDE) {	//完了
				LIDAR_ANGLE = 90;
				LIDAR_X = distance_x - FENCE;
				LIDAR_X *= REVERSE;	//L<t
				LIDAR_Y = 560 - distance_y + FENCE;
				LIDAR_Y *= FORWARD; //L>t
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
			}
			else {	//完了
				LIDAR_ANGLE = -90;
				LIDAR_X = distance_x - FENCE;
				LIDAR_X *= REVERSE;	//L-t
				LIDAR_Y = distance_y - FENCE;
				LIDAR_Y *= REVERSE;	//L-t
				field.set_me_rotateminus90(LIDAR_X, LIDAR_Y);
			}
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			break;

		case 1:	//バスタオル，シーツ排出
		case 3:	//シャツ排出
			if (FIELD_SIDE) {
				LIDAR_ANGLE = -90;
				LIDAR_X = distance_x - FENCE;			
				LIDAR_X *= REVERSE;	//L-t
				LIDAR_Y = 560 - distance_y + FENCE;	 	
				LIDAR_Y *= FORWARD;	//L-t
				field.set_me_rotateminus90(LIDAR_X, LIDAR_Y);
			}
			else {	//完了				
				LIDAR_ANGLE = 90;
				LIDAR_X = distance_x - FENCE;
				LIDAR_X *= REVERSE;	//L<t
				LIDAR_Y = distance_y - FENCE;
				LIDAR_Y *= REVERSE; //L>t
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
			}
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);

			break;

		case 4:	//タオル掛け
			if (FIELD_SIDE) {
				LIDAR_ANGLE = 0;
				LIDAR_X = distance_y - FENCE;
				LIDAR_X *= REVERSE;	//L-t
				LIDAR_Y = 560 - distance_x + FENCE;
				LIDAR_Y *= FORWARD;	//L-t
				field.set_me(LIDAR_X, LIDAR_Y);
			}
			else {
				LIDAR_ANGLE = 0;
				LIDAR_X = distance_y - FENCE;
				LIDAR_X *= REVERSE;	//L-t
				LIDAR_Y = distance_x - FENCE;
				LIDAR_Y *= REVERSE;	//L-t
				field.set_me(LIDAR_X, LIDAR_Y);
			}
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			break;

			/*
		case 70:						//Ume's program case0 
			if (FIELD_SIDE) {
				LIDAR_ANGLE = 90;
				if (-distance_y < 120) {
					LIDAR_X = -distance_x + FENCE + 120;
					LIDAR_Y = -distance_y + FENCE;
				}
				else {
					LIDAR_X = -distance_x + FENCE;
					if (-distance_x < 120) {
						LIDAR_Y = -distance_y + FENCE + 120;
					}
					else {
						LIDAR_Y = -distance_y + FENCE;
					}
				}
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
				field.watching_area_y(LIDAR_Y - FENCE - 5, LIDAR_Y - FENCE + 5);
				field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			}
			else {
				LIDAR_ANGLE = -90;
				if (distance_y < 120) {
					LIDAR_X = -distance_x + FENCE + 120;
					LIDAR_Y = 560 - distance_y + FENCE;
				}
				else {
					LIDAR_X = -distance_x + FENCE;
					if (-distance_x < 120) {
						LIDAR_Y = 560 - distance_y + FENCE - 120;
					}
					else {
						LIDAR_Y = 560 - distance_y + FENCE;
					}
				}
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
				field.watching_area_y(LIDAR_Y - FENCE - 5, LIDAR_Y - FENCE + 5);
				field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			}
			break;

		case 80:
		case 81:
		case 82:
		case 84:
		case 86:
			if (FIELD_SIDE) {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;
				LIDAR_Y = 560 - distance_x - FENCE + 40 - 7.5;
				field.watching_area_y(465, 480);
			}
			else {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;
				LIDAR_Y = -distance_x - FENCE + 40 - 7.5;
				field.watching_area_y(80, 95);
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			break;

		case 83:
		case 88:
			if (FIELD_SIDE) {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;	//縦
				LIDAR_Y = -distance_x - FENCE + 40 - 7.5;	//横
				field.watching_area_y(150, 165);	//横
			}
			else {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;	//縦
				LIDAR_Y = 560 - distance_x - FENCE + 40 - 7.5;	//横
				field.watching_area_y(395, 410);
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);	//縦
			break;

		case 85:
		case 87:
			if (FIELD_SIDE) {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_X = 630 - distance_y + FENCE;
				LIDAR_Y = 560 - distance_x - FENCE + 40 - 7.5; 	
				LIDAR_ANGLE = 0;
			}
			else {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_X = 630 - distance_y + FENCE;
				LIDAR_Y = -distance_x - FENCE + 40 - 7.5;
				LIDAR_ANGLE = 0;
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			break;

		case 89:	//往路
			if (FIELD_SIDE) {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_y + FENCE;
				LIDAR_Y = -distance_x - FENCE + 40 - 7.5;
				LIDAR_ANGLE = 0;
			}
			else {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_y + FENCE;
				LIDAR_Y = 560 - distance_x - FENCE + 40 - 7.5;	
				LIDAR_ANGLE = 0;
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			break;

		case 810:
		case 814:
			if (FIELD_SIDE) {
				field.set_me_rotateminus90(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_x + FENCE;
				LIDAR_Y = -distance_y - FENCE + 40 - 7.5;
				LIDAR_ANGLE = -90;
			}
			else {
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_x + FENCE;
				LIDAR_Y = 560 - distance_y - FENCE + 40 - 7.5;
				LIDAR_ANGLE = 90;
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			field.watching_area_y(LIDAR_Y - 5, LIDAR_Y + 5);
			break;

		case 811:
		case 812:
		case 813:
			if (FIELD_SIDE) {
				field.set_me_rotateminus90(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_x + FENCE;
				LIDAR_Y = -distance_y + 35 + 140 - FENCE + 40 - 7.5;
				LIDAR_ANGLE = -90;
			}
			else {
				field.set_me_rotate90(LIDAR_X, LIDAR_Y);
				LIDAR_X = 1000 - distance_x + FENCE;
				LIDAR_Y = 280 - distance_y + 35 + 70 - FENCE + 40 - 7.5;
				LIDAR_ANGLE = 90;
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);
			field.watching_area_y(LIDAR_X - 5, LIDAR_X + 5);
			break;
		
		case 815:	//復路
			if (FIELD_SIDE) {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;	
				LIDAR_Y = -distance_x - FENCE + 40 - 7.5;	
				field.watching_area_y(150, 165);	
			}
			else {
				field.set_me(LIDAR_X, LIDAR_Y);
				LIDAR_ANGLE = 0;
				LIDAR_X = 432 - distance_y + FENCE;	
				LIDAR_Y = 560 - distance_x - FENCE + 40 - 7.5;	
				field.watching_area_y(395, 410);
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);	
			break;

		case 820:
			field.set_me(LIDAR_X, LIDAR_Y);
			LIDAR_ANGLE = 0;
			LIDAR_X = 432 - distance_y + FENCE;	//縦
			LIDAR_Y = 560 - distance_x + 40 - FENCE - 7.5;	//横
			if (FIELD_SIDE) {
				field.watching_area_y(150, 165);	//横
			}
			else {
				field.watching_area_y(395, 410);
			}
			field.watching_area_x(LIDAR_X - 5, LIDAR_X + 5);	//縦
			break;
			*/
		}
		field.show_text(distance_x_send, distance_y_send);

		/*********************************************************************** SHOW_END */


		/* simurate1 *********************************************/
		/*
		if (LIDAR_X > 390) {
			phase = 1;
		}
		if (LIDAR_Y > 340) {
			phase = 2;
		}

		if (phase == 0) {
			LIDAR_X += vx.get_pid(390.2, LIDAR_X)*0.1;
			LIDAR_Y += vy.get_pid(155, LIDAR_Y)*0.1;
		}
		else if (phase == 1) {
			LIDAR_X += vx.get_pid(390.2, LIDAR_X)*0.1;
			LIDAR_Y += vy.get_pid(345, LIDAR_Y)*0.06;
		}
		else if (phase == 2) {
			LIDAR_X += vx.get_pid(60, LIDAR_X)*0.1;
			LIDAR_Y += vy.get_pid(70, LIDAR_Y)*0.1;
		}
		*/
		/*********************************************************/

		int cnt = 1;

		rplidar_response_measurement_node_t nodes[8192];
		size_t   count = _countof(nodes);

		op_result = drv->grabScanData(nodes, count);

		if (IS_OK(op_result)) {
			drv->ascendScanData(nodes, count);
			for (int pos = 0; pos < (int)count; ++pos) {

				if ((nodes[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) >= 30) {
					x = LIDAR_X + ((nodes[pos].distance_q2 / 4.0f) * (cos(((nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f + LIDAR_ANGLE) / 360 * 2 * M_PI)) / 10);
					y = LIDAR_Y + ((nodes[pos].distance_q2 / 4.0f) * (sin(((nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f + LIDAR_ANGLE) / 360 * 2 * M_PI)) / 10);

					//printf("x:%f, y:%f, phase:%d\n", x, y, phase);

					field.dot_mapping(x, y);

					/********************************************************** GET DISTANCE*/
					switch (phase) {
					case 0:
					case 2:
						if (FIELD_SIDE) {	//完了
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y > LIDAR_Y) {
								distance_y = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_x = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						else {	//完了
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y < LIDAR_Y) {
								distance_y = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_x = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case 1:
					case 3:
						if (FIELD_SIDE) {	
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y > LIDAR_Y) {
								distance_y = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_x = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						else {	//完了
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y < LIDAR_Y) {
								distance_y = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_x = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case 4:
						if (FIELD_SIDE) {	
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y > LIDAR_Y) {
								distance_x = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_y = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						else {		
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && y < LIDAR_Y) {
								distance_x = (short)(y - LIDAR_Y);
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && x < LIDAR_X) {
								distance_y = (short)(x - LIDAR_X);
								field.dot_mapping_red(x, y);
							}
						}
						break;

					/*
					case  0:	//deg:90or-90, type:0//
					case  1:
					case  2:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 480)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 465))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {	//90度考慮
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE +  95)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE +  80))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								//上手く動作しなければ"x"を"-x"にしてみよ
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) < 0) {	//90度考慮
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case  3:	//deg:0, type:1//
					case  8:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 165)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 150))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) < 0) {	//90度考慮
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 410)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 395))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								//上手く動作しなければ"x"を"-x"にしてみよ
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {	//90度考慮
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case  4:	//deg:0, type:2
					case  5:
					case  6:
					case  7:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + !95)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + !80))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 410)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 395))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 550)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 350)))) {
								//上手く動作しなければ"x"を"-x"にしてみよ
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
							distance_x = y - LIDAR_Y;
							field.dot_mapping_red(x, y);
						}
						break;

					case  9:	//deg:0, type:3
					case 15:
						break;

					case 10:	//deg:90or-90, type:4//
					case 14:
						if (FIELD_SIDE) {
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) < 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {	//Field-y軸上部
							distance_y = y - LIDAR_Y;
							field.dot_mapping_red(x, y);
						}
						break;

					case 11:	//deg:90&-90, type:5
					case 12:
					case 13:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + !95)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + !80))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 860)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 840)))) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 860)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 840))) &&
								(((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 500)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 320)))) {
								//上手く動作しなければ"x"を"-x"にしてみよ
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {	//Field-y軸上部
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
						}
						break;
				/*
					case 90:						//凍結
					case 91:
					case 92:
						if (FIELD_SIDE) {
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_y = y - LIDAR_Y;
								if (distance_y > 430)
									distance_y += 570;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) < 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_y = y - LIDAR_Y;
								if (distance_y < 430)
									distance_y += 570;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case  84:
					case  86:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 175)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 180))) && (x - LIDAR_X) > 0 && (x - LIDAR_X) < 440) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 95)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 80))) && (x - LIDAR_X) > 0 && (230 - (x - LIDAR_X)) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) < 0) {
							distance_x = y - LIDAR_Y;
							field.dot_mapping_red(x, y);
						}
						break;

					case  85:
					case  87:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 175)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 180))) && (((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 750)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 550))) && (x - LIDAR_X) > 100 && (x - LIDAR_X) < 350) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							break;
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 95)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 80))) && (x - LIDAR_X) > 150 && (x - LIDAR_X) < 300) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							//		if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
							//			distance_y = x - LIDAR_X;
							//			field.dot_mapping_red(x, y);
							//		}
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) < 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							break;
						}
						break;

					case  89:	//往路//触れるな
						if (FIELD_SIDE) {
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && (y - LIDAR_Y) < 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}

						
						if (FIELD_SIDE) {
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {	//一応動いたから触るな
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if (flags[0] && (((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if ((x - LIDAR_X) < 10 && (x - LIDAR_X) > -10 && (y - LIDAR_Y) < 0 && (y - LIDAR_Y) > -120) {
								flags[0] = true;				//前段土台判定	

							}
						}
						
						break;


					case 810:
					case 814:
						if (!flags[0])
							flags[0] = false;

						if (FIELD_SIDE) {

						}
						else {
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) > 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && (y - LIDAR_Y) > 0) {
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case 811:
						if (FIELD_SIDE) {

						}
						else {
							if ((((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 860)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 840))) && (y - LIDAR_Y) > 0 && (y - LIDAR_Y) < 175) {
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) > 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case 812:
					case 813:
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 175)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 140))) && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((x + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 860)) && ((x + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 840))) && (y - LIDAR_Y) > 0 && (y - LIDAR_Y) < 280) {
								distance_y = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) > 0) {
								distance_x = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						break;

					case 815:	//復路
						if (FIELD_SIDE) {
							if ((x - LIDAR_X) < 5 && (x - LIDAR_X) > -5 && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((y - LIDAR_Y) < 5 && (y - LIDAR_Y) > -5 && (x - LIDAR_X) < 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
								distance_x = y - LIDAR_Y;
								field.dot_mapping_red(x, y);
							}
							if ((((y - LIDAR_Y) < 5) && ((y - LIDAR_Y) > -5)) && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}

	/*						if (flags[1] && (((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 410)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 395))) && (x - LIDAR_X) > 10 && (x - LIDAR_X) < 200) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
							if (     cnt_phase15 == 0 && (x - LIDAR_X) < 10 && (x - LIDAR_X) > -10 && (y - LIDAR_Y) < 0 && (y - LIDAR_Y) > -120)
								cnt_phase15 = 1;
							else if (cnt_phase15 == 1 && (x - LIDAR_X) < 10 && (x - LIDAR_X) > -10 && (y - LIDAR_Y) < -120)
								cnt_phase15 = 2;
							else if (cnt_phase15 == 2 && (x - LIDAR_X) < 10 && (x - LIDAR_X) > -10 && (y - LIDAR_Y) < 0 && (y - LIDAR_Y) > -120)
								cnt_phase15 = 3;
							else if (cnt_phase15 == 3 && (x - LIDAR_X) < 10 && (x - LIDAR_X) > -10 && (y - LIDAR_Y) < -120)
								flags[1] = true;
						}
						break;

					case 820:
						if (!flags[1])
							flags[1] = false;
						if (FIELD_SIDE) {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 175)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 140))) && (x - LIDAR_X) > 0) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						else {
							if ((((y + FIELD_OFFSET_X_Y) < (FIELD_OFFSET_X_Y + FENCE + 410)) && ((y + FIELD_OFFSET_X_Y) > (FIELD_OFFSET_X_Y + FENCE + 395))) && (x - LIDAR_X) > 0 && (x - LIDAR_X) < 440) {
								distance_y = x - LIDAR_X;
								field.dot_mapping_red(x, y);
							}
						}
						if ((((x - LIDAR_X) < 5) && ((x - LIDAR_X) > -5)) && (y - LIDAR_Y) > 0) {
							distance_x = y - LIDAR_Y;
							field.dot_mapping_red(x, y);
						}
						break;
				*/	
					}
				}
				else {
					x = 0;
					y = 0;
				}
				/* GET_DISTANCE_END */
				/*
				printf("%s theta: %03.2f Dist: %08.2f Q: %d x: %08.2f y: %08.2f\n",
				(nodes[pos].sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT) ? "S " : "  ",
				(nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f,
				nodes[pos].distance_q2 / 4.0f,
				nodes[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT, // 0 or 47
				x,
				y);
				*/
			}
		}

		key_flag = field.show_field();
		switch(key_flag) {
		case 'v':
			FIELD_SIDE = 1;
			field.change_color(FIELD_SIDE);
			break;

		case 'b':
			FIELD_SIDE = 0;
			field.change_color(FIELD_SIDE);
			break;
		
		case 'm':
			mode_field = !mode_field;
			field.change_mode(mode_field);
			break;
		
		case 's':
			s = !s;
			break;
		
		case 'q':
			loop = false;
			break;
		
		case '0':
			phase = 0;
			LIDAR_ANGLE = 0;
			break;

		case '1':
			phase = 1;
			LIDAR_ANGLE = 0;
			break;

		case '2':
			phase = 2;
			break;
		
		case '3':
			phase = 3;
			break;

		case '4':
			phase = 4;
			break;

		case '5':
			phase = 5;
			break;

		case '6':
			phase = 6;
			break;

		case '7':
			phase = 7;
			break;

		case '8':
			phase = 8;
			break;

		case '9':
			phase = 9;
			break;

		case 'w':
			phase = 10;
			break;

		case 'e':
			phase = 11;
			break;

		case 'r':
			phase = 12;
			break;

		case 't':
			phase = 13;
			break;

		case 'y':
			phase = 14;
			break;

		case 'u':
			phase = 15;
			break;

		case 'i':
			phase = 16;
			break;

		case 'o':
			phase = 17;
			break;

		case 'p':
			phase = 20;
			break;
		}

		if (ctrl_c_pressed) 
			break;

		switch (phase) {
		case 0:
		case 1:
		case 2:
		case 3:
			distance_x_send = distance_y;
			distance_y_send = distance_x;
			break;

		case 4:	//no change
			distance_x_send = distance_x;
			distance_y_send = distance_y;
			break;
		}

		// 10001000 0cpppppp 0000xxyy 0xxxxxxx 0xxxxxxx 0yyyyyyy 0yyyyyyy (to send 7bytes + header, c:color, p:phase, x:distance_x, y:distance_y)
		send_data[6] = distance_y_send & 0x7F;
		send_data[5] = (distance_y_send >> 7) & 0x7F;
		send_data[4] = distance_x_send & 0x7F;
		send_data[3] = (distance_x_send >> 7) & 0x7F;
		send_data[2] = (distance_x_send >> 12) & 0x0C | (distance_y_send >> 14) & 0x03;
		send_data[1] = (phase & 0x3F) | (FIELD_SIDE << 6);
		send_data[0] = 0x8B;

		serial.write(send_data, 7);
		/*
		// 10001000 0fffffff 0cpppppp 0000xxyy 0xxxxxxx 0xxxxxxx 0yyyyyyy 0yyyyyyy (to send 7bytes + header, d:degree, x:distance_x, y:distance_y)
		send_data[7] = ((distance_y)) & 0x7F;
		send_data[6] = ((distance_y) >> 7) & 0x7F;
		send_data[5] = ((distance_x)) & 0x7F;
		send_data[4] = ((distance_x) >> 7) & 0x7F;
		send_data[3] = ((distance_x >> 12) & 0x0C) + ((distance_y >> 14) & 0x03);
		send_data[2] = (phase & 0x0F) + (FIELD_SIDE << 6);
		send_data[1] = 0x00;
		for (cnt = 0; cnt <= 5; cnt++)
			send_data[1] |= (flags[cnt] << cnt);
		send_data[1] |= (flag << 6);
		send_data[0] = 0x88;

		serial.write(send_data, 8);
		*/
		/*
		send_data[11] = ((distance_y_2)) & 0x7F;
		send_data[10] = ((distance_y_2) >> 7) & 0x7F;
		send_data[9]  = ((distance_x_2)) & 0x7F;
		send_data[8]  = ((distance_x_2) >> 7) & 0x7F;
		send_data[7]  = ((distance_y)) & 0x7F;
		send_data[6] = ((distance_y) >> 7) & 0x7F;
		send_data[5]  = ((distance_x)) & 0x7F;
		send_data[4]  = ((distance_x) >> 7) & 0x7F;
		send_data[3]  = ((distance_x >> 12) & 0x0C) + ((distance_y >> 14) & 0x03);
		send_data[2]  =   phase        & 0x7F;
		send_data[1]  =  (phase >>  7) & 0x7F;
		send_data[0]  = ((phase >> 14) & 0x7F) + 0x80;

		serial.write(send_data, 12);
		*/

		/*
		for (int i = 0; i < 8; i++) {
			printf("%d\t", send_data[i]);
		*/
		//test_phase = serial.getc();		//mbedからphaseデータ取得

		printf("dis_x:%4d, dis_y:%4d, phase:%d\n", distance_x_send, distance_y_send, phase);
	}

	drv->stop();
	drv->stopMotor();
	// done!
	on_finished:
	RPlidarDriver::DisposeDriver(drv);
	drv = NULL;
	cv::destroyAllWindows();
	return 0;
}

