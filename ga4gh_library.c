#include "ga4gh_library.h"
#include "ga4gh_wrapper.h"

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct write_result *result = (struct write_result*)stream;
	
	if(result->pos + size * nmemb >= BUFFER_SIZE)
	{
		fprintf(stderr,"error: too small buffer\n");
		return 0;	
	}	
	
	memcpy(result->data+result->pos,ptr,size*nmemb);
	result->pos+= size*nmemb;
	
	return  size * nmemb;
}

static char* append_protocol_string(const char* baseURL, const char* protocol_name)
{
	int size = strlen(baseURL) + strlen(protocol_name);
	char *completeURL = malloc(size + 1);
	
	strcpy(completeURL, baseURL);
	strcat(completeURL,protocol_name);
	
	return completeURL;
}

char* post_search_variants(char* baseURL, char*postFields)
{
	curl_global_init( CURL_GLOBAL_ALL );
	CURL * myHandle;
	CURLcode result; 
	struct curl_slist *headers = NULL;
	char *data = NULL;	
	char *completeURL = append_protocol_string(baseURL,"/variants/search");
	data = malloc(BUFFER_SIZE);
	struct write_result writeResult = {
		.data = data,
		.pos = 0
	};
		
	myHandle = curl_easy_init();
	headers = curl_slist_append(headers, "Content-Type: application/json");	
	// Notice the lack of major error checking so far....
	curl_easy_setopt(myHandle, CURLOPT_URL,completeURL);
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, postFields);
	curl_easy_setopt(myHandle,CURLOPT_HTTPHEADER,headers);			
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &writeResult);
		
	result = curl_easy_perform( myHandle );
	data[writeResult.pos]='\0';

	curl_easy_cleanup(myHandle);
    curl_slist_free_all(headers);
    curl_global_cleanup();
	return data;
}

void write_vcf_file(char* data, char* name_vcf)
{
	FILE *output_file;
	
	int i = 0;
	int size = 0;
	
	Variant v;
	
	my_parse(data);
	my_set_variants();
	
	output_file = fopen(name_vcf,"w");
	//write header
	fprintf(output_file,"##fileformat=VCFv4.1\n");
 	fprintf(output_file,"#CHROM \t POS \t ID \t REF \t ALT \t QUAL \t FILTER \t INFO \t FORMAT \n");
	
	size = my_variants_size();
	for(i=0; i<size; i++)
	{
		my_get_variant(i,&v);

		fprintf(output_file,"%s \t",v.referenceName);
		fprintf(output_file,"%d \t",v.start);
		fprintf(output_file,"%s \t",v.id);
		fprintf(output_file,"%s \t",v.referenceBases);
		
		fprintf(output_file,"\n");
	}

	fclose(output_file);
}





