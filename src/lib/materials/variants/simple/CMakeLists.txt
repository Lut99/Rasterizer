# Define the custom commands to compile the shaders
add_custom_target(simple_shaders
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/vertex.glsl
    COMMAND glslc -fshader-stage=frag -o ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_frag.spv ${CMAKE_CURRENT_SOURCE_DIR}/fragment.glsl
    COMMENT "Building Simple shaders..."
)

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_TARGETS simple_shaders)

# Carry the list to the parent scope
set(EXTRA_TARGETS "${EXTRA_TARGETS}" PARENT_SCOPE)
