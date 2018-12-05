SET( ${TARGETLIB}_SOURCES
 YHttpServer.cc
 YHttpHandler.cc
 YHttpDialogHandler.cc
 YHttpRootHandler.cc
 YHttpAppHandler.cc
 YHttpWidgetsHandler.cc
 YHttpWidgetsActionHandler.cc
 YHttpMount.cc
 YJsonSerializer.cc
 YWidgetFinder.cc
)

SET( ${TARGETLIB}_HEADERS
 YHttpServer.h
 YHttpServerSockets.h
 YHttpHandler.h
 YHttpDialogHandler.h
 YHttpRootHandler.h
 YHttpAppHandler.h
 YHttpWidgetsHandler.h
 YHttpWidgetsActionHandler.h
 YHttpMount.h
 YWidgetFinder.h
 YJsonSerializer.h
)

SET( EXAMPLES_LIST
  ComboBox1.cc
  ComboBox1-editable.cc
  HelloWorld.cc
  SelectionBox1.cc
  SelectionBox2.cc
  SelectionBox3-many-items.cc
  Table-many-items.cc
  ManyWidgets.cc
)
