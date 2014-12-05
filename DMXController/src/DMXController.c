#include <gtk/gtk.h>			// GTK+3.0 graphics library
#include <dmx.h>			// DMX interface library
#include <stdio.h>			// strings for parsing inputs
#include <stdlib.h>			// for atoi

// constants and definitions

#define numChannels 512                       // # of DMX channels used


// global variables

static int      initDMX       ();
static void     exitDMX       ();


// ===========================================================================
//  main program
// ===========================================================================

int main( int argc, char *argv[] )
{
	// initialize DMX
	int error;
	error = initDMX();
	if ( error < 0 ) return ( error );

printf("running controller... %i \n", argc);

	if (argc > 2) {
		printf("setting value %s \n", argv[0]  );
		dmxSetValue( atoi( argv[1] ), atoi( argv[2] ) );
	}

//	fprintf(stdout, "running");
//
//	for ( char s[12]; (fgets(s, sizeof s, stdin)); ) {
//		int control, chn;
//		ubyte val;
//		control = atoi( s );
//		
//		chn = control >> 8;
//		val = (ubyte) control % 256;
//
//		fprintf(stdout, "%i %i\n", chn, val);
//
//		dmxSetValue( chn , val );
//	}
//
//	fprintf(stdout, "out of loop\n");


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

  dmxSetMaxChannels ( numChannels );


  // return valid status

  return ( 0 );


}


// ===========================================================================
// exitDMX -- terminate the DMX interface
// ===========================================================================

void exitDMX()
{

 // blackout
//	for ( int i = 0; i < numChannels; i++ ) {
//		dmxSetValue ( i , 0 );
//	}

  // close the DMX connection

  dmxClose();

}

