#pragma once 
#include <string>
#include "PhysicsConstants.h"
using namespace std;

class IObject
{
private:
	double v0_; //initial velocity
	double angle_; //angle in degrees
	string name_;
	Vector position_;
public:
	IObject(double v0, double angle, string name);
	void setInitialVelocity(double v0);
	void setAngle(double angle);
	double getInitialVelocity();
	double getAngle();
	Vector getPosition();
};