#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

void print_malloc_failed() { printf("메모리 할당에 실패하였습니다.\n"); }

rbtree *new_rbtree(void)
{
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  if (!t) {
    print_malloc_failed();
    return NULL;
  }

  // T.nil을 생성합니다. 생성된 T.nil은 트리 내에서 계속 사용됩니다.
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  if (!nil_node) {
    print_malloc_failed();
    free(t);
    return NULL;
  }

  // T.nil의 멤버를 설정합니다.
  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node;
  nil_node->left = nil_node;
  nil_node->right = nil_node;

  // 트리의 멤버를 설정합니다.
  t->root = nil_node;
  t->nil = nil_node;

  return t;
}

void delete_rbtree(rbtree *t)
{

  if (!t)
    return;

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
    // T.nil이 아니면 오른쪽 노드를 스택에 push합니다.
    if (node->right != t->nil) {
      stack[stack_top] = node->right;
      stack_top++;
    }

    // T.nil이 아니면 왼쪽 노드를 스택에 push합니다.
    if (node->left != t->nil) {
      stack[stack_top] = node->left;
      stack_top++;
    }

    // 꺼낸 노드를 해제합니다.
    free(node);
    node = NULL;
  }

  // 모든 노드를 해제 후에 T.nil을 해제합니다.
  free(t->nil);

  // 트리를 해제합니다.
  free(t);
}

// 좌회전 함수
void left_rotate(rbtree *t, node_t *x)
{
  if (x == t->nil || x->right == t->nil) {
    return;
  }

  // y를 설정
  node_t *y = x->right;

  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮긴다.
  x->right = y->left;
  y->left->parent = x;

  // y의 부모를 x의 부모로 변경한다.(y를 부모 자리로 승격)
  y->parent = x->parent;
  if (x->parent == t->nil) { // x가 루트였다면 승격된 y를 트리의 루트로 설정
    t->root = y;
  } else if (x == x->parent->left) { // x가 왼쪽 자식 노드였다면 승격된 y를 기존
                                     // 부모의 왼쪽 자식으로 설정
    x->parent->left = y;
  } else { // // x가 오른쪽 자식 노드였다면 승격된 y를 기존 부모의 오른쪽
           // 자식으로
    // 설정
    x->parent->right = y;
  }

  // 승격된 y와 강등된 x의 관계를 설정
  y->left = x;
  x->parent = y;
}

// 우회전 함수
void right_rotate(rbtree *t, node_t *x)
{
  if (x == t->nil || x->left == t->nil) {
    return;
  }

  // y를 설정
  node_t *y = x->left;

  // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 옮긴다.
  x->left = y->right;
  y->right->parent = x;

  // y의 부모를 x의 부모로 변경한다.(y를 부모 자리로 승격)
  y->parent = x->parent;
  if (x->parent == t->nil) { // x가 루트였다면 승격된 y를 트리의 루트로 설정
    t->root = y;
  } else if (x == x->parent->left) { // x가 왼쪽 자식 노드였다면 승격된 y를 기존
                                     // 부모의 왼쪽 자식으로 설정
    x->parent->left = y;
  } else { // // x가 오른쪽 자식 노드였다면 승격된 y를 기존 부모의 오른쪽
           // 자식으로
    // 설정
    x->parent->right = y;
  }

  // 승격된 y와 강등된 x의 관계를 설정
  y->left = x;
  x->parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  // 신규 노드가 루트면 끝 && 신규 노드의 부모 레드면 계속 체크
  while (z != t->root && z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) { // 부모가 왼쪽 자식이라면
      node_t *y = z->parent->parent->right;     // 삼촌 노드 설정
      if (y->color == RBTREE_RED) {
        // 케이스1: z의 삼촌 y가 레드
        z->parent->color = RBTREE_BLACK; // 부모 노드를 블랙으로 설정
        y->color = RBTREE_BLACK;         // 삼촌 노드를 블랙으로 설정
        z->parent->parent->color =
            RBTREE_RED; // 조부모를 레드로 설정(부모와 삼촌에게 블랙을 물려줌)
        z = z->parent->parent; // 레드로 설정된 조부모를 신규 삽입 노드로 취급
      } else {
        if (z == z->parent->right) {
          // 케이스2: z가 오른쪽 자식인 경우(2는 궁극적으로 케이스 3이 됨)
          z = z->parent; // 회전 기준을 z의 부모로 설정
          left_rotate(t, z);
        }
        // 케이스 3: z가 왼쪽 자식인 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent); // 조부모를 기준으로 회전
      }
    } else { // 부모가 오른쪽 자식이라면(좌회전과 반대 코드)
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *new_node = (node_t *)(malloc(sizeof(node_t)));
  if (new_node == NULL) {
    print_malloc_failed();
    return t->root;
  }

  new_node->key = key;
  new_node->color = RBTREE_RED;
  new_node->left = t->nil;
  new_node->right = t->nil;

  node_t *current = t->root;
  node_t *parent = t->nil;

  // BST에 따라 루트부터 신규 노드가 삽입될 위치를 찾습니다.
  while (current != t->nil) {
    parent = current;
    if (key < current->key) {
      current = current->left;
    } else if (key > current->key) {
      current = current->right;
    } else {
      // 키가 중복되었을 경우
      current = current->right;
    }
  }

  // 신규 노드의 부모를 설정
  new_node->parent = current;

  if (parent == t->nil) { // 부모가 nil이면 신규 노드를 트리의 루트로 설정
    t->root = new_node;
  } else if (key < parent->key) { // 신규 노드의 키가 부모의 키보다 작으면
                                  // 부모의 왼쪽 자식으로 설정
    parent->left = new_node;
  } else if (key > parent->key) {
    parent->right = new_node; // 신규 노드의 키가 부모의 키보다 작으면
                              // 부모의 오른쪽 자식으로 설정
  } else {
    parent->right = new_node; // 중복 키라면 부모의 오른쪽 자식으로 설정
  }

  // RB Tree 특성 복구
  rbtree_insert_fixup(t, new_node);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;

  while (current != t->nil) { // 현재 노드가 nil이 아니면 계속 검색
    if (key == current->key) { // 찾았음
      return current;
    }

    if (key < current->key) {
      current = current->left; // 좌측 탐색
    } else {
      current = current->right; // 우측 탐색
    }
  }
  // printf("일치하는 키가 없습니다.\n");
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *current = t->root;
  while (current->left != t->nil) {
    current = current->left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *current = t->root;
  while (current->right != t->nil) {
    current = current->right;
  }
  return current;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  return 0;
}

void inorder_recursion(const node_t *node, key_t *arr, size_t *index,
                       const node_t *nil)
{
  if (node == nil)
    return;

  inorder_recursion(node->left, arr, index, nil);
  arr[(*index)] = node->key;
  (*index)++;
  inorder_recursion(node->right, arr, index, nil);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  if (t == NULL || arr == NULL)
    return -1;

  size_t index = 0;
  inorder_recursion(t->root, arr, &index, t->nil);

  if (index != n) {
    return -1;
  }

  return 0;
}
