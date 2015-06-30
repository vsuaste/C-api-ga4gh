/*
These structures file it supposed to be created 
automatically from schemas on the repository.
*/

typedef struct {
	char** alternateBases;
	//wrong type
	int calls;
	int created;
	long end;
	char * id;
	//wrong type
	int info;
	char** names;
	char* referenceBases;
	char* referenceName;
	long start;
	int updated;
	char * variantSetId;
} Variant;

typedef struct {
    char* name;
	char* referenceName;
	long int start;
	long int end;
	int pageSize;
	char* pageToken;
	char** callSetIds;
	char** variantSetIds;
}search_variant_request;
