#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "yaml.h"
#include "todo.h"

todo_t* todo_from_doc(yaml_document_t* doc);

todo_t* todo_from_seq(yaml_document_t* doc, yaml_node_t* seq);

task_t* task_from_node(yaml_document_t* doc, yaml_node_t* node);

task_t* task_from_mapping(yaml_document_t* doc, yaml_node_t* mapping);

task_t* task_from_scalar(yaml_node_t* scalar);

char* task_name_from_scalar(yaml_node_t* scalar);

char* task_desc_from_scalar(yaml_node_t* scalar);

task_status_t task_status_from_scalar(yaml_node_t* scalar);

task_priority_t task_priority_from_scalar(yaml_node_t* scalar);

todo_t* todo_read(FILE* file) {
  yaml_parser_t parser;
  yaml_document_t doc;

  yaml_parser_initialize(&parser);
  yaml_parser_set_input_file(&parser, file);
  yaml_parser_load(&parser, &doc);

  todo_t* todo = todo_from_doc(&doc);

  yaml_parser_delete(&parser);
  yaml_document_delete(&doc);

  if (!todo) return NULL;

  return todo;
} 

todo_t* todo_from_doc(yaml_document_t* doc) {
  yaml_node_t* root = yaml_document_get_root_node(doc);

  if (!root) return NULL; 

  yaml_node_t* node = yaml_document_get_node(doc, 1);

  if (node->type == YAML_SEQUENCE_NODE)
    return todo_from_seq(doc, node);
  else
    return NULL;
}

todo_t* todo_from_seq(yaml_document_t* doc, yaml_node_t* seq) {
  yaml_node_t* node;
  yaml_node_item_t* i;

  todo_t* todo = todo_create();
  task_t* task;

  for (i = seq->data.sequence.items.start;
      i != seq->data.sequence.items.top; i++) {
    node = yaml_document_get_node(doc, (int) *i);
    task = task_from_node(doc, node);

    if (todo && task) todo_insert(todo, task);
  }

  return todo;
}

task_t* task_from_node(yaml_document_t* doc, yaml_node_t* node) {
  switch(node->type) {
  case YAML_SCALAR_NODE:
    return task_from_scalar(node);

  case YAML_MAPPING_NODE:
    return task_from_mapping(doc, node);

  case YAML_SEQUENCE_NODE:
    return NULL;

  case YAML_NO_NODE:
  default:
    return NULL;
  };
}

task_t* task_from_mapping(yaml_document_t* doc, yaml_node_t* node) {
  task_t* task;
  task_status_t stat;
  task_priority_t prio;

  yaml_node_pair_t* pair;
  yaml_node_t* k_node;
  yaml_node_t* v_node;

  char* name;
  char* desc;
  char* key;
  char* stat_value;
  char* prio_value;
  todo_t* deps;

  name = NULL;
  desc = NULL;
  stat = INCOMPLETE;
  prio = NORMAL;
  deps = NULL;

  for (pair = node->data.mapping.pairs.start; 
      pair != node->data.mapping.pairs.top; pair++) {
    k_node = yaml_document_get_node(doc, pair->key);
    v_node = yaml_document_get_node(doc, pair->value);
    key = (char*) k_node->data.scalar.value;

    if (strcmp(key, "name") == 0) {
      assert(v_node->type == YAML_SCALAR_NODE);
      name = (char *) v_node->data.scalar.value;
    } else if (strcmp(key, "description") == 0) {
      assert(v_node->type == YAML_SCALAR_NODE);
      desc = (char *) v_node->data.scalar.value;
    } else if (strcmp(key, "status") == 0) {
      assert(v_node->type == YAML_SCALAR_NODE);
      stat_value = (char*) v_node->data.scalar.value;
      if (strcmp(stat_value, "complete") == 0)
        stat = COMPLETE;
    } else if (strcmp(key, "priority") == 0) {
      assert(v_node->type == YAML_SCALAR_NODE);
      prio_value = (char*) v_node->data.scalar.value;
      if (strcmp(prio_value, "urgent") == 0)
        prio = URGENT;
      else if (strcmp(prio_value, "high") == 0)
        prio = HIGH;
      else if (strcmp(prio_value, "low") == 0)
        prio = LOW;
    } else if (strcmp(key, "tasks") == 0) {
      assert(v_node->type == YAML_SEQUENCE_NODE);
      deps = todo_from_seq(doc, v_node);
    }
  }

  if (desc) {
    task = task_create(name, desc);
    task->status = stat;
    task->priority = prio;
    if (deps) task->todos = deps;
    return task;
  }

  // even though we couldn't create a task, a todo may have been allocated for the tasks member
  if (deps) todo_destroy(deps);

  return NULL;
}

task_t* task_from_scalar(yaml_node_t* scalar) {
  return task_create(NULL, (char*) scalar->data.scalar.value);
}
