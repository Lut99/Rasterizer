# Add the library
add_library(MtlLoader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/MtlLoader.cpp)

# Set the dependencies for this library:
target_include_directories(MtlLoader PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MtlLoader)

# Also add nested directories
add_subdirectory(tokenizer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)