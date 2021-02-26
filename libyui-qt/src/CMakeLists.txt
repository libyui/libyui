# keep preprocessor defines escaped
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)
  

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

ADD_DEFINITIONS(
		-DY2LOG=\\\"QT4\\\" ${QT_DEFINITIONS}
		-DQT_LOCALEDIR=\\\"${CMAKE_INSTALL_PREFIX}/share/qt4/translations\\\"
		-DICONDIR=\\"${Y2QT_ICONDIR}\\"
		-DLOCALEDIR=\\\"${Y2QT_LOCALEDIR}\\\"
		-DLANG_FONTS_FILE=\\\"${CMAKE_INSTALL_PREFIX}/share/YaST2/data/lang_fonts\\\"
		-DTHEMEDIR=\\\"${Y2QT_THEMEDIR}\\\"
		-DQTLIBDIR=\\\"${LIB_INSTALL_DIR}\\\"
		)

SET( qt_yast_plugin_SRCS

     QY2CharValidator.cc
     QY2ComboTabWidget.cc
     QY2DiskUsageList.cc
     QY2HelpDialog.cc
     QY2ListView.cc
     QY2Styler.cc
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
     YQPackageSelectorPluginStub.cc
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
     YQUI_x11.cc
     YQWidgetCaption.cc
     YQWidgetFactory.cc
     YQWizard.cc
     YQWizardButton.cc
     YQBusyIndicator.cc
     )

SET( yast_qt_HEADERS
     QY2CharValidator.h
     QY2ComboTabWidget.h
     QY2DiskUsageList.h
     QY2HelpDialog.h
     QY2ListView.h
     QY2Styler.h
     utf8.h
     YFrameBuffer.h
     YQAlignment.h
     YQApplication.h
     YQBarGraph.h
     YQBusyIndicator.h
     YQCheckBoxFrame.h
     YQCheckBox.h
     YQComboBox.h
     YQDateField.h
     YQDialog.h
     YQDownloadProgress.h
     YQDumbTab.h
     YQEmpty.h
     YQFrame.h
     YQGenericButton.h
     YQi18n.h
     YQImage.h
     YQInputField.h
     YQIntField.h
     YQLabel.h
     YQLayoutBox.h
     YQLogView.h
     YQMainWinDock.h
     YQMenuButton.h
     YQMultiLineEdit.h
     YQMultiProgressMeter.h
     YQMultiSelectionBox.h
     YQOptionalWidgetFactory.h
     YQPackageSelectorPluginIf.h
     YQPackageSelectorPluginStub.h
     YQPartitionSplitter.h
     YQProgressBar.h
     YQPushButton.h
     YQRadioButtonGroup.h
     YQRadioButton.h
     YQReplacePoint.h
     YQRichText.h
     YQSelectionBox.h
     YQSignalBlocker.h
     YQSlider.h
     YQSpacing.h
     YQSquash.h
     YQTable.h
     YQTimeField.h
     YQTimezoneSelector.h
     YQTree.h
     YQUI.h
     YQWidgetCaption.h
     YQWidgetFactory.h
     YQWizardButton.h
     YQWizard.h
)

INSTALL(  FILES ${yast_qt_HEADERS} DESTINATION "${LIBYUI_INCLUDE_DIR}/../yast2-qt" )

QT4_AUTOMOC(${qt_yast_plugin_SRCS})

QT4_WRAP_UI(qt_yast_plugin_SRCS QHelpDialog.ui)

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
INCLUDE_DIRECTORIES( ${LIBYUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${LIBYUI_INCLUDE_DIR}/yui )

ADD_LIBRARY( py2qt SHARED ${qt_yast_plugin_SRCS})
TARGET_LINK_LIBRARIES( py2qt ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt ${QT_QTGUI_LIBRARY} )
TARGET_LINK_LIBRARIES( py2qt dl X11 )
TARGET_LINK_LIBRARIES( py2qt fontconfig )
SET_TARGET_PROPERTIES( py2qt PROPERTIES SOVERSION 2 )
SET_TARGET_PROPERTIES( py2qt PROPERTIES LINK_FLAGS "-Wl,--no-undefined" )

INSTALL(TARGETS py2qt LIBRARY DESTINATION ${UI_PLUGIN_DIR} )

INSTALL(FILES lang_fonts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/YaST2/data )

#SET_TARGET_PROPERTIES( y2lang_qt PROPERTIES PREFIX "" )

