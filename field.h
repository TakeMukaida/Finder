#pragma once

#include "includes.h"

#define FIELD_OFFSET_X_Y 40
#define FIELD_X 560
#define FIELD_Y 1000
#define FENCE 15

#define Y1 60
#define Y2 240
#define Y3 315
#define Y4 390
#define Y5 500
#define Y_OFFSET 350

#define X1 180
#define X2 350
#define X3 550
#define X4 750
#define X5 925

#define ZONE 120
#define ZONE_1 430

#define POLE 280
#define POLE_BLOCK_W 35
#define POLE_BLOCK_L 40
#define POLE_BLOCK_X 140
#define POLE_BLOCK1_Y  430.2
#define POLE_INTERVAL 200
#define POLE_ADD_BLOCK (POLE + POLE_BLOCK_W)

#define T_SHIRT 100
#define SHEET 50
#define TOWEL 146
#define PARTITION 2
#define T_SHIRT_TABLE_WIDE 50
#define OTHER_TABLE_WIDE 40

#define FORWARD 1
#define REVERSE -1


class Field {
protected:
private:
	_u8 coat_color;
	_u8 red;
	_u8 blue;
	_u8 mode;
	_u8 key;
	cv::Point me;
public:
	Field(_u8 color);
	~Field();

	void change_color(_u8 color);
	void change_mode(_u8 m);
	void make_field();
	void setting_qualifying();
	void setting_final();
	void set_me(double x, double y);
	void set_me_rotate90(double x, double y);
	void set_me_rotateminus90(double x, double y);
	void dot_mapping(double x, double y);
	void dot_mapping_red(double x, double y);
	void watching_area_x(double x1, double x2);
	void watching_area_y(double y1, double y2);
	_u8 show_field();
	void show_text(int x, int y);
};
