#include "VisualUtilities.h"






void AdjustZoom(float& pixelsPerMeter, float zoomAmount)
{
    pixelsPerMeter += zoomAmount;
    const float minZoom =1.0f;
    const float maxZoom = 2000.0f;

    // Clamp to min/max zoom levels
    if (pixelsPerMeter < minZoom) pixelsPerMeter = minZoom;
    if (pixelsPerMeter > maxZoom) pixelsPerMeter = maxZoom;
}

void HandleZoomInput(float& pixelsPerMeter)
{
    float zoomStep = 0.1f;

    // Mouse wheel zoom
    int wheelMove = GetMouseWheelMove();
    if (wheelMove != 0)
    {
        AdjustZoom(pixelsPerMeter, wheelMove * zoomStep);
    }

    // Or keys: + to zoom in, - to zoom out
    if (IsKeyDown(KEY_KP_ADD) || IsKeyDown(KEY_EQUAL))  // '=' key near backspace
    {
        AdjustZoom(pixelsPerMeter, zoomStep);
    }
    if (IsKeyDown(KEY_KP_SUBTRACT) || IsKeyDown(KEY_MINUS))
    {
        AdjustZoom(pixelsPerMeter, -zoomStep);
    }
}

