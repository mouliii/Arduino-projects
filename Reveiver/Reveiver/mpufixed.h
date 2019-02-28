#include "Arduino.h"
#include "Wire.h"

#define MPU6050_ADDR 0x68
#define MPU6050_ADDR1 0x69
#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1a
#define MPU6050_GYRO_CONFIG 0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6b
#define MPU6050_TEMP_H 0x41
#define MPU6050_TEMP_L 0x42
#define ACCEL_CONFIG_2G 2
#define ACCEL_CONFIG_4G 4
#define ACCEL_CONFIG_8G 8
#define ACCEL_CONFIG_16G 16
#define GYRO_CONFIG_250 250
#define GYRO_CONFIG_500 500
#define GYRO_CONFIG_1000 1000
#define GYRO_CONFIG_2000 2000

 class MPU6050
 {
 public: 
	MPU6050(TwoWire &w);
	MPU6050(TwoWire &w, float aC, float gC);
	void begin(int gyros = GYRO_CONFIG_500, int accel = ACCEL_CONFIG_8G, int addr = MPU6050_ADDR);
	void setGyroOffsets(float x, float y, float z);
	void writeMPU6050(byte reg, byte data);
	byte readMPU6050(byte reg);
	int16_t getRawAccX(){ return rawAccX; }; 
	int16_t getRawAccY(){ return rawAccY; };
	int16_t getRawAccZ(){ return rawAccZ; }; 
	int16_t getRawTemp(){ return rawTemp; };
	int16_t getRawGyroX(){ return rawGyroX; }; 
	int16_t getRawGyroY(){ return rawGyroY; };
	int16_t getRawGyroZ(){ return rawGyroZ; }; 
	float getTemp(){ return temp; };
	float getAccX(){ return accX; }; 
	float getAccY(){ return accY; };
	float getAccZ(){ return accZ; }; 
	float getGyroX(){ return gyroX; };
	float getGyroY(){ return gyroY; }; 
	float getGyroZ(){ return gyroZ; };
	void calcGyroOffsets(bool console = false);
	float getGyroXoffset(){ return gyroXoffset; }; 
	float getGyroYoffset(){ return gyroYoffset; }; 
	float getGyroZoffset(){ return gyroZoffset; };
	void update();
	float getAccAngleX(){ return angleAccX; }; 
	float getAccAngleY(){ return angleAccY; }; 
	float getGyroAngleX(){ return angleGyroX; }; 
	float getGyroAngleY(){ return angleGyroY; }; 
	float getGyroAngleZ(){ return angleGyroZ; }; 
	float getAngleX(){ return angleX; }; 
	float getAngleY(){ return angleY; }; 
	float getAngleZ(){ return angleZ; }; 
 private: 
	TwoWire *wire;
	int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ, mpuAddr;
	float gyroXoffset, gyroYoffset, gyroZoffset; 
	float temp, accX, accY, accZ, gyroX, gyroY, gyroZ; 
	float angleGyroX, angleGyroY, angleGyroZ, angleAccX, angleAccY, angleAccZ; 
	float angleX, angleY, angleZ; 
	long interval, preInterval; 
	float accCoef, gyroCoef, gyrosFactor, accelFactor, gForce, gForceInv; 
 };
