#pragma once 
#include "IObject.h"
#include "VisualUtilities.h"


class Bullet : public IObject
{
private:
	
public:
	Bullet(double v0, double angle, string name) :IObject(v0, angle, name) {};

};