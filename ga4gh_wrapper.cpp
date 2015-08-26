#include "ga4gh_wrapper.h"
#include <stdio.h>
#include <rapidjson/document.h>
#include <string>
using namespace rapidjson;
Document document;	
Value variants;
Value callSets;
Value alignments;

extern "C"
{
const char* operations_name[]={
	"ALIGNMENT_MATCH",
	"INSERT",
	"DELETE",
	"SKIP",
	"CLIP_SOFT",
	"CLIP_HARD",
	"PAD",
	"SEQUENCE_MATCH",
	"SEQUENCE_MISMATCH"
};
	
const char operations_char[]={
	'M',
	'I',
	'D',
	'N',
	'S',
	'H',
	'P',
	'=',
	'X'		
};
int my_parse(char *data)
{
	//int i;
	document.Parse(data);
	return 0; //can set errors...
}
	
int my_set_variants(void)
{
	variants = document["variants"];
	return 0;
}	
	
int my_variants_size(void)
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

int my_set_callSets(void)
{
	callSets = document["callSets"];
	return 0;
}	

int callSets_size(void)
{
	return callSets.Size();
}

int get_names_callSets(int size_callSets,char** callSets_name)
{
	int i=0;
	for(i=0; i<size_callSets; i++)
	{
		callSets_name[i] = (char*)callSets[i]["name"].GetString();
	}
	return 0;
}

int my_set_alignments(void)
{
	alignments = document["alignments"];
	return 0;
}

int my_alignments_size(void)
{
	return alignments.Size();
}

int get_alignment(int id, read_alignment* r, int cigar_size)
{	

	r->fragmentName = (char*)alignments[id]["fragmentName"].GetString();
	r->referenceName = (char*)alignments[id]["alignment"]["position"]["referenceName"].GetString();
	r->position = (unsigned long long)alignments[id]["alignment"]["position"]["position"].GetUint64();
	r->mappingQuality = alignments[id]["alignment"]["mappingQuality"].GetInt();
	r->nextMateReferenceName = (char*)alignments[id]["nextMatePosition"]["referenceName"].GetString();
	r->nextMatePosition = (unsigned long long) alignments[id]["nextMatePosition"]["position"].GetUint64();
	r->fragmentLength = alignments[id]["fragmentLength"].GetInt();
	r->alignedSequence = (char*)alignments[id]["alignedSequence"].GetString();
	r->alignedQuality = get_alignedQuality(id);
	get_cigar(id,r->operation,r->lengthOperation,cigar_size);
	return 0;
}

char* get_alignedQuality(int id)
{
	int size_seq = alignments[id]["alignedQuality"].Size();
	int i=0;
	char *string_quality =(char*)malloc((size_seq+1));
	
	for(i=0; i<size_seq; i++)
	{
		string_quality[i] = (char)(alignments[id]["alignedQuality"][i].GetInt() + 33);
	}
	string_quality[size_seq] = '\0';
	
	return string_quality;
}

int get_size_cigar(int id)
{
	return alignments[id]["alignment"]["cigar"].Size();
}

int get_cigar(int id,char* operation, int* length_operation,int size)
{
	int j,i = 0;
	char* operation_name;
	//operation = (char*)malloc(size);
	//length_operation = (int*)malloc(size*sizeof(int));
	
	for(i=0; i<size; i++)
	{
		length_operation[i] = alignments[id]["alignment"]["cigar"][i]["operationLength"].GetInt();
		operation_name =(char*) alignments[id]["alignment"]["cigar"][i]["operation"].GetString();
		for(j=0; j<9; j++)
		{
			if(strcmp(operation_name,operations_name[j])==0)
			{
				operation[i] = operations_char[j];
				break;
			}
		}
	}
	
	return 0;
}

int set_flag(int flag,int id)
{
	if(!alignments[id]["numberReads"].IsNull())
	{
		flag = flag | 1;
	}
	
	if(alignments[id]["properPlacement"].GetBool())
	{
		flag = flag | (1<<1);
	}
	
	if(alignments[id]["alignment"].IsNull())
	{
		flag = flag | (1<<2);
	}else{
		if(alignments[id]["alignment"]["position"]["reverseStrand"].GetBool())
		{
			flag = flag | (1<<4);
		}
	}
	
	if(alignments[id]["nextMatePosition"].IsNull())
	{
		flag = flag | (1<<3);
	}else{
		if(alignments[id]["nextMatePosition"]["reverseStrand"].GetBool())
		{
			flag = flag | (1<<5);
		}
	}

	if(!alignments[id]["readNumber"].IsNull())
	{
		flag = flag | ((1<<6)|(1<<7));
	}	
	return flag;
}