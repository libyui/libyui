#!/usr/bin/perl
#
# Trivial libyui example
#

use yui;

my $dialog = yui::YUI->widgetFactory()->create_popup_dialog();
#my $vbox = ->create_vbox( $dialog );
#factory->create_label( $vbox, "Hello, World!" );
#factory->create_push_button( $vbox, "&OK" );
#my $event = $dialog->wait_for_event();
#$dialog->destroy();
