#include "RenderBullet.h"

RenderObject::RenderObject(Bullet* bullet) : bullet_(bullet), position_({0.0f,0.0f}), t_(0.0f)
{
	cout << "new bullet created!" <<position_.x <<position_.y << t_<< endl;
	trail_.push_back(position_);
}


void RenderObject::setPosition(float deltatime)
{
	deltatime *= timeScale;
	t_ += deltatime;

	float v0 = bullet_->getInitialVelocity();
	float theta = bullet_->getAngle() * (pi / 180.0f); //convert to radians

	
	if (t_ == 0)
	{
		trail_.push_back(position_); //drawing initial postion
	}
	//Physics computation

	position_.x = v0 * cosf(theta) * t_;
	position_.y = v0 * sinf(theta) * t_ - 0.5f * GRAVITY * (t_ * t_); //kinematic equation

	

	trail_.push_back(position_);
	
}



Vector RenderObject::getPositionPixels(float pixelsPerMeter) const {
	return {
		position_.x * pixelsPerMeter + graphArea.x,
	    graphArea.y + graphArea.height - position_.y * pixelsPerMeter // invert Y
	};
}

Vector RenderObject::getTrailPositionPixels(size_t i, float pixelsPerMeter) const {
	return {
		trail_[i].x * pixelsPerMeter + graphArea.x,
		graphArea.y+graphArea.height - trail_[i].y * pixelsPerMeter
	};
}

void RenderObject::renderObject(float& pixelsPerMeter)
{
	Vector p1 = this->getPositionPixels(pixelsPerMeter);
	if (p1.x < graphArea.width +graphArea.x &&
		p1.y < graphArea.height+ graphArea.y)
		DrawCircleV(Vector2{ p1.x, p1.y }, 5, YELLOW);
	
}

void RenderObject::renderTrail(float &pixeslPerMeter) 
{
	for (size_t i = 1; i < this->getTrail().size(); ++i) {
		Vector p1 = this->getTrailPositionPixels(i - 1, pixeslPerMeter);
		Vector p2 = this->getTrailPositionPixels(i, pixeslPerMeter);
		DrawLineV({ p1.x, p1.y }, { p2.x, p2.y }, RED);
	}
}
Bullet* RenderObject::getBullet()const 
{
	return bullet_;
}

Vector RenderObject::getPostition()const 
{
	return position_;
}

float RenderObject::getCurrentTime() const
{
	return 0.0f;
}

void RenderObject::setTrail(Vector position)
{
	trail_.push_back(position);
}

vector<Vector> RenderObject::getTrail()
{
	return trail_;
}

Vector RenderObject::getPositionAt(size_t index)
{
	return trail_.at(index);
}