#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "ga4gh_wrapper.h"
#include "ga4gh_client.h"

static void usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Arguments:\n");
	fprintf(stderr, "--readGroupIds  -r  <id1,id2,...>       If specified, restrict this query to reads within \n");
	fprintf(stderr, "                                        the given readgroups.\n");
	fprintf(stderr, "--referenceName -n  <string>            The reference to query.Provide at most one of referenceId\n");
	fprintf(stderr, "                                        and referenceName.Leaving both blank returns results from all\n");
	fprintf(stderr, "                                        references, including unmapped reads-this could be very large.\n");
	fprintf(stderr, "--referenceId   -i  <string>            The reference to query.Provide at most one of referenceId\n");
	fprintf(stderr, "                                        and referenceName.Leaving both blank returns results from all\n");
	fprintf(stderr, "                                        references, including unmapped reads-this could be very large.\n");	
	fprintf(stderr, "--start -s          <positive int>      The start position (0-based) of this query.If a reference is\n");
	fprintf(stderr, "                                        specified, this defaults to 0.Genomic positions are non-negative\n");
	fprintf(stderr, "                                        integers less than reference length.Requests spanning the join of\n");
	fprintf(stderr, "                                        circular genomes are represented as two requests one on each side\n");
	fprintf(stderr, "                                        of the join (position 0).\n");
	fprintf(stderr, "--end -e            <positive int>      The end position (0-based, exclusive) of this query.If a reference\n");
	fprintf(stderr, "                                        is specified, this defaults to the reference's length.\n");
	fprintf(stderr, "--output -o         <string>            Name for output sam file.\n");
	fprintf(stderr, "\n");
	exit(1);
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
	//request_ = (char**)malloc(size*sizeof(char*));
	
	tok=strtok(setIds,",");
	while(tok!=NULL)
	{
		request_[i] = tok;
		tok = strtok(NULL,",");
	}
}

static char* create_request_string(search_reads_request* request,int id)
{
	char* request_string;
	char request_start[20];
	char request_end[20];
	char* request_pageToken;
	char* request_referenceName;
	char* request_referenceId;
	//compute size of request_string
	int size = 110; //default characters	
	
	snprintf(request_start, sizeof(request_start),"%ld",request->start);
	snprintf(request_end, sizeof(request_end),"%ld",request->end);

	if(strcmp(request->referenceId,"null")==0)
	{
		request_referenceId = "null";
	}else
	{
		request_referenceId = malloc(strlen(request->referenceId)+4);
		strcpy(request_referenceId,"\"");
		strcat(request_referenceId,request->referenceId);
		strcat(request_referenceId,"\"");
	}	
	
	if(strcmp(request->referenceName,"null")==0)
	{
		request_referenceName = "null";
	}else
	{
		request_referenceName = malloc(strlen(request->referenceName)+4);
		strcpy(request_referenceName,"\"");
		strcat(request_referenceName,request->referenceName);
		strcat(request_referenceName,"\"");
	}	
	
	
	if(strcmp(request->pageToken,"null")==0)
	{
		request_pageToken = "null";
	}else
	{
		request_pageToken = malloc(strlen(request->pageToken)+4);
		strcpy(request_pageToken,"\"");
		strcat(request_pageToken,request->pageToken);
		strcat(request_pageToken,"\"");
	}
	
	size+=strlen(request_start);
	size+=strlen(request_end);
	size+=strlen(request->referenceName);
	size+=strlen(request->referenceId);
	size+=strlen(request->readGroupIds[id]);
	size+=strlen(request->pageToken);
	
	request_string = malloc(size);

	if(request_string == NULL)
	{
		printf("memory error \n");
	}
	
	if(request->end == 0)
	{
		strcpy(request_end,"null");
	}

	strcpy(request_string,"{\"readGroupIds\":[\"");
	strcat(request_string,request->readGroupIds[id]);
	strcat(request_string,"\"],\"referenceName\":");
	strcat(request_string,request_referenceName);
	strcat(request_string,",\"referenceId\":");
	strcat(request_string,request_referenceId);
	strcat(request_string,",\"start\":");
	strcat(request_string,request_start);
	strcat(request_string,",\"end\":");
	strcat(request_string,request_end);
	strcat(request_string,",\"pageToken\":");
	strcat(request_string,request_pageToken);
	strcat(request_string,"}");
	
	return request_string;
}

static int create_sam_file(char* name_sam)
{
	FILE* output_file;
	output_file = fopen(name_sam,"w");
	//write header
	fprintf(output_file,"@HD VN:1.5\n");
	
	fclose(output_file);
	return 0;
}			
			
