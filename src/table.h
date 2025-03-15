#ifndef TABLE_H
#define TABLE_H

struct value;

struct table_entry
{
  const char *key;
  struct value *value;
};

#endif // TABLE_H

