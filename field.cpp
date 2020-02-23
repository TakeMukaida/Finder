#include "includes.h"
#include "field.h"

cv::Mat img = cv::Mat::zeros(2 * ( FIELD_OFFSET_X_Y + FENCE ) + FIELD_X, 2 * ( FIELD_OFFSET_X_Y + FENCE ) + FIELD_Y, CV_8UC3);

Field::Field(_u8 color) { // 0: blue, 1: red
	coat_color = color;
}

Field::~Field() {
}

void Field::change_color(_u8 color){ // 0: final, 1: qualifying
	coat_color = color;
}

void Field::change_mode(_u8 m) {
	mode = m;
}

void Field::make_field() {
	img = cv::Mat::zeros(2 * (FIELD_OFFSET_X_Y + FENCE) + FIELD_X, 2 * (FIELD_OFFSET_X_Y + FENCE) + FIELD_Y, CV_8UC3);

	double red, blue;
	if (coat_color) {
		red = 200;
		blue = 0;
	}
	else {
		red = 0;
		blue = 255;
	}

	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + X1, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + X1, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(200, 200, 200), 5, 4); // line1x
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + X2, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + X2, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(200, 200, 200), 5, 4); // line2x
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + X3, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + X3, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(200, 200, 200), 5, 4); // line3x
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + X4, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + X4, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(200, 200, 200), 5, 4); // line4x
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + X5, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + X5, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(200, 200, 200), 5, 4); // line5x

	if (coat_color) { // red

#define POLE_BLOCK_X 140

		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + Y1), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + Y1), cv::Scalar(200, 200, 200), 5, 4); // line1y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + Y2), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + Y2), cv::Scalar(200, 200, 200), 5, 4); // line2y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + Y3), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + Y3), cv::Scalar(200, 200, 200), 5, 4); // line3y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + Y4), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + Y4), cv::Scalar(200, 200, 200), 5, 4); // line4y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + Y5), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + Y5), cv::Scalar(200, 200, 200), 5, 4); // line5y

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + ZONE_1), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + ZONE_1 + ZONE), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // start zone1 (red)

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + ZONE), cv::Scalar(200, 200, 200), -1, 4); // player zone (red)
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE * 2, FIELD_OFFSET_X_Y + FENCE + ZONE), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // start zone2 (red)
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + ZONE), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + ZONE * 2), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // support zone (red)

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1

	}

	else { // blue

#define POLE_BLOCK_X 70

		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y1)), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y1)), cv::Scalar(200, 200, 200), 5, 4); // line1y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y2)), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y2)), cv::Scalar(200, 200, 200), 5, 4); // line2y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y3)), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y3)), cv::Scalar(200, 200, 200), 5, 4); // line3y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + Y_OFFSET, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y4)), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y4)), cv::Scalar(200, 200, 200), 5, 4); // line4y
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y5)), cv::Point(FIELD_OFFSET_X_Y + FENCE + FIELD_Y, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - Y5)), cv::Scalar(200, 200, 200), 5, 4); // line5y

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE - ZONE_1)), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE_1)), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // start zone1 (blue)

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE)), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Scalar(200, 200, 200), -1, 4); // player zone (blue)
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE)), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE * 2, FIELD_OFFSET_X_Y + FENCE + FIELD_X), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // start zone2 (blue)
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE * 2)), cv::Point(FIELD_OFFSET_X_Y + FENCE + ZONE, FIELD_OFFSET_X_Y + FENCE + (FIELD_X - ZONE)), cv::Scalar(blue / 2, 0, red / 2), -1, 4); // support zone (blue)

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_ADD_BLOCK), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L + POLE_INTERVAL * 2, FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + POLE_ADD_BLOCK), cv::Scalar(200, 200, 200), -1, 4); // pole1_block1

	}

	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE / 2, FIELD_OFFSET_X_Y + FENCE + FENCE / 2 + FIELD_X), cv::Point(FIELD_OFFSET_X_Y + FENCE + FENCE / 2 + FIELD_Y, FIELD_OFFSET_X_Y + FENCE / 2), cv::Scalar(blue, 0, red), 15, 4); // corner
	
	if (coat_color) {

		cv::rectangle(img, cv::Point(1 + FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + ZONE_1 + ZONE), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT, FIELD_OFFSET_X_Y + FENCE + ZONE_1 + ZONE + T_SHIRT_TABLE_WIDE), cv::Scalar(80, 80, 80), -1, 4); // tshirt
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION, FIELD_OFFSET_X_Y + FENCE + FIELD_X - 1), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET, FIELD_OFFSET_X_Y + FENCE + FIELD_X + OTHER_TABLE_WIDE), cv::Scalar(80, 80, 80), -1, 4); // sheet
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET + PARTITION, FIELD_OFFSET_X_Y + FENCE + FIELD_X - 1), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET + PARTITION + TOWEL, FIELD_OFFSET_X_Y + FENCE + FIELD_X + OTHER_TABLE_WIDE), cv::Scalar(80, 80, 80), -1, 4); // towel

	}

	else {

		cv::rectangle(img, cv::Point(1 + FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE - 1 - OTHER_TABLE_WIDE), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT, FIELD_OFFSET_X_Y + FENCE + T_SHIRT_TABLE_WIDE - OTHER_TABLE_WIDE), cv::Scalar(80, 80, 80), -1, 4); // tshirt
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION, FIELD_OFFSET_X_Y + FENCE - 1 - OTHER_TABLE_WIDE), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(80, 80, 80), -1, 4); // sheet
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET + PARTITION, FIELD_OFFSET_X_Y + FENCE - 1 - OTHER_TABLE_WIDE), cv::Point(FIELD_OFFSET_X_Y + FENCE + T_SHIRT + PARTITION + SHEET + PARTITION + TOWEL, FIELD_OFFSET_X_Y + FENCE), cv::Scalar(80, 80, 80), -1, 4); // towel

	}

}

