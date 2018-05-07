# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_PATH ${QT_INCLUDES} ${CMAKE_INCLUDE_PATH})

PROCESS_SOURCES()
qt5_use_modules( ${TARGETLIB} Core Gui Widgets Svg X11Extras )
target_link_libraries(
  ${TARGETLIB} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Svg Qt5::X11Extras
  )
