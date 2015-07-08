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
	Value &info = variants[id]["info"];
	int size = info.MemberEnd() - info.MemberBegin() ;
	return size;
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
	Value &info = variants[id]["info"];
	Value info_v;
	unsigned int i,j;
	int size_info = get_info_size(id);
	std::string info_val;
	v->info_key = (char**)malloc(size_info*sizeof(char*));
	v->info_value = (char**)malloc(size_info*sizeof(char*));
	/* 
	for(rapidjson::Value::ConstMemberIterator it=membersArray.MemberBegin(); it != membersArray.MembersEnd(); it++) {
   std::cout << it->value["template"].GetString();
   */
	if(v->info_key!=NULL && v->info_value!=NULL)
	{
		i = 0;
		for(Value::ConstMemberIterator it=info.MemberBegin(); it != info.MemberEnd(); it++)
		{
			 info_v = info[it->name];
			 v->info_key[i] = (char*)(it->name.GetString());
			if(info_v.Size()==1)
			{
				v->info_value[i] = (char*)info_v[0].GetString();
			}else{
				info_val="";
				for(j=0; j<info_v.Size(); j++)
				{
					info_val+= info_v[j].GetString();
					info_val+= ",";
				}
				info_val[info_val.length()-1]='\0';
				v->info_value[i] = (char*) malloc(info_val.length());
				strcpy(v->info_value[i],info_val.c_str());
			}
			i++;
		}
		
	}else{
	
		return 1; //error
	}
	
	//printf("info size of variant %d is %d \n",id,size);
	
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
	

	
}	

