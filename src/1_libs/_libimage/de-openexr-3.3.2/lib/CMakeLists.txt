# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Contributors to the OpenEXR Project.

# message(STATUS "Building OpenEXR libraries")

# add_subdirectory(de-deflate-1.23)

add_subdirectory( Imath )
add_subdirectory( OpenEXR_Config )

add_subdirectory( Iex )
add_subdirectory( IlmThread )

add_subdirectory( OpenEXR_Core )
add_subdirectory( OpenEXR )
add_subdirectory( OpenEXR_Util )
