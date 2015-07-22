#ifndef GA4GH_CLIENT_H
#define GA4GH_CLIENT_H

#include <curl/curl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE (2000 * 1024) //2 MB

typedef struct{
	CURL* curl;
	struct curl_slist* headers;
	char* base_url;
	char* response;
	char* post_fields;
	
	size_t response_length;
	size_t max_response_length;
	
}client_t;

client_t* user;

int client_global_init(void);
int client_global_free(void);

int client_init(client_t* self,char* base_url);
int client_free(client_t* self);
int client_search_request(client_t* user,const char* protocol);

void error(const char *format, ...);

int start_user(char* server_url);
int end_user();
/*
to make different call with different pageToken
and/or different post_fields
int restart_user();
*/

#endif