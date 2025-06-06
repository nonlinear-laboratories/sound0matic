cmake_minimum_required(VERSION 3.15)

project(sound0matic VERSION 0.1.0)

# Ensure GTK is found on Linux
if (UNIX AND NOT APPLE)
     include_directories(/usr/include/gtk-3.0)
    include_directories(/usr/include/atk-1.0)
    include_directories(/usr/include/cairo)
    include_directories(/usr/include/pango-1.0)
    include_directories(/usr/include/gdk-pixbuf-2.0)
    include_directories(/usr/include/glib-2.0)
    include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
endif()

include_directories(/usr/include/webkitgtk-4.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/include/harfbuzz)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)

include_directories(${WEBKIT_INCLUDE_DIRS})
link_directories(${WEBKIT_LIBRARY_DIRS})

# Enable plugin formats
set(JUCE_BUILD_VST3 ON)

# Install to testing path
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/dev/playground")

# Include JUCE from adjacent directory
add_subdirectory(${CMAKE_SOURCE_DIR}/../JUCE JUCE)

add_subdirectory(Source)

find_package(CURL REQUIRED)

if(TARGET sound0matic)
    target_link_libraries(sound0matic PRIVATE ${WEBKIT_LIBRARIES} CURL::libcurl)
endif()

set(JUCE_BUILD_AU OFF)
set(JUCE_BUILD_LV2 OFF)
