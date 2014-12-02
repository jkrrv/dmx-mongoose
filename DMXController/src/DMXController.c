// ===========================================================================
// DMXWheel - a simple program that sets the color on a DMX device using
//            the color wheel selector on the screen
// ===========================================================================


// ===========================================================================

#include <gtk/gtk.h>                        // GTK+3.0 graphics library
#include <dmx.h>                            // DMX interface library
#include <mongoose.h>						// Mongoose Web server


// constants and definitions

#define RedChannel  2                         // DMX channel for red control
#define GrnChannel  3                         // DMX channel for green control
#define BluChannel  4                         // DMX channel for blue control
#define NumChannels 3                         // # of DMX channels used


// global variables

static int      initDMX       ();
static void     setDMXColor   ( gdouble, gdouble, gdouble );
static void     exitDMX       ();


// ===========================================================================
//  main program
// ===========================================================================

int main( int argc, char *argv[] )
{
	// initialize mg
	struct mg_server *server = mg_create_server(NULL, NULL);
	mg_set_option(server, "document_root", ".");      // Serve current directory
	mg_set_option(server, "listening_port", "8080");  // Open port 8080


	// initialize DMX
	int error;
	error = initDMX();
	if ( error < 0 ) return ( error );
	
	
	/* do stuff */
	for (;;) {
		mg_poll_server(server, 1000);   // Infinite loop, Ctrl-C to stop
	}


	// kill mg
	mg_destroy_server(&server);

	// kill DMX
	exitDMX();
	
	// die quietly
	return ( 0 );
}

// ===========================================================================
// initDMX -- initialize DMX interface
// ===========================================================================

int initDMX()
{

  // open DMX interface

  int success = dmxOpen();
  if ( success < 0 ) return ( success );


  // configure

  dmxSetMaxChannels ( NumChannels );


  // return valid status

  return ( 0 );


}

// ===========================================================================
// setDMXColor -- set the color values for the DMX device
// ===========================================================================

void setDMXColor ( gdouble red, gdouble grn, gdouble blu )
{

  // convert values to unsigned bytes

  ubyte redVal = (ubyte) ( 255.0f * red );
  ubyte grnVal = (ubyte) ( 255.0f * grn );
  ubyte bluVal = (ubyte) ( 255.0f * blu );


  // set the channel colors

  dmxSetValue ( RedChannel , redVal );
  dmxSetValue ( GrnChannel , grnVal );
  dmxSetValue ( BluChannel , bluVal );
}


// ===========================================================================
// exitDMX -- terminate the DMX interface
// ===========================================================================

void exitDMX()
{

 // blackout

  dmxSetValue ( RedChannel , 0 );
  dmxSetValue ( GrnChannel , 0 );
  dmxSetValue ( BluChannel , 0 );


  // close the DMX connection

  dmxClose();

}

