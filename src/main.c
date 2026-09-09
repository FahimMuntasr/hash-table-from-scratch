#include <stdio.h>
#include <string.h>
#include "hashtable.h"

int main(void) {

    printf("Creating hash table...\n");

    ht_hash_table* ht = ht_new();

    if (ht == NULL) {
        printf("Failed to create hash table.\n");
        return 1;
    }

    printf("Initial size: %d\n", ht->size);
    printf("Initial count: %d\n\n", ht->count);


    // --------------------------------------------------
    // TEST 1: Basic insertion
    // --------------------------------------------------

    printf("=== TEST 1: INSERT ===\n");

    ht_insert(ht, "name", "Fahim");
    ht_insert(ht, "age", "21");
    ht_insert(ht, "country", "Bangladesh");

    printf("Count after insertion: %d\n", ht->count);

    printf("name    -> %s\n", ht_search(ht, "name"));
    printf("age     -> %s\n", ht_search(ht, "age"));
    printf("country -> %s\n\n", ht_search(ht, "country"));


    // --------------------------------------------------
    // TEST 2: Searching for missing key
    // --------------------------------------------------

    printf("=== TEST 2: SEARCH MISSING KEY ===\n");

    char* result = ht_search(ht, "does_not_exist");

    if (result == NULL) {
        printf("Correct: key was not found.\n\n");
    }
    else {
        printf("ERROR: key should not exist.\n\n");
    }


    // --------------------------------------------------
    // TEST 3: Updating existing key
    // --------------------------------------------------

    printf("=== TEST 3: UPDATE ===\n");

    printf("Before: %s\n", ht_search(ht, "age"));

    ht_insert(ht, "age", "22");

    printf("After:  %s\n", ht_search(ht, "age"));
    printf("Count:  %d\n\n", ht->count);


    // --------------------------------------------------
    // TEST 4: Deletion
    // --------------------------------------------------

    printf("=== TEST 4: DELETE ===\n");

    printf("Before deletion: %s\n", ht_search(ht, "country"));

    ht_delete(ht, "country");

    result = ht_search(ht, "country");

    if (result == NULL) {
        printf("Correct: country was deleted.\n");
    }
    else {
        printf("ERROR: country still exists.\n");
    }

    printf("Count after deletion: %d\n\n", ht->count);


    // --------------------------------------------------
    // TEST 5: Delete nonexistent key
    // --------------------------------------------------

    printf("=== TEST 5: DELETE MISSING KEY ===\n");

    int count_before = ht->count;

    ht_delete(ht, "does_not_exist");

    if (ht->count == count_before) {
        printf("Correct: count did not change.\n");
    }
    else {
        printf("ERROR: count changed.\n");
    }

    printf("\n");


    // --------------------------------------------------
    // TEST 6: Tombstone reuse
    // --------------------------------------------------

    printf("=== TEST 6: INSERT AFTER DELETE ===\n");

    ht_insert(ht, "new_key", "new_value");

    result = ht_search(ht, "new_key");

    if (result != NULL && result[0] != '\0') {
        printf("new_key -> %s\n", result);
        printf("Correct: insertion after deletion works.\n");
    }
    else {
        printf("ERROR: insertion failed.\n");
    }

    printf("\n");


    // --------------------------------------------------
    // TEST 7: Force resizing
    // --------------------------------------------------

    printf("=== TEST 7: RESIZING ===\n");

    printf("Size before mass insertion: %d\n", ht->size);

    for (int i = 0; i < 100; i++) {

        char key[32];
        char value[32];

        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "value%d", i);

        ht_insert(ht, key, value);
    }

    printf("Size after mass insertion: %d\n", ht->size);
    printf("Count: %d\n\n", ht->count);


    // --------------------------------------------------
    // TEST 8: Verify mass insertion
    // --------------------------------------------------

    printf("=== TEST 8: VERIFY INSERTIONS ===\n");

    int errors = 0;

    for (int i = 0; i < 100; i++) {

        char key[32];
        char expected[32];

        snprintf(key, sizeof(key), "key%d", i);
        snprintf(expected, sizeof(expected), "value%d", i);

        result = ht_search(ht, key);

        if (result == NULL || strcmp(result, expected) != 0) {
            printf("ERROR: %s\n", key);
            errors++;
        }
    }

    if (errors == 0) {
        printf("All 100 entries found correctly.\n");
    }
    else {
        printf("%d entries failed.\n", errors);
    }

    printf("\n");


    // --------------------------------------------------
    // TEST 9: Delete many entries
    // --------------------------------------------------

    printf("=== TEST 9: MASS DELETION ===\n");

    int size_before_delete = ht->size;

    for (int i = 0; i < 100; i++) {

        char key[32];

        snprintf(key, sizeof(key), "key%d", i);

        ht_delete(ht, key);
    }

    printf("Size before deletion: %d\n", size_before_delete);
    printf("Size after deletion:  %d\n", ht->size);
    printf("Count: %d\n\n", ht->count);


    // --------------------------------------------------
    // TEST 10: Verify deleted entries
    // --------------------------------------------------

    printf("=== TEST 10: VERIFY DELETIONS ===\n");

    errors = 0;

    for (int i = 0; i < 100; i++) {

        char key[32];

        snprintf(key, sizeof(key), "key%d", i);

        if (ht_search(ht, key) != NULL) {
            printf("ERROR: %s still exists.\n", key);
            errors++;
        }
    }

    if (errors == 0) {
        printf("All 100 entries were deleted correctly.\n");
    }
    else {
        printf("%d deleted entries were still found.\n", errors);
    }


    // --------------------------------------------------
    // Cleanup
    // --------------------------------------------------

    printf("\n=== CLEANUP ===\n");

    ht_del_hash_table(ht);

    printf("Hash table destroyed successfully.\n");

    return 0;
}
