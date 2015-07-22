#include "ga4gh_client.h"

int client_global_init(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	return 0;
}

int client_global_free(void)
{
	curl_global_cleanup();
	return 0;
}

static size_t write_response(void* ptr, size_t size, size_t nmemb, void* stream)
{
	client_t* self = (client_t*)stream;
	if(self->response_length + size > self->max_response_length)
	{
		//Error small buffer
		return 1;
	}
	memcpy(self->response + self->response_length,ptr,size*nmemb);
	self->response_length+=size*nmemb;
	return size*nmemb;
}

int client_init(client_t* self,char* base_url)
{
	self->max_response_length = BUFFER_SIZE;
	self->response_length = 0;
	self->response = malloc(self->max_response_length);
	if(self->response==NULL)
	{
		//error
		return 1;
	}
	
	self->base_url = malloc(strlen(base_url));
	if(self->base_url==NULL)
	{
		//error
		return 1;
	}
	strcpy(self->base_url,base_url);
	
	self->curl = curl_easy_init();
	if(self->curl==NULL)
	{
		//error
		return 1;
	}
	self->headers = curl_slist_append(self->headers,"Content-Type: application/json");
	
	curl_easy_setopt(self->curl, CURLOPT_HTTPHEADER,self->headers);
	curl_easy_setopt(self->curl, CURLOPT_WRITEFUNCTION,write_response);
	curl_easy_setopt(self->curl, CURLOPT_WRITEDATA,self);
	
	return 0;
}

int client_free(client_t* self)
{
	if(self->base_url!=NULL)
	{
		free(self->base_url);
	}
	if(self->response!=NULL)
	{
		free(self->response);
	}
	if(self->post_fields!=NULL)
	{
		free(self->post_fields);
	}
	if(self->headers!=NULL)
	{
		curl_slist_free_all(self->headers);
	}	
	if(self->curl!=NULL)
	{
		curl_easy_cleanup(self->curl);
	}
	return 0;
}	

int client_search_request(client_t* self,const char* protocol)
{
	char * complete_url;
	size_t url_len;
	CURLcode result;
	//restart client
	self->response_length = 0;
	//strlen("/search") = 7
	url_len = strlen(self->base_url) + strlen(protocol) + 8;
	complete_url = malloc(url_len);
	
	if(complete_url == NULL)
	{
		//error
		return 1;
	}
	
	strcpy(complete_url,self->base_url);
	strcat(complete_url,"/");
	strcat(complete_url,protocol);
	strcat(complete_url,"/search");
	
	curl_easy_setopt(self->curl,CURLOPT_URL, complete_url);
	curl_easy_setopt(self->curl,CURLOPT_POSTFIELDS, self->post_fields);
	curl_easy_setopt(self->curl,CURLOPT_POSTFIELDSIZE, strlen(self->post_fields));
	
	result = curl_easy_perform(self->curl);
	if(result != CURLE_OK)
	{
		//error
		printf("No OK curl \n");
		char *error = (char*) curl_easy_strerror(result);
		printf("%s \n",error);
		return 1;
	}
	self->response[self->response_length]='\0';
	if(complete_url!=NULL)
	{
		free(complete_url);
	}
	return 0;
}

int start_user(char* server_url)
{
	user = (client_t*)malloc(sizeof(client_t));
	client_global_init();
	client_init(user,server_url);

	return 0;
}

int end_user()
{

	client_global_free();
	client_free(user);
	if(user!=NULL)
	{
		free(user);
	}
	return 0;
}

void error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(-1);
}

