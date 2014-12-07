#include <gtk/gtk.h>		// GTK+3.0 graphics library
#include <dmx.h>		// DMX interface library
#include "mongoose.h"		// Mongoose Web server
#include <string.h>		// for string manipulations
#include "jsmn.h"		// JSON parser

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
			
			// declare some vars
			int r;
			char *js = NULL;
			size_t jslen = 0;
			char postBuf[conn->content_len];
			jsmn_parser jpar;
			jsmntok_t tok[10000];
			size_t tokcount = 100;

			// prepare parser
			jsmn_init(&jpar);

			// actually parse
			r = jsmn_parse(&jpar, conn->content, conn->content_len, tok, sizeof(tok)/sizeof(tok[0]));

			// assume top-level is an object
			if (r < 1 || t[0].type != JSMN_OBJECT) {
				printf("Object expected\n");
				return 1;
			}

			// loop over keys in root
			for (int i = 1; i < r; i++) {
				if (jsoneq(conn->content, &tok[i], "1") == 0) {
					// fetch string value with strndup
					printf("thing: %.*s\n", t[i+1].end-t[i+1].start, conn->content + t[i+1].start);
					i++;
				} else {
					


			// allocate some tokens to begin with
			tok - malloc(sizeof(*tok) * tokcount);
			if (tok == null) {
				// something bad happened
				return MG_FALSE;
			}

			for (;;) {
				// read a chunk.
				read = 

		//	mg_parse_multipart(postBuf, sizeof postBuf, nullBuf, 
			mg_printf_data(conn, "%s", conn->content);



			jsmn_parser parser;

			// js - pointer to JSON string
			// tokens - an array of tokens available
			// 10 - number of tokens available
			jsmn_init(&parser);


		//	struct jsmn_parser jsonParser;
		//	jsmn_init_parser(&jsonParser);
		//	jsmntok_t jsonTokens[1000];

			jsmn_parse(&jsonParser, conn->content, tokens, 256); 

			mg_printf_data(conn, "%s", "POSTed\n");
		}

		// output json of alllllll the current values. 
		mg_printf_data(conn, "%s", "{\n");
		mg_printf_data(conn, "%s", "\t\"1\" : {\n");

		for (int i=1; i<=numChannels; i++) {
			mg_printf_data(conn, "%s%i%s%i%s", "\t\t\"", i, "\" : ", (int) values[i], ",\n");
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

	for (int i=1; i<20; i++) {
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

