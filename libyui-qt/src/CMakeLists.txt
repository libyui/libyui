INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

ADD_DEFINITIONS(
		-DY2LOG=\\\"QT4\\\" ${QT_DEFINITIONS}
		-DQT_LOCALEDIR=\\\"${CMAKE_INSTALL_PREFIX}/share/qt4/translations\\\"
		-DICONDIR=\\"${Y2QT_ICONDIR}\\"
		-DLOCALEDIR=\\\"${Y2QT_LOCALEDIR}\\\"
		-DLANG_FONTS_FILE=\\\"${CMAKE_INSTALL_PREFIX}/share/fonts/lang_fonts\\\"
		-DTHEMEDIR=\\\"${Y2QT_THEMEDIR}\\\"
		-DQTLIBDIR=\\\"${LIB_INSTALL_DIR}\\\"
		)

SET( qt_yast_plugin_SRCS

     QY2CharValidator.cc
     QY2ComboTabWidget.cc
     QY2DiskUsageList.cc
     QY2HelpDialog.cc
     QY2ListView.cc
     QY2Settings.cc
     QY2Styler.cc
     Y2CCQt.cc
     YFrameBuffer.cc
     YQAlignment.cc
     YQApplication.cc
     YQBarGraph.cc
     YQCheckBox.cc
     YQCheckBoxFrame.cc
     YQComboBox.cc
     YQDateField.cc
     YQDialog.cc
     YQDownloadProgress.cc
     YQDumbTab.cc
     YQEmpty.cc
     YQFrame.cc
     YQGenericButton.cc
     YQIconPool.cc
     YQImage.cc
     YQInputField.cc
     YQIntField.cc
     YQLabel.cc
     YQLayoutBox.cc
     YQLogView.cc
     YQMainWinDock.cc
     YQMenuButton.cc
     YQMultiLineEdit.cc
     YQMultiProgressMeter.cc
     YQMultiSelectionBox.cc
     YQOptionalWidgetFactory.cc
     YQPackageSelectorPlugin.cc
     YQPartitionSplitter.cc
     YQProgressBar.cc
     YQPushButton.cc
     YQRadioButton.cc
     YQRadioButtonGroup.cc
     YQReplacePoint.cc
     YQRichText.cc
     YQSelectionBox.cc
     YQSignalBlocker.cc
     YQSlider.cc
     YQSpacing.cc
     YQSquash.cc
     YQTable.cc
     YQTimeField.cc
     YQTimezoneSelector.cc
     YQTree.cc
     YQUI_builtins.cc
     YQUI_core.cc
     YQUI_widgets.cc
     YQUI_x11.cc
     YQWidgetCaption.cc
     YQWidgetFactory.cc
     YQWizard.cc
     YQWizardButton.cc
     YQBusyIndicator.cc
     )

QT4_AUTOMOC(${qt_yast_plugin_SRCS})

QT4_WRAP_UI(qt_yast_plugin_SRCS QHelpDialog.ui)

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
INCLUDE_DIRECTORIES( ${YAST_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${YAST_INCLUDE_DIR}/yui )

ADD_LIBRARY( py2qt SHARED ${qt_yast_plugin_SRCS})
TARGET_LINK_LIBRARIES( py2qt ${YAST_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${YAST_YCP_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${QT_QTGUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${YAST_PLUGIN_UI_LIBRARY} )
SET_TARGET_PROPERTIES( py2qt PROPERTIES SOVERSION 2 )

INSTALL(TARGETS py2qt LIBRARY DESTINATION ${YAST_PLUGIN_DIR} )

INSTALL(FILES lang_fonts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/fonts )

ADD_SUBDIRECTORY(pkg)

#SET_TARGET_PROPERTIES( y2lang_qt PROPERTIES PREFIX "" )

