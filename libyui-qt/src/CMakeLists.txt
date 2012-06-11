# auto-escape preprocessor defines
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  ${QT_INCLUDES}
  /usr/include/yui )

SET( ${TARGETLIB}_SOURCES
     QY2CharValidator.cc
     QY2ComboTabWidget.cc
     QY2DiskUsageList.cc
     QY2HelpDialog.cc
     QY2ListView.cc
     QY2Styler.cc
     YQAlignment.cc
     YQApplication.cc
     YQBarGraph.cc
     YQBusyIndicator.cc
     YQButtonBox.cc
     YQCheckBox.cc
     YQCheckBoxFrame.cc
     YQComboBox.cc
     YQContextMenu.cc
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
     YQGraphPluginStub.cc
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
     YQUI.cc
     YQUI_builtins.cc
     YQWidgetCaption.cc
     YQWidgetFactory.cc
     YQWizard.cc
     YQWizardButton.cc
     QY2StyleEditor.cc
)

SET( ${TARGETLIB}_HEADERS
     utf8.h
     QY2CharValidator.h
     QY2ComboTabWidget.h
     QY2DiskUsageList.h
     QY2HelpDialog.h
     QY2ListView.h
     QY2Styler.h
     YQi18n.h
     YQAlignment.h
     YQApplication.h
     YQBarGraph.h
     YQBusyIndicator.h
     YQButtonBox.h
     YQCheckBox.h
     YQCheckBoxFrame.h
     YQComboBox.h
     YQContextMenu.h
     YQDateField.h
     YQDialog.h
     YQDownloadProgress.h
     YQDumbTab.h
     YQEmpty.h
     YQFrame.h
     YQGenericButton.h
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
     YQGraphPluginIf.h
     YQGraphPluginStub.h
     YQPartitionSplitter.h
     YQProgressBar.h
     YQPushButton.h
     YQRadioButton.h
     YQRadioButtonGroup.h
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
     YQWizard.h
     YQWizardButton.h
     QY2StyleEditor.h
)

IF( DISABLE_SHARED )
  ADD_LIBRARY( ${TARGETLIB} STATIC ${${TARGETLIB}_SOURCES} )
ELSE( DISABLE_SHARED )
  ADD_LIBRARY( ${TARGETLIB} SHARED ${${TARGETLIB}_SOURCES} )
ENDIF( DISABLE_SHARED )

TARGET_LINK_LIBRARIES( ${TARGETLIB} ${LIB_LINKER} )

SET_TARGET_PROPERTIES(
  ${TARGETLIB} PROPERTIES
  SOVERSION ${SONAME_MAJOR}
  OUTPUT_NAME "${TARGETLIB}"
  PUBLIC_HEADER "${${TARGETLIB}_HEADERS}"
)

INSTALL(
  TARGETS ${TARGETLIB}
  EXPORT ${PROJECTNAME_UC}LibraryDepends
  LIBRARY DESTINATION "${INSTALL_LIB_DIR_PREFIX}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR_PREFIX}"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR_PREFIX}"
  COMPONENT dev
)

INSTALL(
  FILES ${${TARGETLIB}_HEADERS}
  DESTINATION "${INSTALL_INCLUDE_DIR_PREFIX}"
)

IF( ENABLE_STATIC AND NOT DISABLE_SHARED )

  ADD_LIBRARY( ${TARGETLIB}_static STATIC ${${TARGETLIB}_SOURCES} )
  TARGET_LINK_LIBRARIES( ${TARGETLIB}_static ${LIB_LINKER} )

  SET_TARGET_PROPERTIES(
    ${TARGETLIB}_static PROPERTIES
    SOVERSION ${SONAME_MAJOR}
    OUTPUT_NAME "${TARGETLIB}"
    PUBLIC_HEADER "${${TARGETLIB}_HEADERS}"
 )

  INSTALL(
    TARGETS ${TARGETLIB}_static
    EXPORT ${PROJECTNAME_UC}LibraryDepends
    LIBRARY DESTINATION "${INSTALL_LIB_DIR_PREFIX}"
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR_PREFIX}"
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR_PREFIX}"
      COMPONENT dev
   )

ENDIF( ENABLE_STATIC AND NOT DISABLE_SHARED )

IF( INSTALL_DOCS AND DOXYGEN_FOUND )
  ADD_DEPENDENCIES( ${TARGETLIB} docs )
ENDIF( INSTALL_DOCS AND DOXYGEN_FOUND )

QT4_AUTOMOC(${${TARGETLIB}_SOURCES})

QT4_WRAP_UI(${TARGETLIB}_SOURCES QHelpDialog.ui QStyleEditor.ui )

ADD_DEFINITIONS(
		-DQT_LOCALEDIR="${CMAKE_INSTALL_PREFIX}/share/qt4/translations"
		-DICONDIR="${YUIQT_ICONDIR}"
		-DLOCALEDIR="${YUIQT_LOCALEDIR}"
		-DTHEMEDIR="${YUIQT_THEMEDIR}"
		-DQTLIBDIR="${LIB_INSTALL_DIR}"
		-DLANG_FONTS_FILE="${CMAKE_INSTALL_PREFIX}/share/YaST2/data/lang_fonts"
		)

TARGET_LINK_LIBRARIES( ${TARGETLIB} ${QT_QTCORE_LIBRARY} )
TARGET_LINK_LIBRARIES( ${TARGETLIB} ${QT_QTGUI_LIBRARY} )
TARGET_LINK_LIBRARIES( ${TARGETLIB} yui )
TARGET_LINK_LIBRARIES( ${TARGETLIB} fontconfig )
