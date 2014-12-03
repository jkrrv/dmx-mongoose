#include <string.h>
#include "mongoose.h"
#include "dmx.h"

static int event_handler(struct mg_connection *conn, enum mg_event ev) {
  if (ev == MG_AUTH) {
    return MG_TRUE;   // Authorize all requests
  } else if (ev == MG_REQUEST && !strcmp(conn->uri, "/hello")) {
    mg_printf_data(conn, "%s", "Hello world");
    return MG_TRUE;   // Mark as processed
  } else {
    return MG_FALSE;  // Rest of the events are not processed
  }
}

int main(void) {
  struct mg_server *server = mg_create_server(NULL, event_handler);
  mg_set_option(server, "document_root", ".");
  mg_set_option(server, "listening_port", "8080");

  int error;
  error = initDMX();
  if ( error < 0 ) return ( error );

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }
  mg_destroy_server(&server);

  return 0;
}
