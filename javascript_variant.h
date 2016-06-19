
#ifndef _JAVASCRIPT_VARIANT_H__
#define _JAVASCRIPT_VARIANT_H__

enum support_javascript_variant_type {
    NONE=0,    //  NONE_VAR
    NUMBER,    //  VT_INT
    STRING,    //  VT_BSTR
    INT_ARRAY,     //  VT_ARRAY
    OBJECT_ARRAY,     //  VT_ARRAY
    OBJECT,    //  VT_STORED_OBJECT
};

#ifndef _WINDOWS_
#include <windows.h>
#endif

typedef tagVARIANT javascript_variant_struct;
typedef struct {
    unsigned long string_length;
    char* string_data;
} javascript_variant_string;

#ifndef _STRING_
#include <string>

using std::string;
#endif

bool is_exist_variant(string& variant_name);
support_javascript_variant_type get_variant_type(string& variant_name);
bool get_variant(string variant_name,void* output_variant_data,support_javascript_variant_type* output_variant_type);
void set_variant(string variant_name,void* variant_data,support_javascript_variant_type variant_type);
void copy_variant(string destination_variant_name,string source_variant_name);
bool get_variant_array(string variant_name,unsigned long array_index,void* output_variant_data,support_javascript_variant_type* output_variant_type);  //  WARNING !! uncoding it ..
bool set_variant_array(string variant_name,unsigned long array_index,void* input_variant_data,support_javascript_variant_type input_variant_type);  //  WARNING !! uncoding it ..

#endif
