#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int x;
    int y;
    float g;
    float h;
    float f;
    struct Node *neighbors[4];
    struct Node *parent;
} Node;

Node *create_node(int x, int y) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        puts("Failed to allocate memory for new node...");
        exit(1);
    }

    new_node->x = x;
    new_node->y = y;
    new_node->g = 0.0;
    new_node->h = 0.0;
    new_node->f = 0.0;
    for (int i = 0; i < 4; i++) {
        new_node->neighbors[i] = NULL;
    }

    return new_node;
}

const int GridWidth = 100;
const int GridHeight = 100;

Node *nodes[GridWidth][GridHeight];

void generate_grid() {
    for (int x = 0; x < GridWidth; x++) {
        for (int y = 0; y < GridHeight; y++) {
            nodes[x][y] = create_node(x, y);
        }
    }

    for (int x = 0; x < GridWidth; x++) {
        for (int y = 0; y < GridHeight; y++) {
            if (x > 0) {  // left neighbor
                nodes[x][y]->neighbors[0] = nodes[x - 1][y];
            }
            if (x < (GridWidth - 1)) {  // right neighbor
                nodes[x][y]->neighbors[1] = nodes[x + 1][y];
            }
            if (y > 0) {  // down neighbor
                nodes[x][y]->neighbors[2] = nodes[x][y - 1];
            }
            if (y < (GridHeight - 1)) {  // up neighbor
                nodes[x][y]->neighbors[3] = nodes[x][y + 1];
            }
        }
    }
}

Node *random_node() {
    int x = rand() % GridWidth;
    int y = rand() % GridHeight;
    return nodes[x][y];
}

int manhattan_distance(Node *start, Node *end) {
    int x_distance = abs(end->x - start->x);
    int y_distance = abs(end->y - start->y);
    return x_distance + y_distance;
}

Node *get_lowest_f_node(Node **open_list, int open_list_size) {
    Node *lowest_f_cost_node;

    if (open_list_size <= 0) {
        puts("Open list size is 0 or less...");
        exit(1);
    }

    lowest_f_cost_node = open_list[0];
    for (int i = 1; i < open_list_size; i++) {
        if (open_list[i]->f < lowest_f_cost_node->f) {
            lowest_f_cost_node = open_list[i];
        } else if (open_list[i]->f == lowest_f_cost_node->f) {
            if (open_list[i]->h < lowest_f_cost_node->h) {
                lowest_f_cost_node = open_list[i];
            }
        }
    }
    return lowest_f_cost_node;
}

void backtrack(Node *target_node) {
    puts("");
    puts("Starting backtrack...");

    Node *parent_node = target_node->parent;
    int i = 1;
    while (parent_node != NULL) {
        printf("Parent node #%d... f: %f, g: %f, h: %f\n", i, parent_node->f,
               parent_node->g, parent_node->h);
        i++;
        parent_node = parent_node->parent;
    }
}

void a_star(Node *start_node, Node *end_node) {
    int open_list_capacity = 100;
    Node **open_list = (Node **)malloc(open_list_capacity * sizeof(Node *));
    if (open_list == NULL) {
        puts("Failed to allocate memory for open list...");
        exit(1);
    }
    int open_list_size = 0;

    int closed_list_capacity = 100;
    Node **closed_list = (Node **)malloc(closed_list_capacity * sizeof(Node *));
    if (closed_list == NULL) {
        puts("Failed to allocate memory for closed list...");
        exit(1);
    }
    int closed_list_size = 0;

    start_node->h = manhattan_distance(start_node, end_node);
    start_node->g = 0;
    start_node->f = start_node->g + start_node->h;
    start_node->parent = NULL;

    open_list[0] = start_node;
    open_list_size++;

    while (open_list_size > 0) {
        Node *current_node = get_lowest_f_node(open_list, open_list_size);
        if (current_node == NULL) {
            puts("Current node is null...");
            exit(1);
        }

        for (int j = 0; j < open_list_size; j++) {
            if (open_list[j] == current_node) {
                open_list_size--;
                open_list[j] = open_list[open_list_size];
                break;
            }
        }

        if (closed_list_size >= closed_list_capacity) {
            closed_list_capacity *= 2;
            closed_list =
                realloc(closed_list, closed_list_capacity * sizeof(Node *));
            if (closed_list == NULL) {
                puts("Failed to reallocate memory to closed list...");
                exit(1);
            }
        }

        closed_list[closed_list_size] = current_node;
        closed_list_size++;

        printf("Current node... f: %f, g: %f, h: %f\n", current_node->f,
               current_node->g, current_node->h);

        if (current_node == end_node) {
            backtrack(current_node);
            break;
        }

        Node *neighbor_nodes[4];
        neighbor_nodes[0] = current_node->neighbors[0];  // Left neighbor
        neighbor_nodes[1] = current_node->neighbors[1];  // Right neighbor
        neighbor_nodes[2] = current_node->neighbors[2];  // Down neighbor
        neighbor_nodes[3] = current_node->neighbors[3];  // Up neighbor
        int num_of_neighbor_nodes = 4;

        for (int i = 0; i < num_of_neighbor_nodes; i++) {
            Node *neighbor_node = neighbor_nodes[i];

            if (neighbor_node == NULL) {
                continue;
            }

            bool isInClosedList = false;
            for (int k = 0; k < closed_list_size; k++) {
                if (closed_list[k] == neighbor_node) {
                    isInClosedList = true;
                    break;
                }
            }
            if (isInClosedList == true) {
                continue;
            }

            neighbor_node->parent = current_node;
            neighbor_node->g = current_node->g +
                               manhattan_distance(neighbor_node, current_node);
            neighbor_node->h = manhattan_distance(neighbor_node, end_node);
            neighbor_node->f = neighbor_node->g + neighbor_node->h;

            for (int c = 0; c < open_list_size; c++) {
                if (open_list[c]->x == neighbor_node->x &&
                    open_list[c]->y == neighbor_node->y) {
                    if (neighbor_node->g > open_list[c]->g) {
                        continue;
                    }
                }
            }

            if (open_list_size >= open_list_capacity) {
                open_list_capacity *= 2;
                open_list =
                    realloc(open_list, open_list_capacity * sizeof(Node *));
                if (open_list == NULL) {
                    puts("Failed to reallocate memory to open list...");
                    exit(1);
                }
            }

            open_list[open_list_size] = neighbor_node;
            open_list_size++;
        }
    }
    free(open_list);
    free(closed_list);
}

int main() {
    srand(time(NULL));
    generate_grid();

    Node *start_node = random_node();
    Node *end_node = random_node();
    int distance = manhattan_distance(start_node, end_node);

    while (distance < 9) {
        start_node = random_node();
        end_node = random_node();
        distance = manhattan_distance(start_node, end_node);
    }

    puts("Commencing operation A*...");
    puts("");
    printf("start node --- x: %d, y: %d \n", start_node->x, start_node->y);
    printf("end node --- x: %d, y: %d \n", end_node->x, end_node->y);
    printf("Distance between start and end node: %d\n", distance);
    puts("");

    a_star(start_node, end_node);

    for (int x = 0; x < GridWidth; x++) {
        for (int y = 0; y < GridHeight; y++) {
            free(nodes[x][y]);
        }
    }

    return 0;
}