void Field::setting_qualifying() {

	for (int i = 0; i < 3; i++) {
		
		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W / 2), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W * 1.5 + POLE), cv::Scalar(80, 80, 80), 5, 4); // pole

	}

	for (int i = 0; i < 3; i++) {

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 45), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 45 + 30), cv::Scalar(0, 200, 200), -1, 4);
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 125), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 125 + 30), cv::Scalar(0, 200, 200), -1, 4);
		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 205), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 205 + 30), cv::Scalar(0, 200, 200), -1, 4);
	
	}

}

void Field::setting_final() {

	for (int i = 0; i < 3; i++) {

		cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W / 2), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W * 1.5 + POLE), cv::Scalar(80, 80, 80), 5, 4); // pole

	}

	int i = 2;

	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 45), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 45 + 30), cv::Scalar(0, 200, 200), -1, 4);
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 125), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 125 + 30), cv::Scalar(0, 200, 200), -1, 4);
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 205), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 205 + 30), cv::Scalar(0, 200, 200), -1, 4);

	i--;

	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 26), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 26 + 30), cv::Scalar(0, 200, 200), -1, 4);
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 92), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 92 + 30), cv::Scalar(0, 200, 200), -1, 4);
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 158), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 158 + 30), cv::Scalar(0, 200, 200), -1, 4);
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 224), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 224 + 30), cv::Scalar(0, 200, 200), -1, 4);

	i--;

	for (int j = 0; j < 8; j++) {

		cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i - 2.5, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 8.75 + 35 * j), cv::Point(FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK1_Y + POLE_BLOCK_L / 2 + POLE_INTERVAL * i + 2.8, coat_color * 70 + FIELD_OFFSET_X_Y + FENCE + POLE_BLOCK_X + POLE_BLOCK_W + 8.75 + 35 * j + 17.5), cv::Scalar(0, 200, 200), -1, 4);

	}

}

