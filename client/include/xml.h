#ifndef XML_H
#define XML_H

#include "../parser/include/query.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml2/libxml/xmlschemas.h>

#define SCHEMA_FILE_PATH "response.xsd"

char* build_query(struct query q);
bool validate_response(const char *buffer);
void print_response(const char* response);

#endif