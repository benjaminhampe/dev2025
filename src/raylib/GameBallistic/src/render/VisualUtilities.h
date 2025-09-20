#pragma once
#include <iostream>
#include <raylib.h>


static  float PIXELS_PER_METER = 2.0f;
const int screenWidth = 600;
const int screenHeight = 1000;// adjust for zoom level
static const Rectangle graphArea = { 20, 20, 600, 300 };  // graph on left
static const Rectangle uiPanel = { 650, 50, 300, 500 }; // UI on right
static const Rectangle toolPanel = { 300, 320 , 100, 200 };
static float timeScale = 1.0f;
static bool slowMotion = false;

void HandleZoomInput(float& pixelsPerMeter);
void AdjustZoom(float& pixelsPerMeter, float zoomAmount);
