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
	char** info_key;
	char** info_value;
	char** names;
	char* referenceBases;
	char* referenceName;
	long start;
	int updated;
	char * variantSetId;
} Variant;

typedef struct {
	char* id;
	char* readGroupId;
	char* fragmentName;
	char properPlacement;
	char duplicateFragment;
	int numberReads;
	int fragmentLength;
	int readNumber;
	char failedVendorQualityChecks;	
	char secondaryAlignment;
	char supplementaryAlignment;
	char* alignedSequence;
	char* alignedQuality;
	char** info;
	//GALinearAlignment
	char* referenceName;
	int mappingQuality;	
	//GAPosition
	unsigned long long position;	
	unsigned long long nextMatePosition;
	char* nextMateReferenceName;
	//Cigar
	char* operation;
	int* lengthOperation;
}read_alignment;

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

typedef struct {
	char **variantSetIds;
	char* name;
	int pageSize;
	char* pageToken;
}search_callSets_request;


typedef struct {
	char** readGroupIds;
	char* referenceName;
	char* referenceId;
	long int start;
	long int end;
	int pageSize;
	char* pageToken;
}search_reads_request;