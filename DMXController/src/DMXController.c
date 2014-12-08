#include <gtk/gtk.h>		// GTK+3.0 graphics library
#include <dmx.h>		// DMX interface library
#include "mongoose.h"		// Mongoose Web server
#include <string.h>		// for string manipulations
#include <stdio.h>		// needed for frozen
#include "frozen.h"		// frozen JSON parser
#include <stdlib.h>		// dealing with memory

// constants and definitions

#define numChannels 512                       // # of DMX channels used


// global variables

static int      initDMX       ();
static void     exitDMX       ();

ubyte values[numChannels]; 



// ===========================================================================
//  Mongoose Event Handler
// ===========================================================================

static int mg_ev_handler(struct mg_connection *conn, enum mg_event ev) {
	if (ev == MG_AUTH) {
		return MG_TRUE; // authorize all authorization reqs
	} else if (ev == MG_REQUEST && !strcmp(conn->uri, "/lights")) {
//		// spew some headers...
//		mg_printf_data(conn, "%s", "HTTP/1.1 200 OK\n\n");


		// if it's a POST, parse the POSTed data.
		if (!strcmp(conn->request_method, "POST")) {
			char *json = (char*) malloc(conn->content_len);
			strncpy(json, conn->content, conn->content_len);

			struct json_token *arr, *universe, *chan;

			// tokenize JSON string, fill in token array.
			arr = parse_json2(json, strlen(json)); // success is assumed, but seems safe-ish.

//			int arrLen = sizeof(arr) / sizeof(arr[0]);
//			printf("%i", arrLen);

			// search for universe
			universe = find_json_tok(arr, "1"); // currently only universe 1 is supported.

			if (universe!=NULL) {
//				printf("Value is %.*s\n", universe->len, universe->ptr);
				char vChr[3], cChr[3];
				for (int ch=1; ch<=numChannels; ch++) {
					sprintf(cChr, "%i", ch);
//					printf("%s\n", cChr);
					chan = find_json_tok(universe, cChr);
					if (chan!=NULL) {
						sprintf(vChr, "%.*s", chan->len, chan->ptr);
//						printf("spot1: %s\n", vChr);
						values[ch-1] = atoi(vChr);
//						printf("spot2: %i\n", values[ch-1]);
						dmxSetValue(ch, values[ch-1]);
//						printf("Value is %.*s\n", chan->len, chan->ptr);
					}
				}
			}

			// free some memory
			free(arr);

			// print the post data (debugging)
//			mg_printf_data(conn, "%s", json);
//			printf("%s", json);
		}

		// output json of alllllll the current values. 
		mg_printf_data(conn, "%s", "{\n");
		mg_printf_data(conn, "%s", "\t\"1\" : {\n");

		for (int i=1; i<=numChannels; i++) {
			mg_printf_data(conn, "%s%i%s%i%s", "\t\t\"", i, "\" : ", (int) values[i-1], ",\n");
		}
		mg_printf_data(conn, "%s", "\t\t\"running\" : true\n");

		mg_printf_data(conn, "%s", "\t}\n");
		mg_printf_data(conn, "%s", "}\n");
		return MG_TRUE;
	} else {
		return MG_FALSE;
	}
}




// ===========================================================================
//  main program
// ===========================================================================

int main( int argc, char *argv[] ) {
	// initialize mg
	struct mg_server *server = mg_create_server(NULL, mg_ev_handler);
	//mg_set_option(server, "document_root", ".");      // Serve current directory
	mg_set_option(server, "listening_port", "8080");  // Open port 8080

	// make it clear that we've actually started
	printf("Running...\n");

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

int initDMX() {

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

void exitDMX() {

	// blackout
	for (int i=1; i<numChannels; i++) {
		dmxSetValue( i , 0);
	}

	// close the DMX connection
	dmxClose();

}

