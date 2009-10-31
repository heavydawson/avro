/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
#include "avro_private.h"

struct avro_array_value
{
  apr_array_header_t *values;

  struct avro_value *items;

  apr_pool_t *pool;

  int value_set;

  /* Need the ctx to dupe value */
  struct avro_value_ctx *ctx;

  struct avro_value base_value;
};

static void
avro_array_print (struct avro_value *value, FILE * fp)
{
  struct avro_array_value *self =
    container_of (value, struct avro_array_value, base_value);

  avro_value_indent (value, fp);
  fprintf (fp, "array(%p) value items\n", self);
  avro_value_print_info (self->items, fp);
}

static avro_status_t
avro_array_read_skip (struct avro_value *value, struct avro_reader *reader,
		      int skip)
{
  avro_status_t status;
  avro_long_t i, count;
  struct avro_array_value *self =
    container_of (value, struct avro_array_value, base_value);
  struct avro_io_reader *io;

  apr_pool_clear (self->pool);

  if (!reader)
    {
      return AVRO_FAILURE;
    }
  io = reader->io;
  if (!io)
    {
      return AVRO_FAILURE;
    }

  status = avro_read_long (io, &count);
  if (status != AVRO_OK)
    {
      return status;
    }

  if (count < 0)
    {
      /* TODO */
    }

  /* TODO */
  return AVRO_OK;
}

static avro_status_t
avro_array_read (struct avro_value *value, struct avro_reader *reader)
{
  return avro_array_read_skip (value, reader, 0);
}

static avro_status_t
avro_array_skip (struct avro_value *value, struct avro_reader *reader)
{
  return avro_array_read_skip (value, reader, 1);
}

static avro_status_t
avro_array_write (struct avro_value *value, struct avro_writer *writer)
{
/* TODO
  struct avro_array_value *self =
    container_of (value, struct avro_array_value, base_value);
*/
  return AVRO_OK;
}

static struct avro_value *
avro_array_create (struct avro_value_ctx *ctx, struct avro_value *parent,
		   apr_pool_t * pool, const JSON_value * json)
{
  struct avro_array_value *self;
  const JSON_value *items;

  self = apr_palloc (pool, sizeof (struct avro_array_value));
  if (!self)
    {
      return NULL;
    }
  self->base_value.type = AVRO_ARRAY;
  self->base_value.pool = pool;
  self->base_value.parent = parent;
  self->base_value.schema = json;

  /* collect and save required items */
  items = json_attr_get (json, L"items");
  if (!items)
    {
      return NULL;
    }
  self->items = avro_value_from_json (ctx, parent, items);
  if (!self->items)
    {
      return NULL;
    }
  /* Create a pool for the value processing */
  if (apr_pool_create (&self->pool, pool) != APR_SUCCESS)
    {
      return NULL;
    }
  self->value_set = 0;
  self->ctx = ctx;
  return &self->base_value;
}

const struct avro_value_info avro_array_info = {
  .name = L"array",
  .type = AVRO_ARRAY,
  .private = 0,
  .create = avro_array_create,
  .formats = {{
	       .read_data = avro_array_read,
	       .skip_data = avro_array_skip,
	       .write_data = avro_array_write},
	      {
	       /* TODO: import/export */
	       .read_data = avro_array_read,
	       .skip_data = avro_array_skip,
	       .write_data = avro_array_write}},
  .print_info = avro_array_print
};
