#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  if (!t) {
    printf("메모리 할당에 실패하였습니다.\n");
    return NULL;
  }

  // NIL 노드를 생성합니다. 생성된 NIL 노드는 트리 내에서 계속 사용됩니다.
  node_t *nil_node = (node_t*)calloc(1, sizeof(node_t));
  if (!nil_node) {
    printf("메모리 할당에 실패하였습니다.\n");
    free(t);
    return NULL;
  }

  // NIL 노드의 멤버를 설정합니다.
  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node;
  nil_node->left = nil_node;
  nil_node->right = nil_node;

  // 트리의 멤버를 설정합니다.
  t->root = nil_node;
  t->nil = nil_node;

  return t;
}

void delete_rbtree(rbtree *t) {

  if (!t) return;

  // 스택 공간을 할당합니다.
  node_t *stack[1024];
  int stack_top = 0;

  // 스택에 루트 노드를 넣고 스택 포인터를 +1합니다.
  if (t->root != t->nil) {
    stack[stack_top] = t->root;
    stack_top++;
  }

  // 스택이 비어있을 때까지 반복합니다.
  while (stack_top > 0) {

    // 스택에서 노드를 pop합니다.
    stack_top--;
    node_t *node = stack[stack_top];

    // 오른쪽 노드를 먼저 넣어서 전위 순회를 구현합니다.
    // NIL노드가 아니면 오른쪽 노드를 스택에 push합니다.
    if (node->right != t->nil) {
      stack[stack_top] = node->right;
      stack_top++;
    }

    // NIL노드가 아니면 왼쪽 노드를 스택에 push합니다.
    if (node->left != t->nil) {
      stack[stack_top] = node->left;
      stack_top++;
    }

    // 꺼낸 노드를 해제합니다.
    free(node);
  }

  // 모든 노드를 해제 후에 마지막으로 NIL노드를 해제합니다.
  free(t->nil);

  // 트리를 해제합니다.
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
