#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "ga4gh_wrapper.h"
#include "ga4gh_client.h"

static void usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Required arguments:\n");
	fprintf(stderr, "--variantSetIds -v  <id1,id2,...>       The IDs of the variant sets to search over.\n");
	fprintf(stderr, "--referenceName -r  <string>            Only return variants on this reference.\n");
	fprintf(stderr, "--start -s          <positive int>      The beginning of the window(1-bases,inclusive)\n");
	fprintf(stderr, "                                        for which overlapping variants should be returned.\n");
	fprintf(stderr, "--end -e            <positive int>      The end of the window(1-bases,inclusive) for which\n");
	fprintf(stderr, "                                        overlapping variants should be returned.\n");
	fprintf(stderr, "Optional arguments:\n");
	fprintf(stderr, "--callSetIds -c     <c_id1,c_id2,...>   Only return variant calls which belong to call sets\n");
	fprintf(stderr, "                                        with these IDs.\n");
	fprintf(stderr, "--variantName -n    <string>            Only return variants which have exactly this name\n");
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
//supported for one variantSetId
static char* create_request_string(search_variant_request* request,int id,int size_call)
{
	char* request_string;
	char request_start[20];
	char request_end[20];
	char* variant_name;
	//compute size of request_string
	int size = 107; //default characters
	int i=0;
	size+=strlen(request->variantSetIds[id]);
	size+= 5*size_call;
	for(i=0; i<size_call;i++)
	{
		size+=strlen(request->callSetIds[i]);
	}

	snprintf(request_start, sizeof(request_start),"%ld",request->start);
	snprintf(request_end, sizeof(request_end),"%ld",request->end);
	
	size+=strlen(request_start);
	size+=strlen(request_end);
	
	request_string = malloc(size);
	

	
	if(strcmp(request->name,"null")==0)
	{
		variant_name = "null";
	}else
	{
		variant_name = malloc(strlen(request->name)+4);
		strcpy(variant_name,"\"");
		strcat(variant_name,request->name);
		strcat(variant_name,"\"");
	}
		
	strcpy(request_string,"{\"variantSetIds\":[\"");
	strcat(request_string,request->variantSetIds[id]);
	strcat(request_string,"\"],\"callSetIds\":[");
	for(i=0; i<size_call; i++)
	{
		strcat(request_string,"\"");
		strcat(request_string,request->callSetIds[i]);
		strcat(request_string,"\",");
		//extra ',' at the end, need to fix it....
	}

	strcat(request_string,"],\"variantName\":");
	strcat(request_string,variant_name);
	strcat(request_string,",\"referenceName\":\"");
	strcat(request_string,request->referenceName);
	strcat(request_string,"\",\"start\":");
	strcat(request_string,request_start);
	strcat(request_string,",\"end\":");
	strcat(request_string,request_end);
	strcat(request_string,"}");
	//printf("request string: %s\n",request_string);
	return request_string;
}

static char* get_variantSetId_vcf_name(search_variant_request* request,int id)
{
	char *vcf_file_name;
	vcf_file_name = malloc(strlen(request->variantSetIds[id])+4);
	strcpy(vcf_file_name,request->variantSetIds[id]);
	strcat(vcf_file_name,".vcf");

	return vcf_file_name;
}

static int create_vcf_file(char* name_vcf)
{
	FILE* output_file;
	output_file = fopen(name_vcf,"w");
	//write header
	fprintf(output_file,"##fileformat=VCFv4.1\n");
 	fprintf(output_file,"#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT\n");	
	
	fclose(output_file);
	return 0;
}

