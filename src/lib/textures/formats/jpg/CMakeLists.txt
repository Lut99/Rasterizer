# Add the library
add_library(JpgLoader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/JpgLoader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/jpgd.cpp)

# Set the dependencies for this library:
target_include_directories(JpgLoader PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS JpgLoader)

# # Also add nested directories
# add_subdirectory(tokenizer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)