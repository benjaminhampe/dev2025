#pragma once
#include <juce_gui_basics/juce_gui_basics_config.h>

namespace juce
{

//==============================================================================
/**
    Contains status information about a mouse wheel event.

    @see MouseListener, MouseEvent

    @tags{GUI}
*/
struct MouseWheelDetails  final
{
    //==============================================================================
    /** The amount that the wheel has been moved in the X axis.

        If isReversed is true, then a negative deltaX means that the wheel has been
        pushed physically to the left.
        If isReversed is false, then a negative deltaX means that the wheel has been
        pushed physically to the right.
    */
    float deltaX;

    /** The amount that the wheel has been moved in the Y axis.

        If isReversed is true, then a negative deltaY means that the wheel has been
        pushed physically upwards.
        If isReversed is false, then a negative deltaY means that the wheel has been
        pushed physically downwards.
    */
    float deltaY;

    /** Indicates whether the user has reversed the direction of the wheel.
        See deltaX and deltaY for an explanation of the effects of this value.
    */
    bool isReversed;

    /** If true, then the wheel has continuous, un-stepped motion. */
    bool isSmooth;

    /** If true, then this event is part of the inertial momentum phase that follows
        the wheel being released. */
    bool isInertial;
};

//==============================================================================
/**
    Contains status information about a pen event.

    @see MouseListener, MouseEvent

    @tags{GUI}
*/
struct PenDetails  final
{
    /**
        The rotation of the pen device in radians. Indicates the clockwise rotation, or twist,
        of the pen. The default is 0.
    */
    float rotation;

    /**
        Indicates the angle of tilt of the pointer in a range of -1.0 to 1.0 along the x-axis where
        a positive value indicates a tilt to the right. The default is 0.
    */
    float tiltX;

    /**
        Indicates the angle of tilt of the pointer in a range of -1.0 to 1.0 along the y-axis where
        a positive value indicates a tilt toward the user. The default is 0.
    */
    float tiltY;
};


} // namespace juce
