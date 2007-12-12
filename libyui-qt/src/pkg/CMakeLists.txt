################################################################
# package selector
################################################################

FIND_PACKAGE(Zypp REQUIRED)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_QT3SUPPORT_INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/src/pkg" ${ZYPP_INCLUDE_DIR})

ADD_DEFINITIONS(
-DY2LOG=\\\"QT4\\\" ${QT_DEFINITIONS}
-DICONDIR=\\"${Y2QT_ICONDIR}\\"
-DLOCALEDIR=\\\"${Y2QT_LOCALEDIR}\\\"
-DTHEMEDIR=\\\"${Y2QT_THEMEDIR}\\\"
-DQTLIBDIR=\\\"${LIB_INSTALL_DIR}\\\"
-DHELPIMAGEDIR=\\\"${CMAKE_INSTALL_PREFIX}/share/YaST2/images\\\"
)
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -fmessage-length=0" )

SET(qt_yast_pkg_plugin_SRCS YQPackageSelectorBase.cc YQPackageSelector.cc YQPackageSelectorHelp.cc YQPatternSelector.cc YQPkgChangeLogView.cc YQPkgChangesDialog.cc YQPkgConflictDialog.cc YQPkgConflictList.cc YQPkgDependenciesView.cc YQPkgDescriptionDialog.cc YQPkgDescriptionView.cc YQPkgDiskUsageList.cc YQPkgDiskUsageWarningDialog.cc YQPkgFileListView.cc YQPkgGenericDetailsView.cc YQPkgLangList.cc YQPkgList.cc YQPkgObjList.cc YQPkgPatchFilterView.cc YQPkgPatchList.cc YQPkgPatternList.cc YQPkgProductDialog.cc YQPkgProductList.cc YQPkgRepoFilterView.cc YQPkgRepoList.cc YQPkgRpmGroupTagsFilterView.cc YQPkgSearchFilterView.cc YQPkgSelDescriptionView.cc YQPkgSelList.cc YQPkgSelMapper.cc YQPkgStatusFilterView.cc YQPkgTechnicalDetailsView.cc YQPkgTextDialog.cc YQPkgUpdateProblemFilterView.cc YQPkgVersionsView.cc YQSimplePatchSelector.cc QY2LayoutUtils.cc )

QT4_AUTOMOC(${qt_yast_pkg_plugin_SRCS})

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
INCLUDE_DIRECTORIES( ${YAST_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${YAST_INCLUDE_DIR}/yui )

ADD_LIBRARY( py2qt_pkg SHARED ${qt_yast_pkg_plugin_SRCS})
TARGET_LINK_LIBRARIES( py2qt_pkg ${YAST_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${YAST_YCP_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${YAST_PLUGIN_WFM_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${QT_QTGUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${YAST_PLUGIN_UI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${ZYPP_LIBRARY} )
SET_TARGET_PROPERTIES( py2qt_pkg PROPERTIES SOVERSION 2 )

INSTALL(TARGETS py2qt_pkg LIBRARY DESTINATION ${YAST_PLUGIN_DIR} )
