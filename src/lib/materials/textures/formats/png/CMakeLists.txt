# Add the library
add_library(PngLoader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/PngLoader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/LodePNG.cpp)

# Set the dependencies for this library:
target_include_directories(PngLoader PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS PngLoader)

# # Also add nested directories
# add_subdirectory(tokenizer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)