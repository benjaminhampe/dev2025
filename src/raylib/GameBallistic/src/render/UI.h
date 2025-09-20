#pragma once 
#include "Menu.h"
#include "raygui.h"
#include "VisualUtilities.h"
#include "RenderBullet.h"
#include "string"


class MainUI
{

private:
	float userVelocity_;
	float userAngle_;

	char velocityInput_[6] = "400";
	char angleInput_[4] = "45";

	bool fireRequested_;

	bool velocityEditMode_;
	bool angleEditMode_;

public:
	MainUI()
	{
	 userVelocity_ = 400.0f;
	 userAngle_ = 45.0f;

	 fireRequested_ = false;

	 velocityEditMode_ = false;
	 angleEditMode_ = false;
	}
	void DrawUIPanel();

	// Velocity
	void DrawVelocityInput();
	void DrawAngleInput();
	// Fire button
	void DrawPlayButton();
	//Speed button
	void DrawTimeScaleToggle(float& timeScale);
	
	void DrawGridWithLabelsMeters(float pixelsPerMeter, Color lineColor, Color labelColor);

	void DrawObjectData(const RenderObject& bullet) const;
	//getters
	bool getFireRequested()const;
	void setFireRequested(bool fireRequested);
	float getUserVelocity()const;
	float getUserAngle()const;
};

