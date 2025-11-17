/*
	Cron.hpp
 	
	Author: Dieter Stubler

Copyright (C) 2019 Dieter Stubler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "OrangeLine.hpp"

//
// Defaults
//

#define HALF_PHASE_LENGTH_HISTORY  24
#define MIDI_CLK_PER_CLK 24

//
// Virtual Parameter Ids stored using Json
//
enum jsonIds {
	//
	// Parameters not bound to any user interface component to save internal module state
	//
    STYLE_JSON,
    LATENCY_JSON,
    LATENCY_JSON_LAST = LATENCY_JSON + POLY_CHANNELS - 1,

	NUM_JSONS
};

//
// Parameter Ids
//
enum ParamIds {
	//
	// Paramater for user interface components
	//
    CHL_PARAM,
    LATENCY_PARAM,

    NUM_PARAMS
};

//
// Input Ids
//
enum InputIds {
	CLKN_INPUT,
	CLK_INPUT,
    START_INPUT,
    STOP_INPUT,
    CONT_INPUT,
    CMP_INPUT,

	NUM_INPUTS
};

//
// Output Ids
//
enum OutputIds {
	CLK_OUTPUT,
    BPM_OUTPUT,
    RST_OUTPUT,
    RUN_OUTPUT,

    CMP_OUTPUT,
    CMP_OUTPUT_LAST = CMP_OUTPUT + POLY_CHANNELS - 1,
 
	NUM_OUTPUTS
};

//
// Ligh Ids
//
enum LightIds {

	NUM_LIGHTS
};
