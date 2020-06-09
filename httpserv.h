#ifndef HTTPSERV_H
#define HTTPSERV_H

struct CallbackContext
{
		void *crfm;
		char *(*fptr)(void *crfm,const char *ptr);
};

int start_server(int port_number,struct CallbackContext *ct);

#endif