static int write_vcf_file(char* response,char* name_vcf)
{
	FILE *output_file;
	
	int i = 0;
	int j = 0;
	int size = 0;
	int size_info,size_alt;
	Variant *v = (Variant*)malloc(1*sizeof(Variant));

	my_parse(response);
	my_set_variants();
	size = my_variants_size();
	
	
	
	output_file = fopen(name_vcf,"a");
	for(i=0; i<size; i++)
	{
		size_info =  get_info_size(i);
		size_alt = get_alt_size(i);
		
		my_get_variant(i,v);
		
		fprintf(output_file,"%s\t",v->referenceName);
		fprintf(output_file,"%ld\t",v->start);
		fprintf(output_file,"%s\t",v->id);
		fprintf(output_file,"%s\t",v->referenceBases);
		fprintf(output_file,"%s",v->alternateBases[0]);
		for(j=1; j<size_alt; j++)
		{
			fprintf(output_file,",%s",v->alternateBases[j]);
		}
		fprintf(output_file,"\t");
		fprintf(output_file,".\t");
		fprintf(output_file,"PASS\t");
		//consider case size_info = 0 ?
		if(size_info!=-1)
		{
			fprintf(output_file,"%s",v->info_key[0]);
			fprintf(output_file,"=%s",v->info_value[0]);
			for(j = 1; j< size_info; j++)
			{
				fprintf(output_file,";%s",v->info_key[j]);
				fprintf(output_file,"=%s",v->info_value[j]);
			}
		}else{
		
			fprintf(output_file,".");
		}
		fprintf(output_file,"\t");
		fprintf(output_file,"\n");
		
		if(v->info_key!=NULL)
		{
		free(v->info_key);
		}
		
		if(v->info_value)
		{
		free(v->info_value);
		}
		
		if(v->alternateBases!=NULL)
		{
		free(v->alternateBases);
		}
	}

	fclose(output_file);		
	return 0;
}


int main_searchvariants(int argc, char* argv[],char *server_url)
{
	int cmd;
	int i;
	search_variant_request *request=(search_variant_request*)malloc(1*sizeof(search_variant_request));	
	int size_variants = 0;
	int size_calls = 0;
	request->name = "null";
	char debug = 0;
	
	static struct option long_options[]={
		{"variantSetIds",required_argument,0,'v'},
		{"referenceName",required_argument,0,'r'},
		{"start",required_argument,0,'s'},
		{"end",required_argument,0,'e'},
		{"callSetIds",optional_argument,0,'c'},
		{"variantName",optional_argument,0,'n'},
		{"debug",no_argument,0,'d'},
		{0,0,0,0}
	};
	//if(argc < 2)
	while((cmd=getopt_long(argc,argv,"v:r:s:e:cnd",long_options,NULL))!=-1)
	{
		switch(cmd)
		{
			case 'v':
					if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							error("--variantSetIds string can't be empty.\n");	
						}
					  else
						{
							size_variants = count_ids(optarg);
						 	request->variantSetIds = (char**)malloc(size_variants*sizeof(char*));
							set_ids(optarg,request->variantSetIds,size_variants);
						}
						break;
			case 'r':
					if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							error("--refereName string can't be empty.\n");				
						}
					  else
						{
							request->referenceName = optarg; 
						  	
						}
						break;
			case 's': request->start = atol(optarg);
					if(request->start < 0)
					{
					  	error("--start integer must be no negative.");
					}
					break;
			case 'e': request->end = atol(optarg); 
					if(request->end < 0)
					{
						perror("--end integer must be no negative.");
					}
					break;
			case 'c': if(optarg==NULL||(strcmp(optarg,"")==0))
						{
							size_calls = 0;
						}
					  else
					  	{
					  		size_calls = count_ids(optarg);
						  	request->callSetIds = (char**)malloc(size_calls*sizeof(char*));
						  	set_ids(optarg,request->callSetIds,size_calls);
					  	}
					break;
			case 'n': request->name = optarg; break;
			case 'd': debug = 1; break;
			case '?': usage();
			default: error("Unknown argument %s\n",optarg);
		}
	}
	
	if(size_variants==0)
	{
		usage();
	}
	
	start_user(server_url);
	char* vcf_file_name;
	//process each variantSet
	for(i=0; i<size_variants; i++)
	{

		vcf_file_name = get_variantSetId_vcf_name(request,i);
		
		create_vcf_file(vcf_file_name);		
		
		user->post_fields = create_request_string(request,i,size_calls);
		
		client_search_request(user,"variants");
		
		write_vcf_file(user->response,vcf_file_name);
		if(debug)
		{
		printf("%s\n",user->response);
		}

	}
	end_user();
	return 0;
}

