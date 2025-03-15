#ifndef TABLE_H
#define TABLE_H

#include <stddef.h>

struct value;

struct table_entry
{
  char *key;
  struct value *value;
  struct table_entry *next;
};

struct table
{
  struct table_entry **storage;
  size_t size;
  size_t capacity;
  struct table *parent;
};

struct table_entry *table_entry_create (const char *, struct value *);
struct table_entry *table_entry_copy (struct table_entry *);

void table_entry_destroy (struct table_entry *);

struct table *table_create (size_t, struct table *);
struct table *table_copy (struct table *);

void table_destroy (struct table *);

void table_append (struct table *, struct table_entry *);
struct value *table_find (struct table *, const char *);

#endif // TABLE_H

