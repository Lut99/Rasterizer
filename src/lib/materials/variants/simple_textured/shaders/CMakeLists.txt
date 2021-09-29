# Define the custom commands to compile the shaders
add_custom_target(simple_textured_shaders
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_textured_vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/vertex.glsl
    COMMAND glslc -fshader-stage=frag -o ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_textured_frag.spv ${CMAKE_CURRENT_SOURCE_DIR}/fragment.glsl
    COMMENT "Building SimpleTextured shaders..."
)

# Add it to the list of includes & linked libraries
list(APPEND SHADERS simple_textured_shaders)

# Carry the list to the parent scope
set(SHADERS "${SHADERS}" PARENT_SCOPE)
