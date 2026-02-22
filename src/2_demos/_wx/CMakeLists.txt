# add_subdirectory(HexEditor-v0.24-wx) # Missing wxGraphicsContext

if (BENNI_USE_IRR AND BENNI_USE_WX)
    add_subdirectory(IrrlichtWidget2021-wx)
endif()
