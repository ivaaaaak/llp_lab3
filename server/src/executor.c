#include "../include/executor.h"

struct query_result execute_query(struct database* db, struct query q) {
    switch (q.type) {

        case MATCH:
            return match_node(db, q);
        case CREATE:
            return create_node(db, q);
        case DELETE:
            return delete_node(db, q);
        case SET:
            return update_node(db, q);
    }
}