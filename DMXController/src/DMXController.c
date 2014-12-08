#include <gtk/gtk.h>		// GTK+3.0 graphics library
#include <dmx.h>		// DMX interface library
#include "mongoose.h"		// Mongoose Web server
#include <string.h>		// for string manipulations
#include <stdio.h>		// needed for frozen
#include "frozen.h"		// frozen JSON parser
#include <stdlib.h>		// dealing with memory

// constants and definitions

#define RedChannel  2                         // DMX channel for red control
#define GrnChannel  3                         // DMX channel for green control
#define BluChannel  4                         // DMX channel for blue control
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

			struct json_token *arr, *tok;

			// tokenize JSON string, fill in token array.
			arr = parse_json2(json, strlen(json));

//			if (!!arr) {
//				int arrLen = sizeof(arr) / sizeof(arr[0]);
//				printf("%i", arrLen);
//				for (int arrIndx = 0; arrIndx < 2; arrIndx++) {
//					printf("asdfasdfadsf");
//					printf("ans: %i/n/n", arr[arrIndx].type);
					// search for parameter
					tok = find_json_token(arr, "0");
//					printf("%i", arr->len);
//					for (int i=0; i<1; i++) {
						printf("%s", arr[0]);
//					}
//
//					if (tok) {
//						printf("Value is [%.*s]\n", tok->len, tok->ptr);
//					}
				}

				// free some memory
				free(arr);
//			}

			// print the post data (debugging)
			mg_printf_data(conn, "%s", json);
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

	dmxSetValue ( BluChannel , (ubyte) 255 );

	for (int i=1; i<5; i++) {
		sleep(1);
		dmxSetValue( i , (ubyte) 80);
	}
	
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