void Field::set_me(double x, double y){
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + x - 80, FIELD_OFFSET_X_Y + y - 40), cv::Point(FIELD_OFFSET_X_Y + x + 0, FIELD_OFFSET_X_Y + y + 40), cv::Scalar(255 * coat_color, 50, 255 * (!coat_color)), 5, 4);
	me.x = x-33;
	me.y = y-20;
	cv::circle(img, cv::Point(FIELD_OFFSET_X_Y + x, FIELD_OFFSET_X_Y + y), 5, cv::Scalar(0, 255, 255), -1, CV_AA);
}

void Field::set_me_rotate90(double x, double y) {
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + x - 40, FIELD_OFFSET_X_Y + y - 80), cv::Point(FIELD_OFFSET_X_Y + x + 40, FIELD_OFFSET_X_Y + y + 0), cv::Scalar(255 * coat_color, 50, 255 * (!coat_color)), 5, 4);
	me.x = x - 33;
	me.y = y - 20;
	cv::circle(img, cv::Point(FIELD_OFFSET_X_Y + x, FIELD_OFFSET_X_Y + y), 5, cv::Scalar(0, 255, 255), -1, CV_AA);
}

void Field::set_me_rotateminus90(double x, double y) {
	cv::rectangle(img, cv::Point(FIELD_OFFSET_X_Y + x - 40, FIELD_OFFSET_X_Y + y + 80), cv::Point(FIELD_OFFSET_X_Y + x + 40, FIELD_OFFSET_X_Y + y + 0), cv::Scalar(255 * coat_color, 50, 255 * (!coat_color)), 5, 4);
	me.x = x - 33;
	me.y = y - 20;
	cv::circle(img, cv::Point(FIELD_OFFSET_X_Y + x, FIELD_OFFSET_X_Y + y), 5, cv::Scalar(0, 255, 255), -1, CV_AA);
}

void Field::dot_mapping(double x, double y){
	cv::circle(img, cv::Point(FIELD_OFFSET_X_Y + x, FIELD_OFFSET_X_Y + y), 1, cv::Scalar(80, 255, 0), -1, CV_AA);
}

void Field::dot_mapping_red(double x, double y) {
	cv::circle(img, cv::Point(FIELD_OFFSET_X_Y + x, FIELD_OFFSET_X_Y + y), 1, cv::Scalar(30, 0, 255), -1, CV_AA);
}

void Field::watching_area_x(double x1, double x2){
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + x1, FIELD_OFFSET_X_Y + FENCE), cv::Point(FIELD_OFFSET_X_Y + x1, FIELD_X + FIELD_OFFSET_X_Y + FENCE), cv::Scalar(0, 255, 255), 1, 4); // watching
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + x2, FIELD_OFFSET_X_Y + FENCE), cv::Point(FIELD_OFFSET_X_Y + x2, FIELD_X + FIELD_OFFSET_X_Y + FENCE), cv::Scalar(0, 255, 255), 1, 4); // watching
}

void Field::watching_area_y(double y1, double y2) {
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + y1), cv::Point(FIELD_Y + FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + y1), cv::Scalar(0, 255, 255), 1, 4); // watching
	cv::line(img, cv::Point(FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + y2), cv::Point(FIELD_Y + FIELD_OFFSET_X_Y + FENCE, FIELD_OFFSET_X_Y + FENCE + y2), cv::Scalar(0, 255, 255), 1, 4); // watching
}

_u8 Field::show_field() {
	if (mode) {
		setting_qualifying();
	}
	else {
		setting_final();
	}
	cv::imshow("Field", img);
	key = cv::waitKey(1);
	return key;
}

void Field::show_text(int x, int y) {
	std::stringstream sst_x, sst_y;
	sst_x << "x : " << x;
	sst_y << "y : " << y;
	cv::putText(img, sst_x.str(), me, CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255));
	me.y += 13;
	cv::putText(img, sst_y.str(), me, CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255));
}