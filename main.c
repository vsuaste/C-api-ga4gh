#include "ga4gh_library.h"

#include "getopt.h"


int main(int argc, char *argv[])
{
	char*data = NULL;	
	int opt = 0;
	int long_index = 0;
	char*server = NULL;
	char*URL_STRING = NULL;
	char*URL_DATA = NULL;
	char*output_file = NULL;
	
	static struct option long_options[] = {
        {"server",    required_argument, 0,  's' },
		//more option to come for command line utility...	
        {0,           0,                 0,  0   }
    };
	
	while((opt = getopt_long(argc,argv,"s:",long_options,&long_index))!= -1)
	{
		switch(opt)
		{
			case 's' : server = optarg;
				break;
			//more option to come for command line utility...
		}
	}
	
	if(!strcmp(server,"ga4gh"))
	{
		URL_STRING = "http://localhost/ga4gh/v0.5.1";
		URL_DATA = "{\"variantSetIds\":[\"1kg-phase1\"], \"callSetIds\":[], \"variantName\":null, \"referenceName\":\"2\", \"start\":33100, \"end\":34000}";
		output_file = "test_ga4gh.vcf";
	}else if(!strcmp(server,"ebi"))
	{
		URL_STRING ="http://wwwdev.ebi.ac.uk/eva/webservices/rest/v1/ga4gh";
		URL_DATA ="{\"referenceName\":22, \"start\":25662282, \"end\":25662302}";
		output_file = "test_ebi.vcf";
	}else
	{
		printf("%s server is not supported. Try ebi or ga4gh options.\n",server);
		return 0;
	}
	
	data = post_search_variants(URL_STRING,URL_DATA);
	write_vcf_file(data,output_file);
	
	free(data);	
	return 0;
}