static int write_sam_file(char* response,char* name_sam)
{
	FILE *output_file;
	int size = 0;
	int i=0;
	int j=0;
	read_alignment *read = (read_alignment*)malloc(1*sizeof(read_alignment));
	
	my_parse(response);
	my_set_alignments();
	size = my_alignments_size();
		
	output_file = fopen(name_sam,"a");
	int size_cigar=0;
	int flag;
	for(i=0; i<size; i++)
	{
		flag = 0;
		size_cigar = get_size_cigar(i);
		read->operation = (char*)malloc(size_cigar);
		read->lengthOperation = (int*)malloc(size_cigar*sizeof(int));
		get_alignment(i,read,size_cigar);
		flag = set_flag(flag,i);
		//printf("flag: %d \n",flag);
		fprintf(output_file,"%s\t",read->fragmentName);
		fprintf(output_file,"%d\t",flag);
		fprintf(output_file,"%s\t",read->referenceName);
		fprintf(output_file,"%llu \t",(read->position + 1));
		fprintf(output_file,"%d\t",read->mappingQuality);
		for(j=0; j<size_cigar; j++)
		{
			fprintf(output_file,"%d",read->lengthOperation[j]);
			fprintf(output_file,"%c",read->operation[j]);
		}
		fprintf(output_file,"\t");
		fprintf(output_file,"%s\t",(read->nextMateReferenceName));
		fprintf(output_file,"%llu\t",(read->nextMatePosition + 1));
		fprintf(output_file,"%d\t",read->fragmentLength);
		fprintf(output_file,"%s\t",read->alignedSequence);
		fprintf(output_file,"%s\t",read->alignedQuality);
		fprintf(output_file,"\n");
		
		if(read->alignedQuality!=NULL)
		{
			free(read->alignedQuality);
		}
		
		if(read->operation!=NULL)
		{
			free(read->operation);
		}
		
		if(read->lengthOperation!=NULL)
		{
			free(read->lengthOperation);
		}
	}
	
	return 0;
}

int main_searchreads(int argc, char* argv[],char *server_url)
{
	int cmd;
	int i;
	search_reads_request *request=(search_reads_request*)malloc(1*sizeof(search_reads_request));	
	int size_reads = 0;
	request->referenceId = "null";
	request->referenceName = "null";
	request->pageToken ="null";
	request->start = 0;
	request->end = 0;
	char* sam_file_name;
	
	char debug = 0;
	
	static struct option long_options[]={
		{"readGroupIds",required_argument,0,'r'},
		{"referenceName",required_argument,0,'n'},
		{"referenceId",required_argument,0,'i'},	
		{"start",required_argument,0,'s'},
		{"end",required_argument,0,'e'},
		{"output",required_argument,0,'o'},
		{"debug",no_argument,0,'d'},
		{0,0,0,0}
	};
	//if(argc < 2)
	while((cmd=getopt_long(argc,argv,"r:n:i:s:e:o:d",long_options,NULL))!=-1)
	{
		switch(cmd)
		{
			case 'r':
					if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							error("--readGroupIds string can't be empty.\n");
							usage();
						}
					  else
						{
							size_reads = count_ids(optarg);
						 	request->readGroupIds = (char**)malloc(size_reads*sizeof(char*));
							set_ids(optarg,request->readGroupIds,size_reads);
						}
						break;
			case 'n':
					if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							error("--referenceName string can't be empty.\n");
							usage();
						}
					  else
						{
							request->referenceName = optarg; 
						  	
						}
						break;
			case 'i':
					if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							error("--referenceId string can't be empty.\n");	
							usage();
						}
					  else
						{
							request->referenceId = optarg; 
						  	
						}
						break;			
			case 's': request->start = atol(optarg);
					if(request->start < 0)
					{
					  	error("--start integer must be no negative.");
						usage();
					}
					break;
			case 'e': request->end = atol(optarg); 
					if(request->end < 0)
					{
						error("--end integer must be no negative.");
						usage();
					}
					break;
			case 'o': sam_file_name = optarg;
					break;
			case 'd': debug = 1; break;
			case '?': usage();
			default: error("Unknown argument %s\n",optarg);
		}
	}
	
	if(size_reads==0)
	{
		usage();
	}
	
	start_user(server_url);
	//char* sam_file_name = "test.sam";
	//process each variantSet
	for(i=0; i<size_reads; i++)
	{
		create_sam_file(sam_file_name);	
		
		//while(strcmp(request->pageToken,"NULL")!=0)
		//{
			user->post_fields = create_request_string(request,i);
			//printf("post field string: %s \n",user->post_fields);
			client_search_request(user,"reads");
			//printf("%s\n",user->response);
			write_sam_file(user->response,sam_file_name);
			request->pageToken = get_pageToken();
			//printf("%s \n",request->pageToken);
			if(debug)
			{
				printf("%s\n",user->response);
			}
		//}
	}
	end_user();
	return 0;
}
