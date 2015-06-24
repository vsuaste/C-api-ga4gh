#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include <curl/curl.h>

#define BUFFER_SIZE (350 * 1024) //350 KB

struct write_result
{
	char *data;
	int pos;
};


char* post_search_variants(char* baseURL, char*postFields);

void write_vcf_file(char* data, char* name_vcf);