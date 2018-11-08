# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON )

find_package( Qt5Core	   CONFIG REQUIRED )
find_package( Qt5Gui	   CONFIG REQUIRED )
find_package( Qt5Widgets   CONFIG REQUIRED )
find_package( Qt5Svg	   CONFIG REQUIRED )
find_package( Qt5X11Extras CONFIG REQUIRED )

set( CMAKE_INCLUDE_PATH ${QT_INCLUDES} ${CMAKE_INCLUDE_PATH} )

PROCESS_SOURCES()

target_link_libraries( ${TARGETLIB}
		       Qt5::Core
		       Qt5::Gui
		       Qt5::Widgets
		       Qt5::Svg
		       Qt5::X11Extras
		      )

SUBDIRS( icons )
