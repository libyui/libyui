#!/usr/bin/perl

use lib '../../../build/swig/perl';
use yui;
use Data::Dumper;
use Net::Twitter;

my $nt;
my $factory = yui::YUI::widgetFactory;

sub login {
    my $username = shift;
    my $password = shift;

    $nt = Net::Twitter::Lite->new( 
	    username => $username,
	    password => $password,
	);
}

sub post {
    my $newdialog = $factory->createPopupDialog;
	    
    my $min = $factory->createMinWidth($newdialog, 45);
    my $vvbox = $factory->createVBox( $min );
    my $textbox = $factory->createMultiLineEdit($vvbox, "What's up?");
    
    my $hbox = $factory->createHBox( $vvbox );

    my $left = $factory->createLeft( $hbox );
    my $statuslabel = $factory->createLabel($left, "Ready ...");
    my $right = $factory->createRight( $hbox );
    my $update = $factory->createPushButton( $right, "Post Update..." );
    my $cruft = $factory->createVSpacing($vvbox, 1);
    my $cancel = $factory->createPushButton( $vvbox, "Quit" );
   
    while ( 1 ) {
        $event = $newdialog->waitForEvent();

	if ( not event) {
	    next
	}

	# break the loop, quit

    	if ($event->widget() == $cancel ) {
	    last;
    	}

	# post an update

    	if ($event->widget() == $update ) {
	   my $text = $textbox->value();
	   eval { $nt->update($text) };

    	   if ( $@ ) {
		$statuslabel->setValue( "Update failed!" );
	   }
	   else {
		$statuslabel->setValue( "Update OK :)");
		$textbox->setValue("");
	   }
	   next;
        } 
    }
    # clean up
    $newdialog->destroy();
}

my $dialog = $factory->createPopupDialog;

####################################################
#                                                  # 
#  dialog					   #
#	vbox					   #
#	    label				   #
#	    input field				   #
#	    passwd field			   #
#	    spacing				   #
#	    hbox				   #
#		pushbutton	    		   #
#		pushbutton	    		   #
#						   #
####################################################

my $vbox = $factory->createVBox( $dialog );
my $label = $factory->createLabel( $vbox, "Login to your Twitter account");
my $username = $factory->createInputField($vbox, "User");
$username->setStretchable( $yui::YD_HORIZ, 1 );
my $pass = $factory->createPasswordField($vbox,"Password");
$pass->setStretchable( $yui::YD_HORIZ, 1 );

my $fillup = $factory->createVSpacing($vbox, 1);
my $buttonbox = $factory->createHBox( $vbox );
my $cancel = $factory->createPushButton( $buttonbox, "Cancel" );
my $login = $factory->createPushButton( $buttonbox, "Login" );

while ( 1 ) {
    $event = $dialog->waitForEvent();
    if ( not event ) {
	next
    }

    if ($event->widget() == $cancel ) {
	$dialog->destroy();
	last;
    }

    if ($event->widget() == $login ) {
	login( $username->value(), $pass->value() );
	$dialog->destroy();
	post;
	last;
    }
} 

