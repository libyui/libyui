#!/usr/bin/perl
#
# More advanced libyui example
#

use lib '../../../build/swig/perl';

use yui;

# yui::YUILog::setLogFileName( "/tmp/libyui-examples.log" );
# yui::YUILog::enableDebugLogging();

my $factory = yui::YUI::widgetFactory;
my $dialog = $factory->createPopupDialog;

my $vbox = $factory->createVBox( $dialog );

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
  $event = $dialog->waitForEvent();
  if( not event ) {
    next
  }
  if ($event->eventType() == $yui::YEvent::CancelEvent) { # window manager "close window" button
    last;
  }
  $valueField->setValue( "???" );
  if ($event->widget() == $closeButton) {
    last;
  }

  if ( ($event->widget() == $valueButton) or ($event->widget() == $selBox )) {		# selBox will only send events with setNotify()
    $item = $selBox->selectedItem();
    if ($item) {
      $valueField->setValue( $item->label() );
    }
    else {
      $valueField->setValue( "<none>" );
    }
  }
}
