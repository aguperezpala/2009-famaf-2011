/* * * * * * * * * * * * * * * * * * * * * * * *
 * <  TAD String  >--<  Versión de Mariano  >  *
 * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _STRING__
#define _STRING__

typedef struct {
  char* data;
  int pos;
  int len_data;
  int initialized;
} string;

string* string_create (char* cstr);

void string_destroy (string *str);

char* string_get_front (string *str, int pos ,int len);

char* string_get_back (string *str, int pos ,int len);

char* string_slice_right (string *str, int count, int len);

char* string_slice_left (string *str, int count, int len);




#endif