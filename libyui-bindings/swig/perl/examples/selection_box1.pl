#!/usr/bin/perl
#
# More advanced libyui example
#

use yui;

yui::YUILog::setLogFileName( "/tmp/libyui-examples.log" );
yui::YUILog::enableDebugLogging();

my $factory = yui::YUI::widgetFactory;
my $dialog = $factory->createPopupDialog;

my $vbox = $factory->createVBox( $dialog );

# $factory->createLabel( $vbox, "Hello, World!" );
# $factory->createPushButton( $vbox, "&OK" );
# my $event = $dialog->waitForEvent();
# $dialog->destroy();

my $selBox = $factory->createSelectionBox( $vbox, "&Menu" );

$selBox->addItem( "Pizza Margherita" );
$selBox->addItem( "Pizza Capricciosa" );
$selBox->addItem( "Pizza Funghi" );
$selBox->addItem( "Pizza Prosciutto" );
$selBox->addItem( "Pizza Quattro Stagioni" );
$selBox->addItem( "Calzone" );

my $hbox = $factory->createHBox( $vbox );
$valueField  = $factory->createOutputField( $hbox, "<SelectionBox value unknown>" );
$valueField->setStretchable( $yui::YD_HORIZ, 1 ); # // allow stretching over entire dialog width

$valueButton = $factory->createPushButton( $hbox, "&Value" ); 
$factory->createVSpacing( $vbox, 0.3 );

$rightAlignment = $factory->createRight( $vbox );
$closeButton    = $factory->createPushButton( $rightAlignment, "&Close" );

#
# Event loop
#

while (1) {
  print "Begin of loop";
  my $event = $dialog->waitForEvent();
  if( not event ) {
    next
  }
  print "Event !";
  if ($event->eventType() == yui::YEvent::CancelEvent) { # window manager "close window" button
    print "Cancel";
    break;
  }
  print "Not a cancel";
  $valueField->setValue( "???" );
  if ($event->widget() == $closeButton) {
    print "Close";
    break;
  }
  print "Not a close";

  if ( ($event->widget() == $valueButton) || ($event->widget() == $selBox )) {		# selBox will only send events with setNotify()
    $item = $selBox->selectedItem();
    print "selBox or valueButton";
    if ($item) {
      $valueField->setValue( $item->label() );
    }
    else {
      $valueField->setValue( "<none>" );
    }
    print "valueField set";
  }
}
print "Loop end";
