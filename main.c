#include "ga4gh_client.h"
#include "getopt.h"

int main_searchvariants(int argc, char* argv[],char* server_url);

typedef struct
{
	int (*func)(int,char*[],char *);
	const char *alias,*help;
}cmd_t;

static cmd_t cmds[]=
{
	{	.func = NULL,
	 	.alias = "Indexing",
	 	.help = NULL
	},
	{	.func = main_searchvariants,
		.alias = "search_variants",
	 	.help = "Gets a list of variants matching the search criteria."
	},
	/*
	more commands, one for each protocol
	*/
	{	.func = NULL,
		.alias = NULL,
	 	.help = NULL
	}
};


static void usage(FILE *fp)	
{
	int i=0;
	
	fprintf(fp,"\n");
	fprintf(fp,"Program: ./ga4gh-tools Tools for using GA4GH API protocols.\n");
	/*
		Version, and more description goes here...
	*/
	fprintf(fp,"Usage:   ./ga4gh-tools [--server <string>][--help] <command> <arguments> \n");
	fprintf(fp,"If not specified, default server will be ga4gh one.\n");
	fprintf(fp,"\n");
	fprintf(fp,"Commands: \n");
	while(cmds[i].alias)
	{
		if(cmds[i].func)
		{
		fprintf(fp," %s    %s\n",cmds[i].alias, cmds[i].help);
		}
		i++;
	}
	fprintf(fp,"\n");
	fprintf(fp,"For more info about each command use --help option before the command.\n");
}


int main(int argc, char *argv[])
{
	if(argc < 2){usage(stderr); return 1;}
	char* server_url="http://localhost/ga4gh/v0.5.1";
	
	if(strcmp(argv[1],"--server")==0)
	{
		if(argc==3)
		{
			usage(stdout);
			return 0;
		}
		else
		{
			if(strcmp(argv[2],"ga4gh")==0)
			{
				server_url = "http://localhost/ga4gh/v0.5.1";
			}else if(strcmp(argv[2],"ebi")==0)
			{
				server_url ="http://wwwdev.ebi.ac.uk/eva/webservices/rest/v1/ga4gh";
			}else{
				error("%s server is not supported. Try ebi or ga4gh options.\n",argv[2]);
			}

			argv+=2;
			argc-=2;
		}
	}
	if(strcmp(argv[1],"--help")==0)
	{
		if(argc==2)
		{
			usage(stdout);
			return 0;
		}
		else
		{
			argv++;
			argc=2;
		}
	}
	
	int i=0;
	while(cmds[i].alias)
	{
		if(cmds[i].func && strcmp(argv[1],cmds[i].alias)==0)
		{
			return cmds[i].func(argc-1,argv+1,server_url);
		}
		i++;
	}
	fprintf(stderr, "Unrecognized command '%s'\n",argv[1]);
	return 1;
}
