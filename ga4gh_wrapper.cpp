#include "ga4gh_wrapper.h"
#include <stdio.h>
#include <rapidjson/document.h>
using namespace rapidjson;
Document document;	
Value variants;
extern "C"
{

int my_parse(char *data)
{
	//int i;
	document.Parse(data);
	return 0; //can set errors...
}
	
int my_set_variants()
{
	variants = document["variants"];
	return 0;
}	
	
int my_variants_size()
{
	return variants.Size();
}
	
int my_get_variant(int id, Variant *v)
{
		v->referenceName = (char*)variants[id]["referenceName"].GetString();
		v->id = (char*)variants[id]["id"].GetString();
		v->referenceBases = (char*)variants[id]["referenceBases"].GetString();
		v->start = variants[id]["start"].GetInt();	
	return 0;
}
	

	
}	

