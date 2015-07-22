#include "structures_schemas.h"

#ifdef __cplusplus
extern "C"
{
#endif

int my_parse(char *data);
int my_set_variants();	
int my_get_variant(int id,Variant *v);
int my_variants_size();
int get_info_size(int id);
int get_alt_size(int id);	
char* get_pageToken(void);	
#ifdef __cplusplus	
}
#endif
