################################################################
# package selector
################################################################

ADD_DEFINITIONS(
		-DY2LOG=\\\"QT4\\\" ${QT_DEFINITIONS}
		-DICONDIR=\\"${Y2QT_ICONDIR}\\"
		-DLOCALEDIR=\\\"${Y2QT_LOCALEDIR}\\\"
		-DTHEMEDIR=\\\"${Y2QT_THEMEDIR}\\\"
		-DQTLIBDIR=\\\"${LIB_INSTALL_DIR}\\\"
		-DHELPIMAGEDIR=\\\"${CMAKE_INSTALL_PREFIX}/share/YaST2/images\\\"
		-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"
                -DSHOW_SINGLE_PRODUCT
		)
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -fmessage-length=0" )

SET(qt_yast_pkg_plugin_SRCS 

    QY2LayoutUtils.cc
    YQIconPool.cc
    YQPackageSelectorBase.cc
    YQPackageSelector.cc
    YQPackageSelectorHelp.cc
    YQPatternSelector.cc
    YQSimplePatchSelector.cc
    YQPkgChangeLogView.cc
    YQPkgChangesDialog.cc
    YQPkgConflictDialog.cc
    YQPkgConflictList.cc
    YQPkgDependenciesView.cc
    YQPkgDescriptionDialog.cc
    YQPkgDescriptionView.cc
    YQPkgDiskUsageList.cc
    YQPkgDiskUsageWarningDialog.cc
    YQPkgFileListView.cc
    YQPkgFilterTab.cc
    YQPkgGenericDetailsView.cc
    YQPkgHistoryDialog.cc
    YQPkgLangList.cc
    YQPkgList.cc
    YQPkgObjList.cc
    YQPkgPackageKitGroupsFilterView.cc
    YQPkgPatchFilterView.cc
    YQPkgPatchList.cc
    YQPkgPatternList.cc
    YQPkgProductDialog.cc
    YQPkgProductList.cc
    YQPkgRepoFilterView.cc
    YQPkgRepoList.cc
    YQPkgRpmGroupTagsFilterView.cc
    YQPkgSearchFilterView.cc
    YQPkgSelDescriptionView.cc
    YQPkgSelMapper.cc
    YQPkgStatusFilterView.cc
    YQPkgTechnicalDetailsView.cc
    YQPkgTextDialog.cc
    YQPkgUpdateProblemFilterView.cc
    YQPkgVersionsView.cc
    YQPackageSelectorPluginImpl.cc
    YQZyppSolverDialogPluginStub.cc
    )

QT4_AUTOMOC(${qt_yast_pkg_plugin_SRCS})

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
INCLUDE_DIRECTORIES( ${LIBYUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/include/YaST2/yast2-qt )
INCLUDE_DIRECTORIES( ${ZYPP_INCLUDE_DIR} ) 

ADD_LIBRARY( py2qt_pkg SHARED ${qt_yast_pkg_plugin_SRCS} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${UI_PLUGIN_DIR}/libpy2qt.so )
TARGET_LINK_LIBRARIES( py2qt_pkg ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${ZYPP_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt_pkg ${QT_QTGUI_LIBRARY} )
SET_TARGET_PROPERTIES( py2qt_pkg PROPERTIES SOVERSION 2 )
SET_TARGET_PROPERTIES( py2qt_pkg PROPERTIES LINK_FLAGS "-Wl,--no-undefined -Wl,-rpath -Wl,${UI_PLUGIN_DIR}" )

INSTALL(TARGETS py2qt_pkg LIBRARY DESTINATION ${UI_PLUGIN_DIR} )

ADD_SUBDIRECTORY( icons )

