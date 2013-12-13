# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_PATH ${QT_INCLUDES} ${CMAKE_INCLUDE_PATH})
set(Y2QT_ICONDIR ${CMAKE_INSTALL_PREFIX}/share/YaST2/theme/current)
add_definitions(
  -DICONDIR="${Y2QT_ICONDIR}"
  -DQT_LOCALEDIR="${QT_TRANSLATIONS_DIR}"
  -DQTLIBDIR="${QT_LIBRARY_DIR}"
  )

qt5_wrap_ui(${TARGETLIB}_SOURCES "${${TARGETLIB}_WRAP_UI}")

PROCESS_SOURCES()
qt5_use_modules( ${TARGETLIB} Core Gui Widgets Svg X11Extras )
