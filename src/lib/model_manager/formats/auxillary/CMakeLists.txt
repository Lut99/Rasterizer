# Add the library
add_library(ModelsAuxillary STATIC ${CMAKE_CURRENT_SOURCE_DIR}/DebugInfo.cpp)

# Set the dependencies for this library:
target_include_directories(ModelsAuxillary PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ModelsAuxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)