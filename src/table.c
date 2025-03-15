#include "string.h"
#include "table.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>

struct table_entry *
table_entry_create (const char *key, struct value *value)
{
  struct table_entry *entry;

  entry = calloc (1, sizeof (struct table_entry));

  entry->key = string_copy (key, NULL);
  entry->value = value_copy (value);

  return entry;
}

struct table_entry *
table_entry_copy (struct table_entry *entry)
{
  return table_entry_create (entry->key, entry->value);
}

void
table_entry_destroy (struct table_entry *entry)
{
  free (entry->key);
  value_destroy (entry->value);
  free (entry);
}

struct table *
table_create (size_t capacity, struct table *parent)
{
  struct table *table;

  table = calloc (1, sizeof (struct table));

  table->storage = calloc (capacity, sizeof (struct table_entry *));
  table->capacity = capacity;
  table->parent = parent;

  return table;
}

struct table *
table_copy (struct table *table)
{
  struct table *copy;

  copy = table_create (table->capacity, table->parent);

  for (size_t i = 0; i < table->size; ++i)
    copy->storage[i] = table_entry_copy (table->storage[i]);

  copy->size = table->size;

  return copy;
}

void
table_destroy (struct table *table)
{
  for (size_t i = 0; i < table->size; ++i)
    table_entry_destroy (table->storage[i]);
  free (table->storage);
  free (table);
}

void
table_append (struct table *table, struct table_entry *entry)
{
  if (table->size >= table->capacity)
    {
      table->capacity *= 2;
      size_t size = table->capacity * sizeof (struct table_entry *);
      table->storage = realloc (table->storage, size);
    }

  table->storage[table->size++] = entry;
}

struct value *
table_find (struct table *table, const char *key)
{
  for (size_t i = 0; i < table->size; ++i)
    if (strcmp (table->storage[i]->key, key) == 0)
      return table->storage[i]->value;

  if (table->parent)
    return table_find (table->parent, key);

  return NULL;
}

