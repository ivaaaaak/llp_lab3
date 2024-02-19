#include "../include/executor.h"
#include "../include/xml.h"
#include "../include/network.h"

#define DEFAULT_FILENAME "db"
#define DEFAULT_PORT 8264

int parse_and_send_result(char *buffer, int client_socket, struct database* db) {
    
    struct query q = {0};
    parse_query_from_buffer(buffer, &q);

    struct query_result result = execute_query(db, q);
    xmlDocPtr doc = build_response(result);
    
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpMemory(doc, &xmlbuff, &buffersize);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    printf("Sending: %s", xmlbuff);
    send_data(client_socket, (char *) xmlbuff);
    xmlFree(xmlbuff);

    return 0;
}

void handle_client(int *client_socket_ptr, struct database* db) {
    int client_socket = *client_socket_ptr;
    free(client_socket_ptr);

    bool receiving_data = true;

    while (receiving_data) {
        char buffer[4096];
        int received_bytes = receive_data(client_socket, buffer, sizeof(buffer));
        if (received_bytes <= 0) {
            receiving_data = false;
            continue;
        }
        buffer[received_bytes] = '\0';

        printf("Received: %s\n", buffer);

        if (!validate_request(buffer)) {
            printf("Invalid request\n");
            send_data(client_socket, "<result><message>Invalid request</message></result>");

        } else {
            parse_and_send_result(buffer, client_socket, db);
        }
    }

    close_connection(client_socket);
}


int main(int argc, char** argv) {

    char *filename = DEFAULT_FILENAME;
    int port = DEFAULT_PORT;

    if (argc > 2) {
        filename = argv[2];
    }

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    struct database db = {0};
    create_db("db", &db);

    int server_socket = init_server_socket(port);
    if (server_socket < 0) {
        return -1;
    }

    int client_socket = listen_connections(server_socket, 1);
    if (client_socket < 0) {
        return -1;
    }

    bool accepting_connections = true;

    while (accepting_connections) {
        int client_socket = accept_connection(server_socket);

        if (client_socket < 0) {
            printf("Error accepting connection");
            return -1;
        }

        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        handle_client(client_socket_ptr, &db);
    }

    close_socket(server_socket);
    return 0;
}


void a() {
    struct database db = {0};
    create_db("db", &db);

    struct attribute attr = {
        .type = STRING,
        .value.as_string = "eva"
    };

    struct property prop = {
        .attr_name = "name",
        .attr = attr
    };

    struct query create_q = {
        .type = CREATE,
        .node_type_name = "Person",
        .as_create.prop = &prop
    };

    execute_query(&db, create_q);
    execute_query(&db, create_q);

    struct query q = {0};
    parse_query_from_buffer("<root><query><MATCH_QUERY><NODE label=\"Person\"></NODE><WHERE><EQ><IDENTIFIER>name</IDENTIFIER><STRING>eva</STRING></EQ></WHERE></MATCH_QUERY></query></root>", &q);
    
    execute_query(&db, q);

}