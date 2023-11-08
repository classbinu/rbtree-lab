#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

// 메시지 타입을 정의합니다.
typedef enum
{
  MALLOC_FAILED,
  TREE_GENERATE_FAILED,
  NODE_GENERATE_FAILED,
  TREE_LOAD_FAILED,
  LEFT_ROTATE_FAILED,
  RIGHT_ROTATE_FAILED,
} message_t;

// 타입에 맞는 메시지를 출력하는 함수입니다.
void print_message(message_t type)
{
  switch (type) {
  case MALLOC_FAILED:
    printf("메모리 할당에 실패하였습니다.\n");
    break;
  case TREE_LOAD_FAILED:
    printf("트리를 불러올 수 없습니다.\n");
    break;
  case LEFT_ROTATE_FAILED:
    printf("좌회전에 실패하였습니다.\n");
    break;
  case RIGHT_ROTATE_FAILED:
    printf("우회전에 실패하였습니다\n");
    break;
  case TREE_GENERATE_FAILED:
    printf("트리 생성에 실패하였습니다.\n");
    break;
  case NODE_GENERATE_FAILED:
    printf("노드 생성에 실패하였습니다.\n");
    break;
  default:
    printf("알 수 없는 메시지 타입입니다.\n");
    break;
  }
}

// 새로운 트리를 생성하는 함수입니다.
rbtree *new_rbtree(void)
{
  // 트리를 생성합니다.
  rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));
  if (!tree) {
    print_message(TREE_GENERATE_FAILED);
    return NULL;
  }

  // nil노드를 생성합니다.
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  if (!nil_node) {
    print_message(NODE_GENERATE_FAILED);
    free(tree);
    return NULL;
  }

  // nil노드 초기 멤버를 설정합니다.
  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node;
  nil_node->left = nil_node;
  nil_node->right = nil_node;

  // 트리의 초기 멤버를 설정합니다.
  tree->root = nil_node;
  tree->nil = nil_node;

  return tree;
}

// 생성된 트리를 삭제하는 함수입니다.
void delete_rbtree(rbtree *tree)
{
  if (!tree) {
    print_message(TREE_LOAD_FAILED);
    return;
  }

  // 트리 안의 노드를 삭제하기 위한 스택을 생성합니다.
  node_t *stack[1024];
  int stack_top = 0;

  // 루트 노드가 존재하면 노드 탐색을 시작합니다.
  if (tree->root != tree->nil) {
    stack[stack_top++] = tree->root;
  }

  // 스택에 오른쪽 노드를 먼저 넣어 전위 순회합니다.
  while (stack_top > 0) {
    node_t *node = stack[--stack_top];

    if (node->right != tree->nil) {
      stack[stack_top++] = node->right;
    }

    if (node->left != tree->nil) {
      stack[stack_top++] = node->left;
    }

    // 자식 삽입이 끝난 노드는 메모리 반납(노드 삭제)합니다.
    free(node);
    node = NULL;
  }

  // 트리 내의 모든 노드를 삭제 후 최종적으로 nil노드와 트리를 삭제합니다.
  free(tree->nil);
  free(tree);
}

// 좌회전 함수
void left_rotate(rbtree *tree, node_t *x)
{
  // 중심 노드 또는 오른쪽 자식이 nil이면 좌회전이 불가능합니다.
  if (x == tree->nil || x->right == tree->nil) {
    print_message(LEFT_ROTATE_FAILED);
    return;
  }

  // y를 설정합니다.(좌회전 대기)
  node_t *y = x->right;

  // y를 부모 자리(현 x)로 좌회전합니다.
  y->parent = x->parent;
  if (x == tree->root) { // x가 루트 노드였다면 좌회전된 y노드를 루트 노드로 설정합니다.
    tree->root = y;
  } else if (x == x->parent->left) { // x가 부모의 왼쪽 자식이었다면 좌회전된 y를 원부모의 왼쪽 자식으로 설정합니다.
    x->parent->left = y;
  } else if (x == x->parent->right) { // x가 부모의 오른쪽 자식이었다면 좌회전된 y를 원부모의 오른쪽 자식으로 설정합니다.
    x->parent->right = y;
  }

  // x를 y의 왼쪽 자식으로 좌회전합니다.
  y->left = x;
  x->parent = y;

  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮깁니다.
  x->right = y->left;
  if (y->left != tree->nil) { // nil 노드의 부모는 변경할 수 없습니다.
    y->left->parent = x;
  }
}

