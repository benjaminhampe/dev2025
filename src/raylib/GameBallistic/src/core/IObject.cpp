#include "IObject.h"

IObject::IObject(double v0, double angle, string name) : v0_(v0), angle_(angle), name_(name)
{
	
}
void IObject::setInitialVelocity(double v0)
{
	v0_ = v0;

}
void IObject::setAngle(double angle)
{
	angle_ = angle;
}

double IObject::getInitialVelocity()
{
	return v0_;
}

double IObject::getAngle()
{
	return angle_;
}

Vector IObject::getPosition()
{
	return position_;
}

