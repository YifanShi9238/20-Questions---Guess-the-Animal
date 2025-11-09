#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lab5.h"

extern Node *g_root;

#define MAGIC 0x41544C35  /* "ATL5" */
#define VERSION 1

typedef struct {
    Node *node;
    int id;
} NodeMapping;

/* TODO 27: Implement save_tree
 * Save the tree to a binary file using BFS traversal
 * 
 * Binary format:
 * - Header: magic (4 bytes), version (4 bytes), nodeCount (4 bytes)
 * - For each node in BFS order:
 *   - isQuestion (1 byte)
 *   - textLen (4 bytes)
 *   - text (textLen bytes, no null terminator)
 *   - yesId (4 bytes, -1 if NULL)
 *   - noId (4 bytes, -1 if NULL)
 * 
 * Steps:
 * 1. Return 0 if g_root is NULL
 * 2. Open file for writing binary ("wb")
 * 3. Initialize queue and NodeMapping array
 * 4. Use BFS to assign IDs to all nodes:
 *    - Enqueue root with id=0
 *    - Store mapping[0] = {g_root, 0}
 *    - While queue not empty:
 *      - Dequeue node and id
 *      - If node has yes child: add to mappings, enqueue with new id
 *      - If node has no child: add to mappings, enqueue with new id
 * 5. Write header (magic, version, nodeCount)
 * 6. For each node in mapping order:
 *    - Write isQuestion, textLen, text bytes
 *    - Find yes child's id in mappings (or -1)
 *    - Find no child's id in mappings (or -1)
 *    - Write yesId, noId
 * 7. Clean up and return 1 on success
 */

 // https://chatgpt.com/share/690f9b07-6af0-8009-9de7-4b803f83f797
int save_tree(const char *filename) {
    if (g_root == NULL)
        return 0;

    FILE *f = fopen(filename, "wb");
    if (!f)
        return 0;

    Queue q;
    q_init(&q);

    NodeMapping *map = malloc(sizeof(NodeMapping) * 10000);
    int mapCount = 0;

    // Enqueue root
    q_enqueue(&q, g_root, 0);
    map[0].node = g_root;
    map[0].id = 0;
    mapCount = 1;

    // BFS traversal to assign IDs
    while (!q_empty(&q)) {
        Node *curr;
        int currId;
        q_dequeue(&q, &curr, &currId);

        if (curr->yes) {
            map[mapCount].node = curr->yes;
            map[mapCount].id = mapCount;
            q_enqueue(&q, curr->yes, mapCount);
            mapCount++;
        }
        if (curr->no) {
            map[mapCount].node = curr->no;
            map[mapCount].id = mapCount;
            q_enqueue(&q, curr->no, mapCount);
            mapCount++;
        }
    }

    // Write header
    uint32_t magic = MAGIC, version = VERSION, count = mapCount;
    fwrite(&magic, sizeof(uint32_t), 1, f);
    fwrite(&version, sizeof(uint32_t), 1, f);
    fwrite(&count, sizeof(uint32_t), 1, f);

    // Helper: find ID by node pointer
    auto int find_id(Node *n) {
        if (!n)
            return -1;
        for (int i = 0; i < mapCount; i++)
            if (map[i].node == n)
                return map[i].id;
        return -1;
    }

    // Write nodes in mapping order
    for (int i = 0; i < mapCount; i++) {
        Node *n = map[i].node;
        uint8_t isQ = (uint8_t)n->isQuestion;
        uint32_t len = (uint32_t)strlen(n->text);
        int32_t yesId = find_id(n->yes);
        int32_t noId = find_id(n->no);

        fwrite(&isQ, sizeof(uint8_t), 1, f);
        fwrite(&len, sizeof(uint32_t), 1, f);
        fwrite(n->text, sizeof(char), len, f);
        fwrite(&yesId, sizeof(int32_t), 1, f);
        fwrite(&noId, sizeof(int32_t), 1, f);
    }

    free(map);
    q_free(&q);
    fclose(f);
    return 1;
}

/* TODO 28: Implement load_tree
 * Load a tree from a binary file and reconstruct the structure
 * 
 * Steps:
 * 1. Open file for reading binary ("rb")
 * 2. Read and validate header (magic, version, count)
 * 3. Allocate arrays for nodes and child IDs:
 *    - Node **nodes = calloc(count, sizeof(Node*))
 *    - int32_t *yesIds = calloc(count, sizeof(int32_t))
 *    - int32_t *noIds = calloc(count, sizeof(int32_t))
 * 4. Read each node:
 *    - Read isQuestion, textLen
 *    - Validate textLen (e.g., < 10000)
 *    - Allocate and read text string (add null terminator!)
 *    - Read yesId, noId
 *    - Validate IDs are in range [-1, count)
 *    - Create Node and store in nodes[i]
 * 5. Link nodes using stored IDs:
 *    - For each node i:
 *      - If yesIds[i] >= 0: nodes[i]->yes = nodes[yesIds[i]]
 *      - If noIds[i] >= 0: nodes[i]->no = nodes[noIds[i]]
 * 6. Free old g_root if not NULL
 * 7. Set g_root = nodes[0]
 * 8. Clean up temporary arrays
 * 9. Return 1 on success
 * 
 * Error handling:
 * - If any read fails or validation fails, goto load_error
 * - In load_error: free all allocated memory and return 0
 */
int load_tree(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;

    uint32_t magic, version, count;
    if (fread(&magic, sizeof(uint32_t), 1, f) != 1 ||
        fread(&version, sizeof(uint32_t), 1, f) != 1 ||
        fread(&count, sizeof(uint32_t), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    if (magic != MAGIC || version != VERSION || count == 0) {
        fclose(f);
        return 0;
    }

    Node **nodes = calloc(count, sizeof(Node *));
    int32_t *yesIds = calloc(count, sizeof(int32_t));
    int32_t *noIds = calloc(count, sizeof(int32_t));

    for (uint32_t i = 0; i < count; i++) {
        uint8_t isQ;
        uint32_t len;
        if (fread(&isQ, sizeof(uint8_t), 1, f) != 1 ||
            fread(&len, sizeof(uint32_t), 1, f) != 1) {
            goto load_error;
        }
        if (len > 10000)
            goto load_error;

        char *buf = malloc(len + 1);
        if (!buf || fread(buf, sizeof(char), len, f) != len) {
            free(buf);
            goto load_error;
        }
        buf[len] = '\0';

        int32_t yesId, noId;
        if (fread(&yesId, sizeof(int32_t), 1, f) != 1 ||
            fread(&noId, sizeof(int32_t), 1, f) != 1) {
            free(buf);
            goto load_error;
        }

        nodes[i] = isQ ? create_question_node(buf)
                       : create_animal_node(buf);
        yesIds[i] = yesId;
        noIds[i] = noId;

        free(buf);
    }

    // Rebuild links
    for (uint32_t i = 0; i < count; i++) {
        if (yesIds[i] >= 0 && yesIds[i] < (int32_t)count)
            nodes[i]->yes = nodes[yesIds[i]];
        if (noIds[i] >= 0 && noIds[i] < (int32_t)count)
            nodes[i]->no = nodes[noIds[i]];
    }

    if (g_root)
        free_tree(g_root);
    g_root = nodes[0];

    free(nodes);
    free(yesIds);
    free(noIds);
    fclose(f);
    return 1;

load_error:
    if (nodes) {
        for (uint32_t i = 0; i < count; i++)
            if (nodes[i])
                free(nodes[i]);
        free(nodes);
    }
    free(yesIds);
    free(noIds);
    fclose(f);
    return 0;
}