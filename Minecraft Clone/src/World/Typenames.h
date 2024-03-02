#pragma once
typedef unsigned long long int EventID;
typedef unsigned char BlockID;
typedef unsigned long long int EntityUUID; //UUID
typedef unsigned short EntityTypeID;

constexpr int FRONT = 0x00;
constexpr int BACK = 0x01;
constexpr int TOP = 0x02;
constexpr int BOTTOM = 0x03;
constexpr int RIGHT = 0x04;
constexpr int LEFT = 0x05;