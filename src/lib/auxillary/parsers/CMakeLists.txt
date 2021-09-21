# Specify the libraries in this directory
add_library(GeneralAuxillaryParsers STATIC ${CMAKE_CURRENT_SOURCE_DIR}/DebugInfo.cpp ${CMAKE_CURRENT_SOURCE_DIR}/TokenizerTools.cpp)

# Set the dependencies for this library:
target_include_directories(GeneralAuxillaryParsers PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS GeneralAuxillaryParsers)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
