#pragma once
class memoryLoop {
private:
	static void animationCheck();
	static void gravityCheck();
	static void setAnimationSpeed(short num);
	static void setGravityStatus(int num);
	static void setPosition(float x, float y, float z);
	static void setXPosition(float x);
	static void setYPosition(float y);
	static void setZPosition(float z);
	static float getXPosition();
	static float getYPosition();
	static float getZPosition();
public:
	static void MemoryLoop();
};

