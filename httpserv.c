#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "dyad.h"
#include "httpserv.h"

/* A very simple HTTP server which responds to a number of paths with different
 * information. See httpserv2.c for an example of an HTTP server which responds
 * with files. */

static int count = 0;

static void urldecode2(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

static void onLine(dyad_Event *e) {
  char path[128];
  if (sscanf(e->data, "GET %127s", path) == 1) {
    /* Print request */
    printf("%s %s\n", dyad_getAddress(e->stream), path);
    /* Send header */
    dyad_writef(e->stream, "HTTP/1.1 200 OK\r\n\r\n");
    /* Handle request */
    if (!strcmp(path, "/")) {
      dyad_writef(e->stream, "<html><body><pre>"
                             "<a href='/date'>date</a><br>"
                             "<a href='/count'>count</a><br>"
                             "<a href='/ip'>ip</a>"
                             "</pre></html></body>" );

    } else {

	char *ptr = strchr(path,'/');
	if (ptr && ptr[1] != 0)
	{    
		if (!strcmp(path,"/favicon.ico"))
			printf("Skipping favicon request\n");
		else 
		{
			struct CallbackContext *cb = (struct CallbackContext *)e->udata;
			char *st1 = &ptr[1];
			char converted[(st1 ? strlen(st1)*2 : 0) + 1];
			urldecode2(converted,st1);
			char *ret_val = cb->fptr(cb->crfm,converted);
			dyad_writef(e->stream, "%s",ret_val);
			free(ret_val);
		}
	}    
	else 
		dyad_writef(e->stream, "bad request '%s'", path);
    }
    /* Close stream when all data has been sent */
    dyad_end(e->stream);
  }
}

static void onAccept(dyad_Event *e) {
  dyad_addListener(e->remote, DYAD_EVENT_LINE, onLine, e->udata);
}

static void onListen(dyad_Event *e) {
  printf("server listening: http://localhost:%d\n", dyad_getPort(e->stream));
}

static void onError(dyad_Event *e) {
  printf("server error: %s\n", e->msg);
}


int start_server(int port,struct CallbackContext *ct) {
  dyad_Stream *s;
  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_ERROR,  onError,  ct);
  dyad_addListener(s, DYAD_EVENT_ACCEPT, onAccept, ct);
  dyad_addListener(s, DYAD_EVENT_LISTEN, onListen, ct);
  dyad_listen(s, port);

  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }

  return 0;
}
