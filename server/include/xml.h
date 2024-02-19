#ifndef XML_H
#define XML_H

#include "../database/include/query.h"
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlschemas.h>

#define SCHEMA_FILE_PATH "../query.xsd"

int parse_query_from_buffer(char* buffer, struct query* query);
xmlDocPtr build_response(struct query_result result);
bool validate_request(const char *buffer);

#endif