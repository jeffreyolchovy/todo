#include<stdio.h>
#include "yaml.h"
#include "todo.h"

void todo_write(todo_t* todo, FILE* file);

int doc_append_todo(yaml_document_t* doc, todo_t* todo);

void todo_write(todo_t* todo, FILE* file) {
  yaml_emitter_t emitter;
  yaml_document_t doc;

  yaml_emitter_initialize(&emitter);
  yaml_document_initialize(&doc, NULL, NULL, NULL, 0, 0);

  doc_append_todo(&doc, todo);

  yaml_emitter_set_output_file(&emitter, file);
  yaml_emitter_open(&emitter);
  yaml_emitter_dump(&emitter, &doc);
  yaml_emitter_close(&emitter);

  yaml_emitter_delete(&emitter);
  yaml_document_delete(&doc);
}

void doc_append_task(yaml_document_t* doc, int root, task_t* task) {
  int node;

  if (!task->key && task->status == INCOMPLETE && task->priority == NORMAL && !task->todo) {
    node = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->value, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
    yaml_document_append_sequence_item(doc, root, node);
  } else {
    int key = 0;
    int value = 0;

    node = yaml_document_add_mapping(doc, NULL, YAML_BLOCK_MAPPING_STYLE);
    yaml_document_append_sequence_item(doc, root, node);

    key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "description", -1, YAML_PLAIN_SCALAR_STYLE);
    value = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->value, -1, YAML_DOUBLE_QUOTED_SCALAR_STYLE);
    yaml_document_append_mapping_pair(doc, node, key, value);

    if (task->key) {
      key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "name", -1, YAML_PLAIN_SCALAR_STYLE);
      value = yaml_document_add_scalar(doc, NULL, (unsigned char*) task->key, -1, YAML_PLAIN_SCALAR_STYLE);
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

    if (task->todo) {
      key = yaml_document_add_scalar(doc, NULL, (unsigned char*) "tasks", -1, YAML_PLAIN_SCALAR_STYLE);
      value = doc_append_todo(doc, task->todo);
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
  size_t i;

  for (i = 0; i < map->lists_size; i++)
    if (map->lists[i])
      doc_append_tasklist(doc, root, map->lists[i]);
}

int doc_append_todo(yaml_document_t* doc, todo_t* todo) {
  int root = yaml_document_add_sequence(doc, NULL, YAML_ANY_SEQUENCE_STYLE);

  if (todo->map) doc_append_taskmap(doc, root, todo->map);

  if (todo->list) doc_append_tasklist(doc, root, todo->list);

  return root;
}
