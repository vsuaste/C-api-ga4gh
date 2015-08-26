#include "structures_schemas.h"

#ifdef __cplusplus
extern "C"
{
#endif

int my_parse(char *data);
int my_set_variants(void);	
int my_get_variant(int id,Variant *v);
int my_variants_size(void);
int get_info_size(int id);
int get_alt_size(int id);	
char* get_pageToken(void);
int my_set_callSets(void);	
int callSets_size(void);	
int get_names_callSets(int size_callSets,char** callSets_name);	
int my_set_alignments(void);
int my_alignments_size(void);
int get_alignment(int id, read_alignment* r,int cigar_size);	
char* get_alignedQuality(int id);	
int get_size_cigar(int id);	
int get_cigar(int id,char* operation, int* length_operation,int size);	
int set_flag(int flag,int id);	
#ifdef __cplusplus	
}
#endif
