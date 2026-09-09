# Hash Table From Scratch in C

A hash table implementation written from scratch in **C**, without relying on built-in hash table libraries.

The project implements a dynamically resizing hash table using **open addressing**, **double hashing**, and **tombstone deletion**. It also includes an interactive command-line interface for inserting, searching, deleting, printing, and generating random values.

---

## Features

* Hash table implemented from scratch in C
* Open addressing
* Double hashing for collision resolution
* Dynamic resizing

  * Automatically grows when the table becomes too full
  * Automatically shrinks when the table becomes mostly empty
* Tombstone-based deletion
* Updating existing keys
* Random data generation
* Interactive CLI
* Prime-number table sizes
* Memory management and cleanup
* Separate hash table and prime-number modules

---

## How It Works

### Hashing

The hash table uses two hash functions to implement **double hashing**.

For a key `s`, the first hash function determines the initial position:

```text
hash1(key)
```

The second hash function determines the step size used when a collision occurs:

```text
hash2(key)
```

The probe sequence is:

```text
index = (hash1(key) + attempt × (hash2(key) + 1)) % table_size
```

This allows the table to search for another available position without using linked lists.

---

## Collision Handling

This implementation uses **open addressing**.

When two keys produce the same index:

```text
             ┌─────────────┐
"apple" ────►│             │
             │   Bucket 5  │
"orange" ───►│             │
             └─────────────┘
```

the second key is not stored in a linked list.

Instead, the hash table continues searching using the double-hashing probe sequence:

```text
Bucket 5 → Bucket 17 → Bucket 31 → Bucket 42 → ...
```

until an empty position is found.

---

## Deletion

Deleting an item directly by setting its bucket to `NULL` would break the probe sequence.

For example:

```text
[05] apple
[17] banana
[31] orange
```

If `apple` is deleted and bucket `05` simply becomes empty, searching for `banana` could stop at bucket `05` and incorrectly conclude that the key does not exist.

Instead, this implementation uses a **tombstone**:

```text
[05] DELETED
[17] banana
[31] orange
```

The search operation continues through deleted positions.

When inserting a new key, the implementation can also reuse a tombstone position.

---

## Dynamic Resizing

The hash table automatically changes its size depending on the load factor.

### Resize Up

When the table becomes more than approximately **70% full**, it is resized.

```text
53 buckets
    ↓
107 buckets
    ↓
211 buckets
    ↓
...
```

The existing elements are rehashed into the new table.

### Resize Down

When the table becomes less than approximately **10% full**, the table is resized down.

```text
211 buckets
    ↓
107 buckets
    ↓
53 buckets
```

The table never shrinks below its initial size.

---

## Prime Table Sizes

The table sizes are chosen as prime numbers.

This helps the double-hashing probe sequence cover the table effectively and reduces the likelihood of problematic probe patterns.

The project includes a separate prime-number module:

```text
prime.c
prime.h
```

which provides:

* `is_prime()`
* `next_prime()`

---

## Project Structure

```text
hashtable-from-scratch/
│
├── src/
│   ├── main.c
│   ├── hashtable.c
│   ├── hashtable.h
│   ├── prime.c
│   └── prime.h
│
├── build/
│   └── hashtable
│
└── README.md
```

---

## Main Components

### `hashtable.h`

Contains the data structures and public functions used by the hash table.

The main structures are:

```c
typedef struct {
    char* key;
    char* value;
} ht_item;
```

and:

```c
typedef struct {
    int size;
    int count;
    int base_size;
    ht_item** items;
} ht_hash_table;
```

---

### `hashtable.c`

Contains the main hash table implementation.

Important operations include:

```c
ht_new()
ht_insert()
ht_search()
ht_delete()
ht_del_hash_table()
```

It also contains the internal hashing, resizing, memory-management, and tombstone logic.

---

### `prime.c`

Provides prime-number utilities used when determining the size of the hash table.

```c
is_prime()
next_prime()
```

---

### `main.c`

Provides an interactive CLI for interacting with the hash table.

The available operations are:

```text
1. Insert
2. Search
3. Delete
4. Print table
5. Insert random values
6. Exit
```

---

## CLI Example

Running the program displays:

```text
========================================
          HASH TABLE CLI
========================================
1. Insert
2. Search
3. Delete
4. Print table
5. Insert random values
6. Exit
========================================

Choice:
```

### Insert

```text
Choice: 1

Enter key: name
Enter value: Fahim

Inserted successfully.
```

### Search

```text
Choice: 2

Enter key to search: name

Found!
Key   : name
Value : Fahim
```

### Delete

```text
Choice: 3

Enter key to delete: name

Deleted successfully.
```

### Random Insertion

```text
Choice: 5

How many random values? 100

Successfully inserted 100 random values.
```

This can be used to quickly populate the table and observe resizing and collision handling.

---

## Testing

The CLI can be used to test the main functionality of the hash table:

### Basic operations

1. Insert a key/value pair
2. Search for the key
3. Update an existing key
4. Delete the key
5. Search for the deleted key

### Collision testing

Insert many values:

```text
Choice: 5
How many random values? 100
```

Then print the table to inspect how entries are distributed.

### Resizing testing

Insert enough values to exceed the load threshold and observe the table size increase.

Then delete enough values to trigger a resize down.

---

## Memory Management

The project manually manages memory using:

```c
malloc()
calloc()
free()
```

Keys and values are dynamically allocated when an item is created.

When an item is deleted, its key, value, and structure are freed.

The entire hash table can be cleaned up using:

```c
ht_del_hash_table()
```

The implementation also avoids freeing the static tombstone object used to represent deleted entries.

---

## Complexity

For a well-distributed hash table:

| Operation | Average | Worst Case |
| --------- | ------- | ---------- |
| Insert    | O(1)    | O(n)       |
| Search    | O(1)    | O(n)       |
| Delete    | O(1)    | O(n)       |

The average-case performance is close to constant time because hashing allows the table to directly determine where a key should be located.

The worst case occurs when many keys collide and a large portion of the table must be probed.

---

## Technologies

* **Language:** C
* **Compiler:** GCC
* **Standard:** C11
* **Libraries:** Standard C Library, Math Library
* **Data Structure:** Hash Table
* **Collision Resolution:** Double Hashing
* **Storage Strategy:** Open Addressing

---

## Learning Goals

This project was built to understand how hash tables work internally.

It demonstrates concepts including:

* Hash functions
* Modular arithmetic
* Collision resolution
* Open addressing
* Double hashing
* Tombstones
* Load factors
* Dynamic resizing
* Prime numbers
* Pointers
* Dynamic memory allocation
* Structs
* Separate compilation
* Header files
* C memory management

---

## Future Improvements

Possible improvements include:

* More robust hash functions
* Improved CLI visualization
* Probe-sequence visualization
* Collision statistics
* Load-factor statistics
* Configurable initial table size
* Benchmarking against other hash table implementations
* Automated unit tests
* Support for integer keys
* Generic key/value support
* More efficient integer-based hashing

---
