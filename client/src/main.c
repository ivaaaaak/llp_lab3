#include "../include/network.h"
#include "../include/xml.h"

extern int yyparse();
extern FILE *yyin;
extern struct query q;
 
size_t print_line(char* line, size_t node_cnt) {
    node_cnt++;

    char* node1 = strtok(line, "#");
    char* node2 = strtok(NULL, "#");

    printf("NODE %zu\n", node_cnt);
    char* attr = strtok(node1, ",");
    while (attr != NULL) {
        printf("\t%s\n", attr);
        attr = strtok(NULL, ",");
    }

    node_cnt++;

    printf("NODE %zu\n", node_cnt);
    attr = strtok(node2, ",");
    while (attr != NULL) {
        printf("\t%s\n", attr);
        attr = strtok(NULL, ",");
    }

    return node_cnt;
}


void process_edge_query() {
    FILE* f = fopen("has_control.txt", "r");

    char line[256];
    char substring[100];
    char substring2[100];
    struct filter* fil = q.as_match.cond.filter;
    struct logic_operation* lop = q.as_match.cond.log_op;

    size_t node_cnt = 0;

    while (fil != NULL) {
        
        strcpy(substring, fil->property.name);
        strcat(substring, ": ");
        strcat(substring, fil->property.value.as_string);

        if (lop != NULL && lop->operation == AND_T) {
            fil = fil->next_filter;
            strcpy(substring2, fil->property.name);
            strcat(substring2, ": ");
            strcat(substring2, fil->property.value.as_string);
        }

        while (fgets(line, 256, f) != NULL) {
            if (strstr(line, substring) != NULL) {
               if (lop != NULL && lop->operation == AND_T) {
                    if (strstr(line, substring2) != NULL) {
                        node_cnt = print_line(line, node_cnt);
                        continue;
                    }
                }
                node_cnt = print_line(line, node_cnt);
            }
        }
        rewind(f);

        if (fil != NULL) {
            fil = fil->next_filter;
        }
        if (lop != NULL) {
            lop = lop->next_logic_operation;
        }
    }
    
    fclose(f);
    memset(&q, 0, sizeof(struct query));

}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Usage: %s <server_host> <server_port>\n", argv[0]);
        return -1;
    }

    int client_socket = init_client_socket();

    if (client_socket < 0) {
        return -1;
    }

    connect_to_server(client_socket, argv[1], atoi(argv[2]));
    bool sending_to_server = true;
    
    while (sending_to_server) {

        char query[1000];
        printf("Enter query: ");
        scanf("%[^\n]%*c", query);

        if (strcmp(query, "exit") == 0) {
            sending_to_server = false;
            printf("Exiting...\n");
            continue;
        }

        yyin = fmemopen(query, strlen(query), "r");
        int status = yyparse();
        fclose(yyin);

        if (status == 0) {

            if (q.type == MATCH_EDGE_T) {
                process_edge_query();
                continue;
            }

            char* xml = build_query(q);
            send_data(client_socket, xml);

            char received_data[RECEIVE_BUFFER_SIZE];
            int received_bytes = receive_data(client_socket, received_data, sizeof(received_data));
            received_data[received_bytes] = '\0';
            
            if (!validate_response(received_data)) {
                printf("Invalid response\n");
                continue;
            } else {
                print_response(received_data);
            }
            memset(&q, 0, sizeof(struct query));
        }
    }

    close_connection(client_socket);
    close_socket(client_socket);
    return 0;
}