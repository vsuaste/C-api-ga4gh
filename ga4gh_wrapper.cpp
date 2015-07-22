#include "ga4gh_wrapper.h"
#include <stdio.h>
#include <rapidjson/document.h>
#include <string>
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
	
int get_info_size(int id)
{
	//Value &info = variants[id]["info"];
	Value::ConstMemberIterator info = variants[id].FindMember("info");
	if(info != variants[id].MemberEnd())
	{
	int size = info->value.MemberEnd() - info->value.MemberBegin() ;
		return size;
	}else{
		//means no info field was found (for ensembl case)
		return -1;
	}
	return 0;
}

int get_alt_size(int id)
{
	return variants[id]["alternateBases"].Size();
}
	
static int set_alt(int id, Variant *v)
{
	int i = 0;
	int size = 0;
	Value& alt = variants[id]["alternateBases"];
	size = alt.Size();
	v->alternateBases = (char**)malloc(size*sizeof(char*));
	for(i=0; i<size; i++)
	{
		v->alternateBases[i] = (char*)alt[i].GetString();
	}
	return 0;
}
	
static int set_info(int id, Variant *v)
{
	//Value &info = variants[id]["info"];
	Value::ConstMemberIterator info = variants[id].FindMember("info");
	if(info != variants[id].MemberEnd())
	{
		Value info_v;
		Value::ConstValueIterator it_info_v;	
		unsigned int i;//j;
		int size_info = info->value.MemberEnd() - info->value.MemberBegin() ;
		std::string info_val;

		v->info_key = (char**)malloc(size_info*sizeof(char*));
		v->info_value = (char**)malloc(size_info*sizeof(char*));
		if(v->info_key!=NULL && v->info_value!=NULL)
		{
			i = 0;
			for(Value::ConstMemberIterator it=info->value.MemberBegin(); it != info->value.MemberEnd(); it++)
			{
				v->info_key[i] = (char*)(it->name.GetString());
				info_val="";
				it_info_v = it->value.Begin();	 
				while(it_info_v != it->value.End())
				{
					info_val+= it_info_v->GetString();
					info_val+= ",";
					it_info_v++;
				}
				info_val[info_val.length()-1]='\0';
				v->info_value[i] = (char*) malloc(info_val.length());
				strcpy(v->info_value[i],info_val.c_str());
				i++;
			}

		}else{

			return 1; //memory error
		}
	}else
	{
		//printf("Response query doesn't have info field \n");
	}
	
	return 0;
}
	
int my_get_variant(int id, Variant *v)
{
		v->referenceName = (char*)variants[id]["referenceName"].GetString();
		v->id = (char*)variants[id]["id"].GetString();
		v->referenceBases = (char*)variants[id]["referenceBases"].GetString();
		v->start = variants[id]["start"].GetInt();
		set_alt(id,v);
		set_info(id,v);	
	return 0;
}
	
char* get_pageToken(void)
{
	Value& pageToken_v = document["nextPageToken"];
	char* pageToken;
	
	if(pageToken_v.IsNull())
	{
		//printf("es null...\n");	
		pageToken=(char*)"NULL";
	}else
	{
		pageToken = (char*)pageToken_v.GetString();
	}
	
	return pageToken;
}
	
}	

