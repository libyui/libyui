################################################################
# graph
################################################################

ADD_DEFINITIONS(
		-DY2LOG=\\\"QT4\\\" ${QT_DEFINITIONS}
		-DLOCALEDIR=\\\"${Y2QT_LOCALEDIR}\\\"
		-DQTLIBDIR=\\\"${LIB_INSTALL_DIR}\\\"
		-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"
		)
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -fmessage-length=0" )

SET(qt_yast_graph_plugin_SRCS
    QY2Graph.cc
    YQGraph.cc   
    YQGraphPlugin.cc
    YQGraphPluginImpl.cc
)

QT4_AUTOMOC(${qt_yast_graph_plugin_SRCS})

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
INCLUDE_DIRECTORIES( ${LIBYUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/include/YaST2/yast2-qt )
INCLUDE_DIRECTORIES( "/usr/include/graphviz" )

ADD_LIBRARY( py2qt_graph SHARED ${qt_yast_graph_plugin_SRCS} )
TARGET_LINK_LIBRARIES( py2qt_graph ${UI_PLUGIN_DIR}/libpy2qt.so )
TARGET_LINK_LIBRARIES( py2qt_graph ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_graph ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_graph ${QT_QTGUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_graph "-lgvc -lgraph -lcdt" )
SET_TARGET_PROPERTIES( py2qt_graph PROPERTIES SOVERSION 2 )
SET_TARGET_PROPERTIES( py2qt_graph PROPERTIES LINK_FLAGS "-Wl,--no-undefined -Wl,-rpath -Wl,${UI_PLUGIN_DIR}" )

INSTALL(TARGETS py2qt_graph LIBRARY DESTINATION ${UI_PLUGIN_DIR} )
