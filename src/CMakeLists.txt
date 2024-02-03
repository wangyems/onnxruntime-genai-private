set(GENERATORS_ROOT ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
add_subdirectory(models)
file(GLOB generator_srcs CONFIGURE_DEPENDS
   "${GENERATORS_ROOT}/*.h"
   "${GENERATORS_ROOT}/*.cpp"
   "${MODELS_ROOT}/*.h"
   "${MODELS_ROOT}/*.cpp"
)