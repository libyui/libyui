#!/usr/bin/perl
#
# Trivial libyui example
#
use lib '../../../build/swig/perl';

use yui;

my $factory = yui::YUI::widgetFactory;
my $dialog = $factory->createPopupDialog;

my $vbox = $factory->createVBox( $dialog );
$factory->createLabel( $vbox, "Hello, World!" );
$factory->createPushButton( $vbox, "&OK" );
my $event = $dialog->waitForEvent();
$dialog->destroy();
