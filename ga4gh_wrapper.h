#ifdef __cplusplus
extern "C"
{
#endif
	
//temporary variant structure
typedef struct {
    char** alternateBases;
    //wrong type
	int calls;
    int created;
    long long end;
    char * id;
	//wrong type
    int info;
    char** names;
    char* referenceBases;
    char* referenceName;
    int start;
    int updated;
    char * variantSetId;
} Variant;	
	
int my_parse(char *data);
int my_set_variants();	
int my_get_variant(int id,Variant *v);
int my_variants_size();
	
#ifdef __cplusplus	
}
#endif