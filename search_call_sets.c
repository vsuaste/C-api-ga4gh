#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "ga4gh_wrapper.h"
#include "ga4gh_client.h"

static void usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Optional arguments:\n");
	fprintf(stderr, "--variantSetIds -v  <id1,id2,...>       If specified, will restrict the query to call sets.\n");
 	fprintf(stderr, "                                        within the given variants sets.\n");
	fprintf(stderr, "--name	-n           <string>            Only return call sets for which a substring of the\n");
	fprintf(stderr, "                                        name matches this string.\n");
	fprintf(stderr, "\n");
	
	exit(1);
}


static char* create_request_string(search_callSets_request* request,int size_variants)
{
	char* request_string;
	char* callSet_name;
	//compute size of request_string
	int size = 35; //default characters
	int i=0;
	size+= strlen(request->name);
	size+= 5*size_variants;
	
	for(i=0; i<size_variants;i++)
	{
		size+=strlen(request->variantSetIds[i]);	
	}
	
	request_string = malloc(size);
	
	if(strcmp(request->name,"null")==0)
	{
		callSet_name = "null";
	}else
	{
		callSet_name = malloc(strlen(request->name)+4);
		strcpy(callSet_name,"\"");
		strcat(callSet_name,request->name);
		strcat(callSet_name,"\"");
	}
		
	strcpy(request_string,"{\"variantSetIds\":[");
	
	if(size_variants > 0)
	{
		
		strcat(request_string,"\"");
		strcat(request_string,request->variantSetIds[0]);
		strcat(request_string,"\"");
		for(i=1; i<size_variants; i++)
		{
			strcat(request_string,",\"");
			strcat(request_string,request->variantSetIds[i]);
			strcat(request_string,"\"");
		}	
	}
	strcat(request_string,"],");
	
	strcat(request_string,"\"name\":");
	strcat(request_string,callSet_name);
	strcat(request_string,"}");
	return request_string;
}

static int count_ids(char *_ids)
{
	int i=0;
	//assuming there is not ',' at the very end of _ids and already validated not NULL.
	int count = 1;
	while(_ids[i]!='\0')
	{
		if(_ids[i]==',')
		{
			count++;
		}
		i++;
	}
	return count;
}

static void set_ids(char *setIds, char**request_, int size)
{
	int i=0;
	char* tok;
	
	tok=strtok(setIds,",");
	while(tok!=NULL)
	{
		request_[i++] = tok;
		tok = strtok(NULL,",");
	}
}

static int print_callSets()
{
	int size_callSets = 0;
	int i = 0;
	char** callSets_name;
	
	my_parse(user->response);
	my_set_callSets();
	size_callSets = callSets_size();
	
	callSets_name = malloc(size_callSets*sizeof(char*));
	
	get_names_callSets(size_callSets,callSets_name);

	for(i=0; i<size_callSets; i++)
	{
		printf("%s\n",callSets_name[i]);
	}
	
	return 0;
}

int main_searchcallSets(int argc, char* argv[],char *server_url)
{
	int cmd;
	search_callSets_request *request=(search_callSets_request*)malloc(1*sizeof(search_callSets_request));	
	int size_variants = 0;
	request->name = "null";
	char debug = 0;
	
	static struct option long_options[]={
		{"variantSetIds",required_argument,0,'v'},
		{"name",required_argument,0,'n'},
		{"debug",no_argument,0,'d'},
		{0,0,0,0}
	};
	//if(argc < 2)
	while((cmd=getopt_long(argc,argv,"v:n:d",long_options,NULL))!=-1)
	{
		switch(cmd)
		{
			case 'v':
					size_variants = count_ids(optarg);
					request->variantSetIds = (char**)malloc(size_variants*sizeof(char*));
					set_ids(optarg,request->variantSetIds,size_variants);
					break;
			case 'n': request->name = optarg; break;
			case 'd': debug = 1; break;
			case '?': usage();
			default: error("Unknown argument %s\n",optarg);
		}
	}
	
	/*
	if(size_variants==0)
	{
		usage();
	}
	*/
	start_user(server_url);
	
	user->post_fields = create_request_string(request,size_variants);
	//printf("%s\n",user->post_fields);
	client_search_request(user,"callsets");
	print_callSets();
	if(debug)
	{
	printf("%s\n",user->response);
	}

	end_user();
	return 0;
}
