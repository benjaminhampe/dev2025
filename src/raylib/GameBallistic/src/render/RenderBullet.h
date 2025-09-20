#pragma once
#include "raylib.h"
#include "Bullet.h"
#include "PhysicsConstants.h"
#include "vector"
#include "VisualUtilities.h"
class RenderObject
{
private: 
	Bullet* bullet_;
	Vector position_;
	vector<Vector> trail_;
	float t_ = 0.0f;
public:
	RenderObject(Bullet * bullet);
	Bullet* getBullet()const ;
	Vector getPostition()const;
	float getCurrentTime() const;
	void setPosition(float deltatime);
	void setTrail(Vector position);
	vector<Vector> getTrail();
	Vector getPositionAt(size_t index);
	void renderTrail(float &pixelsPerMeter);
	void renderObject(float &pixelsPerMeter);
	Vector getPositionPixels(float pixelPerMeter) const;
	Vector getTrailPositionPixels(size_t i, float pixelsPerMeter) const;
};