// 우회전 함수
void right_rotate(rbtree *tree, node_t *x)
{
  // 중심 노드 또는 왼쪽 자식이 nil이면 우회전이 불가능합니다.
  if (x == tree->nil || x->left == tree->nil) {
    print_message(RIGHT_ROTATE_FAILED);
    return;
  }

  // y를 설정합니다.(우회전 대기)
  node_t *y = x->left;

  // y를 부모 자리(현 x)로 우회전합니다.
  y->parent = x->parent;
  if (x == tree->root) { // x가 루트 노드였다면 우회전된 y노드를 루트 노드로 설정합니다.
    tree->root = y;
  } else if (x == x->parent->left) { // x가 부모의 왼쪽 자식이었다면 우회전된 y를 원부모의 왼쪽 자식으로 설정합니다.
    x->parent->left = y;
  } else if (x == x->parent->right) { // x가 부모의 오른쪽 자식이었다면 우회전된 y를 원부모의 오른쪽 자식으로 설정합니다.
    x->parent->right = y;
  }

  // x를 y의 오른쪽 자식으로 우회전합니다.
  y->right = x;
  x->parent = y;

  // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 옮깁니다.
  x->left = y->right;
  if (y->right != tree->nil) { // nil 노드의 부모는 변경할 수 없습니다.
    y->right->parent = x;
  }
}

void rbtree_insert_fixup(rbtree *tree, node_t *z)
{
  //
}

node_t *rbtree_insert(rbtree *tree, const key_t key)
{
  // 신규 노드를 생성합니다.
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  if (!new_node) {
    print_message(NODE_GENERATE_FAILED);
    return tree->root;
  }

  // 노드의 초기 멤버를 설정합니다.
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->parent = tree->nil;
  new_node->left = tree->nil;
  new_node->right = tree->nil;

  // 신규 노드가 삽입될 위치를 찾습니다.
  node_t *current = tree->root; // 주목 노드를 설정합니다.
  node_t *parent = tree->nil;

  while (current != tree->nil) {
    parent = current;         // 신규 노드의 부모가 될 노드입니다.
    if (key < current->key) { // 주목 노드보다 키가 작으면 왼쪽 서브트리를 탐색합니다.
      current = current->left;
    } else if (key > current->key) { // 주목 노드보다 키가 크면 오른쪽 서브트리를 탐색합니다.
      current = current->right;
    } else if (key == current->key) { // 중복된 값인 경우 오른쪽 서브트리를 탐색합니다.
      current = current->right;
    }
  }

  // while문 종료 시 신규 노드의 부모 노드가 결정되었습니다.
  new_node->parent = parent;
  if (parent == tree->nil) { // 트리가 비어있던 경우 신규 노드를 루트 노드로 설정합니다.
    tree->root = new_node;
  } else if (key < parent->key) { // 부모 노드보다 신규 노드의 값이 작으면 왼쪽 자식으로 설정합니다.
    parent->left = new_node;
  } else if (key > parent->key) { // 부모 노드보다 신규 노드의 값이 크면 오른쪽 자식으로 설정합니다.
    parent->left = new_node;
  } else if (key == parent->key) { // 중복된 값일 경우 오른쪽 자식으로 설정합니다.
    parent->right = new_node;
  }

  // 레드블랙트리의 특성을 복구합니다.
  rbtree_insert_fixup(tree, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  return t->root;
}

node_t *rbtree_min(const rbtree *t)
{

  return t->root;
}

node_t *rbtree_max(const rbtree *t)
{
  return t->root;
}

node_t *rbtree_min_in_subtree(rbtree *t, node_t *node)
{
  return node;
}

node_t *rbtree_max_in_subtree(rbtree *t, node_t *node)
{
  return node;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  //
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  //
}

int rbtree_erase(rbtree *t, node_t *z)
{
  return 0;
}

void inorder_recursion(const node_t *node, key_t *arr, size_t *index, const node_t *nil)
{
  //
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  return 0;
}
