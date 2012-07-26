#include<stdio.h>
#include "yaml.h"
#include "todo.h"

void tasktable_write(tasktable_t* table, FILE* file);

int doc_append_tasktable(yaml_document_t* doc, tasktable_t* table);

void tasktable_write(tasktable_t* tasktable, FILE* file) {
  yaml_emitter_t emitter;
  yaml_document_t doc;

  yaml_emitter_initialize(&emitter);
  yaml_document_initialize(&doc, NULL, NULL, NULL, 0, 0);

  doc_append_tasktable(&doc, tasktable);

  yaml_emitter_set_output_file(&emitter, file);
  yaml_emitter_open(&emitter);
  yaml_emitter_dump(&emitter, &doc);
  yaml_emitter_close(&emitter);

  yaml_emitter_delete(&emitter);
  yaml_document_delete(&doc);
}

void doc_append_task(yaml_document_t* doc, int root, task_t* task) {
  int node;

  if (!task->name && task->status == INCOMPLETE && task->priority == NORMAL && !task->tasks) {
    node = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->description, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
    yaml_document_append_sequence_item(doc, root, node);
  } else {
    int key = 0;
    int value = 0;

    node = yaml_document_add_mapping(doc, NULL, YAML_BLOCK_MAPPING_STYLE);
    yaml_document_append_sequence_item(doc, root, node);

    key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "description", -1, YAML_PLAIN_SCALAR_STYLE);
    value = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->description, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
    yaml_document_append_mapping_pair(doc, node, key, value);

    if (task->name) {
      key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "name", -1, YAML_PLAIN_SCALAR_STYLE);
      value = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->name, -1, YAML_PLAIN_SCALAR_STYLE);
      yaml_document_append_mapping_pair(doc, node, key, value);
    }

    if (task->status) {
      key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "status", -1, YAML_PLAIN_SCALAR_STYLE);
      value = yaml_document_add_scalar(doc, NULL, (unsigned char*) "complete", -1, YAML_PLAIN_SCALAR_STYLE);
      yaml_document_append_mapping_pair(doc, node, key, value);
    }

    if (task->priority != NORMAL) {
      unsigned char* prio_value = NULL;

      if (task->priority == URGENT)
        prio_value = (unsigned char*) "urgent";

      else if (task->priority == HIGH)
        prio_value = (unsigned char*) "high";

      else if (task->priority == LOW)
        prio_value = (unsigned char*) "low";

      if (prio_value) {
        key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "priority", -1, YAML_PLAIN_SCALAR_STYLE);
        value = yaml_document_add_scalar(doc, NULL, prio_value, -1, YAML_PLAIN_SCALAR_STYLE);
        yaml_document_append_mapping_pair(doc, node, key, value);
      }
    }

    if (task->tasks) {
      key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "tasks", -1, YAML_PLAIN_SCALAR_STYLE);
      value = doc_append_tasktable(doc, task->tasks);
      yaml_document_append_mapping_pair(doc, node, key, value);
    }
  }
}

void doc_append_tasklist(yaml_document_t* doc, int root, tasklist_t* list) {
  while(list) {
    doc_append_task(doc, root, list->current);
    list = list->next;
  }
}

void doc_append_taskmap(yaml_document_t* doc, int root, taskmap_t* map) {
  int i;

  for (i = 0; i < map->size; i++)
    if (map->lists[i])
      doc_append_tasklist(doc, root, map->lists[i]);
}

int doc_append_tasktable(yaml_document_t* doc, tasktable_t* table) {
  int root = yaml_document_add_sequence(doc, NULL, YAML_ANY_SEQUENCE_STYLE);

  if (table->map) doc_append_taskmap(doc, root, table->map);

  if (table->list) doc_append_tasklist(doc, root, table->list);

  return root;
}
