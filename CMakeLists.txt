cmake_minimum_required(VERSION 3.16)

# Project Definition with C++17 Standard (Required for modern Qt/VTK)
project(AnatomyViewer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Dependency Resolution
# -----------------------------------------------------------------------------

# Find Qt 6 Components explicitly
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

# Find VTK 9 Components
# We rely on the specific modules needed for rendering and Qt integration.
find_package(VTK REQUIRED COMPONENTS
  CommonCore
  CommonDataModel
  IOImage               # For loading DICOM
  InteractionStyle      # For mouse interaction
  InteractionWidgets    # For slicing planes
  RenderingCore
  RenderingOpenGL2
  RenderingVolumeOpenGL2 # For GPU-accelerated volume rendering
  GUISupportQt          # The critical bridge module
  ViewsQt
)

# -----------------------------------------------------------------------------
# Source Management
# -----------------------------------------------------------------------------

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SceneManager.cpp
)

set(PROJECT_HEADERS
    src/MainWindow.h
    src/SceneManager.h
)

# Qt Auto-MOC handles the signal/slot mechanism generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# -----------------------------------------------------------------------------
# Target Definition
# -----------------------------------------------------------------------------

add_executable(AnatomyViewer ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Link Libraries
target_link_libraries(AnatomyViewer
    PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets
    ${VTK_LIBRARIES}
)

# VTK Module Initialization
# Essential for registering rendering factories (OpenGL2)
vtk_module_autoinit(
    TARGETS AnatomyViewer
    MODULES ${VTK_LIBRARIES}
)
