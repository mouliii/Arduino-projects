#include "mpufixed.h" 

MPU6050::MPU6050(TwoWire &w)
{
	wire = &w; accCoef = 0.02f; gyroCoef = 0.98f;
}
MPU6050::MPU6050(TwoWire &w, float aC, float gC)
{
	wire = &w; accCoef = aC; gyroCoef = gC;
}
void MPU6050::begin(int gyros, int accel, int addr)
{
	int gyrosConfig;
	int accelConfig;
	mpuAddr = addr;
	switch (gyros)
	{
	case 250:
		gyrosConfig = 0x00;
		gyrosFactor = 131.0f;
		break;
	case 500:
		gyrosConfig = 0x08;
		gyrosFactor = 65.5f;
		break;
	case 1000:
		gyrosConfig = 0x10;
		gyrosFactor = 32.8f;
		break;
	case 2000:
		gyrosConfig = 0x18;
		gyrosFactor = 16.4f;
		break;
	default:
		gyrosConfig = 0x08;
		gyrosFactor = 65.5f;
		break;
	}
	switch (accel)
	{
	case 2:
		accelConfig = 0x00;
		accelFactor = 16384.0f;
		gForce = 2.0f;
		gForceInv = -2.0f;
		break;
	case 4:
		accelConfig = 0x08;
		accelFactor = 8192.0f;
		gForce = 4.0f;
		gForceInv = -4.0f;
		break;
	case 8:
		accelConfig = 0x10;
		accelFactor = 4096.0f;
		gForce = 8.0f;
		gForceInv = -8.0f;
		break;
	case 16:
		accelConfig = 0x18;
		accelFactor = 2048.0f;
		gForce = 16.0f;
		gForceInv = -16.0f;
		break;
	default:
		accelConfig = 0x00;
		accelFactor = 16384.0f;
		gForce = 2.0f;
		gForceInv = -2.0f;
		break;
	}
	writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);
	writeMPU6050(MPU6050_CONFIG, 0x00);
	writeMPU6050(MPU6050_GYRO_CONFIG, gyrosConfig);
	writeMPU6050(MPU6050_ACCEL_CONFIG, accelConfig); // 0x18 = 16G | 0x00 = 2G 
	writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);
	this->update();
	angleGyroX = this->getAccAngleX();
	angleGyroY = this->getAccAngleY();
	Serial.println("=================");
	Serial.println(accCoef);
	Serial.println(gyroCoef);
}
void MPU6050::writeMPU6050(byte reg, byte data)
{
	wire->beginTransmission(mpuAddr);
	wire->write(reg);
	wire->write(data);
	wire->endTransmission();
}
byte MPU6050::readMPU6050(byte reg)
{
	wire->beginTransmission(mpuAddr);
	wire->write(reg);
	wire->endTransmission(true);
	wire->requestFrom((uint8_t)mpuAddr, (size_t)2/*length*/);
	byte data = wire->read();
	wire->read();
	return data;
}
void MPU6050::setGyroOffsets(float x, float y, float z)
{
	gyroXoffset = x; gyroYoffset = y; gyroZoffset = z;
}
void MPU6050::calcGyroOffsets(bool console)
{
	float x = 0, y = 0, z = 0;
	float rx, ry, rz;
	if (console)
	{
		Serial.println("calculating gyro offsets");
		Serial.println("DO NOT MOVE A MPU6050");
	}
	for (int i = 0; i < 2000; i++)
	{
		if (console && i % 125 == 0)
		{
			Serial.print(".");
		}
		wire->beginTransmission(mpuAddr);
		wire->write(0x43); // from GYRO_XOUT_H to GYRO_ZOUT_L 
		wire->endTransmission(false);
		wire->requestFrom((int)mpuAddr, 6, (int)true);
		rx = wire->read() << 8 | wire->read(); // GYRO_XOUT_H | GYRO_XOUT_L
		ry = wire->read() << 8 | wire->read();
		rz = wire->read() << 8 | wire->read(); // GYRO_ZOUT_H | GYRO_ZOUT_L 
		x += ((float)rx) / gyrosFactor; // 500: 65.5
		y += ((float)ry) / gyrosFactor;
		z += ((float)rz) / gyrosFactor;
		delay(3);
	}
	gyroXoffset = x / 2000;
	gyroYoffset = y / 2000;
	gyroZoffset = z / 2000;

	if (console)
	{
		Serial.println("Done!!!");
		Serial.print("X : ");
		Serial.println(gyroXoffset);
		Serial.print("Y : ");
		Serial.println(gyroYoffset);
		Serial.print("Z : ");
		Serial.println(gyroYoffset);
		Serial.println("Program will start after 3 seconds");
		delay(3000);
	}
}
void MPU6050::update()
{
	wire->beginTransmission(mpuAddr);
	wire->write(0x3B); // from ACCEL_XOUT_H to GYRO_ZOUT_L 
	wire->endTransmission(false);
	wire->requestFrom((int)mpuAddr, 14, (int)true);

	rawAccX = wire->read() << 8 | wire->read(); // ACCEL_XOUT_H | ACCEL_XOUT_L
	rawAccY = wire->read() << 8 | wire->read();
	rawAccZ = wire->read() << 8 | wire->read();
	rawTemp = wire->read() << 8 | wire->read();
	rawGyroX = wire->read() << 8 | wire->read();
	rawGyroY = wire->read() << 8 | wire->read();
	rawGyroZ = wire->read() << 8 | wire->read(); // GYRO_ZOUT_H | GYRO_ZOUT_L 

	temp = (rawTemp + 12412.0) / 340.0;
	accX = ((float)rawAccX) / accelFactor; // 2G: 16384 | 16G: 2048 
	accY = ((float)rawAccY) / accelFactor;
	accZ = ((float)rawAccZ) / accelFactor;
	angleAccX = atan2(accY, accZ + abs(accX)) * 360 / gForce / PI; // 2.0 
	angleAccY = atan2(accX, accZ + abs(accY)) * 360 / gForceInv / PI; // -2.0 
	gyroX = ((float)rawGyroX) / gyrosFactor;
	gyroY = ((float)rawGyroY) / gyrosFactor;
	gyroZ = ((float)rawGyroZ) / gyrosFactor;

	interval = millis() - preInterval;

	gyroX -= gyroXoffset;
	gyroY -= gyroYoffset;
	gyroZ -= gyroZoffset;
	angleGyroX += gyroX * (interval * 0.001);
	angleGyroY += gyroY * (interval * 0.001);
	angleGyroZ += gyroZ * (interval * 0.001);

	preInterval = millis();

	angleX = (gyroCoef * angleGyroX) + (accCoef * angleAccX);
	angleY = (gyroCoef * angleGyroY) + (accCoef * angleAccY);
	angleZ = angleGyroZ;
}