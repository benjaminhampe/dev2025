#include "Voxglitch_plugin.hpp"
#include "osdialog.h"
#include <rack_settings.hpp>

#include "vgLib-2.0/constants.h"

#include "vgLib-2.0/components/VoxglitchComponents.hpp"

using namespace vgLib_v2;

#include "VectorRotation/VectorRotation.hpp"
#include "VectorRotation/VectorRotationWidget.hpp"

Model* modelVectorRotation = createModel<VectorRotation, VectorRotationWidget>("vector_rotation");
