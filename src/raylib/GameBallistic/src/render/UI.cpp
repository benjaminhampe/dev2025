#include "UI.h"

void MainUI::DrawUIPanel()
{
	DrawRectangleRec(uiPanel, LIGHTGRAY);
	DrawText("Input Parameters",
		uiPanel.x + 10,
		uiPanel.y + 10,
		20,
		DARKGRAY);
}

void MainUI::DrawVelocityInput()
{
	Rectangle v0Field = { uiPanel.x + 100, uiPanel.y + 50, 100, 30 };
	GuiTextBox(v0Field, velocityInput_, 6, velocityEditMode_);
	DrawText("v0 = ", v0Field.x - 30, v0Field.y + 10, 16, DARKGRAY);
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		velocityEditMode_ = CheckCollisionPointRec(GetMousePosition(), v0Field);

	}

}
void MainUI::DrawAngleInput()
{
	Rectangle angleField = { uiPanel.x + 100, uiPanel.y + 100, 100, 30 };
	GuiTextBox(angleField, angleInput_, 4, angleEditMode_);
	DrawText("angle = ", angleField.x - 50, angleField.y + 10, 14, DARKGRAY);
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		angleEditMode_ = CheckCollisionPointRec(GetMousePosition(), angleField);

	}
}

void MainUI::DrawPlayButton()
{
	Rectangle playButton = { uiPanel.x + 80, uiPanel.y + 150, 140, 40 };
	if (GuiButton(playButton, "Launch Bullet")) {
		userVelocity_ = atof(velocityInput_);
		userAngle_ = atof(angleInput_);
		fireRequested_ = true;
	}
}

void MainUI::DrawTimeScaleToggle(float& timeScale)
{
    Rectangle buttonNormalSpeed{ toolPanel.x, toolPanel.y + 20, 80, 30 };
	const char* label = "Speed: Normal";

	if (GuiButton(buttonNormalSpeed, label)) {
		slowMotion = false;
		timeScale = 1.0f;
	}

    Rectangle buttonSlowedSpeed{toolPanel.x - 100, toolPanel.y + 20, 60, 30 };
	const char* slowLabel = "Speed Slow";
	if (GuiButton(buttonSlowedSpeed, slowLabel))
	{
		slowMotion = true;
		timeScale = 0.1f;
	}

    Rectangle buttonPauseSpeed{ toolPanel.x - 30, toolPanel.y + 20, 20, 30 };
	const char* pauseLabel = "||";
	if (GuiButton(buttonPauseSpeed, pauseLabel))
	{
		slowMotion = true;
		timeScale = 0.0f;
	}

}

void MainUI::DrawObjectData(const RenderObject& bullet) const
{
	Rectangle horizontalData = { toolPanel.x - 250, toolPanel.y + 5, 50, 20 };
	//X:
	DrawText("x: ", horizontalData.x - 20, horizontalData.y, 16, DARKGRAY);
	DrawRectangle(horizontalData.x, horizontalData.y, horizontalData.width, horizontalData.height, LIGHTGRAY);
	string xCoordinate = to_string(bullet.getPostition().x);
	xCoordinate.resize(4);
	const char* bufferX = xCoordinate.c_str();
	DrawText(bufferX, horizontalData.x + 5, horizontalData.y + 4, 14, DARKGRAY);

	// Y: 
	DrawText("y: ", horizontalData.x - 20, horizontalData.y + 25, 16, GRAY);
	DrawRectangle(horizontalData.x, horizontalData.y + 25, horizontalData.width, horizontalData.height, LIGHTGRAY);
	string yCoordinate = to_string(bullet.getPostition().y);
	yCoordinate.resize(4);
	const char* bufferY = yCoordinate.c_str();
	DrawText(bufferY, horizontalData.x + 5, horizontalData.y + 29, 14, DARKGRAY);
}

bool MainUI::getFireRequested()const
{
	return fireRequested_;
}
void MainUI::setFireRequested(bool fireRequested)
{
	fireRequested_ = fireRequested;
}
float MainUI::getUserVelocity()const
{
	return userVelocity_;
}
float MainUI::getUserAngle()const
{
	return userAngle_;
}

void MainUI::DrawGridWithLabelsMeters(float pixelsPerMeter, Color lineColor, Color labelColor)
{
	
    int fontSize = 10;

    const int majorStepMeters = 50;
    const int minorStepMeters = 10;

    Color minorLineColor = Fade(lineColor, 0.3f);  // lighter color for sub-lines

    // Calculate visible world bounds (in meters)
    float leftMeters = 0.0f;
    float rightMeters = graphArea.width / pixelsPerMeter;
    float bottomMeters = 0.0f;
    float topMeters = graphArea.height / pixelsPerMeter;

    // Start drawing from the nearest lower multiple of step
    int startXMeters = (int)(leftMeters / minorStepMeters) * minorStepMeters;
    int endXMeters = (int)(rightMeters / minorStepMeters + 1) * minorStepMeters;

    int startYMeters = (int)(bottomMeters / minorStepMeters) * minorStepMeters;
    int endYMeters = (int)(topMeters / minorStepMeters + 1) * minorStepMeters;

    // Minor vertical lines
    for (int xMeters = startXMeters; xMeters <= endXMeters; xMeters += minorStepMeters) {
        float xPixel = graphArea.x + xMeters * pixelsPerMeter;
        DrawLine((int)xPixel, graphArea.y, (int)xPixel, graphArea.y + graphArea.height, minorLineColor);
    }

    // Minor horizontal lines
    for (int yMeters = startYMeters; yMeters <= endYMeters; yMeters += minorStepMeters) {
        float yPixel = graphArea.y + graphArea.height - yMeters * pixelsPerMeter; // invert Y
        DrawLine((int)graphArea.x, (int)yPixel, (int)(graphArea.x + graphArea.width), (int)yPixel, minorLineColor);
    }

    // Major vertical lines + labels
    for (int xMeters = startXMeters; xMeters <= endXMeters; xMeters += majorStepMeters) {
        float xPixel = graphArea.x + xMeters * pixelsPerMeter;
        DrawLine((int)xPixel, graphArea.y, (int)xPixel, graphArea.y + graphArea.height, lineColor);
        DrawText(TextFormat("%dm", xMeters), (int)xPixel + 2, graphArea.y + 5, fontSize, labelColor);
    }

    // Major horizontal lines + labels
    for (int yMeters = startYMeters; yMeters <= endYMeters; yMeters += majorStepMeters) {
        float yPixel = graphArea.y + graphArea.height - yMeters * pixelsPerMeter;
        DrawLine((int)graphArea.x, (int)yPixel, (int)(graphArea.x + graphArea.width), (int)yPixel, lineColor);
        DrawText(TextFormat("%dm", yMeters), graphArea.x + 5, (int)yPixel - fontSize / 2, fontSize, labelColor);
    }


}
