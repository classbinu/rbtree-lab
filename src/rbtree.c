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
  } else if (x == x->parent->left) { // x가 왼쪽 자식 노드였다면 승격된 y를 기존 부모의 왼쪽 자식으로 설정
    x->parent->left = y;
  } else { // // x가 오른쪽 자식 노드였다면 승격된 y를 기존 부모의 오른쪽 자식으로 설정
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
  } else if (x == x->parent->left) { // x가 왼쪽 자식 노드였다면 승격된 y를 기존 부모의 왼쪽 자식으로 설정
    x->parent->left = y;
  } else { // x가 오른쪽 자식 노드였다면 승격된 y를 기존 부모의 오른쪽 자식으로 설정
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
            RBTREE_RED;        // 조부모를 레드로 설정(부모와 삼촌에게 블랙을 물려줌)
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
  } else if (key < parent->key) { // 신규 노드의 키가 부모의 키보다 작으면 부모의 왼쪽 자식으로 설정
    parent->left = new_node;
  } else if (key > parent->key) {
    parent->right = new_node; // 신규 노드의 키가 부모의 키보다 작으면 부모의 오른쪽 자식으로 설정
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

  while (current != t->nil) {  // 현재 노드가 nil이 아니면 계속 검색
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

node_t *rbtree_min_in_subtree(rbtree *t, node_t *node)
{
  if (node == NULL) {
    return NULL;
  }

  while (node->left != t->nil) {
    node = node->left;
  }

  return node;
}

node_t *rbtree_max_in_subtree(rbtree *t, node_t *node)
{
  if (node == NULL) {
    return NULL;
  }

  while (node->right != t->nil) {
    node = node->right;
  }

  return node;
}

// 삭제된 노드의 서브트리를 삭제된 노드의 부모와 연결한다.
// u는 삭제할 노드, v는 삭제할 노드의 서브트리
void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// rbtree 속성을 복구한다.
void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  node_t *w;
  {
    while (x != t->root && x->color == RBTREE_BLACK) {
      if (x == x->parent->left) { // x가 왼쪽 노드이면 오른쪽 노드를 삼촌으로 설정
        w = x->parent->right;
        if (w->color == RBTREE_RED) { // 케이스1. 형제 w가 적색인 경우
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          left_rotate(t, x->parent);
          w = x->parent->right; // 삼촌을 재설정해서 케이스 2,3,4로 변환
        }
        if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // 케이스2
          w->color = RBTREE_RED;
          x = x->parent; // 이 시점에서 x가 블랙이면서 루트가 되면 루프가 종료
        } else if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w); // 이 시점에서 케이스4로 변환
        }
        // 케이스 4. 케이스2의 일부 케이스를 제외하고 궁극적으로 모든 케이스는 4로 귀결됨.
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      } else { // x가 오른쪽 노드이면 왼쪽 노드를 삼촌으로 설정
        w = x->parent->left;
        if (w->color == RBTREE_RED) { // 케이스1. 형제 w가 적색인 경우
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          right_rotate(t, x->parent);
          w = x->parent->left; // 삼촌을 재설정해서 케이스 2,3,4로 변환
        }
        if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { // 케이스2
          w->color = RBTREE_RED;
          x = x->parent; // 이 시점에서 x가 블랙이면서 루트가 되면 루프가 종료
        } else if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w); // 이 시점에서 케이스4로 변환
        }
        // 케이스 4. 케이스2의 일부 케이스를 제외하고 궁극적으로 모든 케이스는 4로 귀결됨.
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
      x->color = RBTREE_BLACK;
    }
  }
}

// 이진 검색 트리 방식으로 삭제
int rbtree_erase(rbtree *t, node_t *z)
{
  if (z == NULL || z == t->nil) {
    return -1;
  }

  node_t *y = z;
  node_t *x; // x는 삭제 연산으로 인해 부모 노드를 잃게 된 고아 노드
  color_t y_original_color = y->color;

  if (z->left == t->nil) { // 삭제할 노드의 왼쪽 자녀가 nil
    x = z->right;          // 삭제 과정에서 새롭게 올라온 노드를 기준으로 트리를 재조정함.
    rbtree_transplant(t, z, z->right);
  } else if (z->right == t->nil) { // 삭제할 노드의 오른쪽 자녀가 nil
    x = z->left;                   // 삭제 과정에서 새롭게 올라온 노드를 기준으로 트리를 재조정함.
    rbtree_transplant(t, z, z->left);
  } else {                                  // 삭제한 노드에 모두 자녀가 있음
    y = rbtree_min_in_subtree(t, z->right); // 후계자 찾기
    y_original_color = y->color;
    x = y->right;         // y의 왼쪽 자식은 무조건 nil이지만 오른쪽은 서브트리 존재 가능함
    if (y->parent == z) { // 후계자가 삭제 노드의 직접 자식이라면, y는 이미 올바른 위치(y는 언제든지 떠날 준비가 되어 있음.)
      //
    } else {                             // y가 z를 대체하기 위해서는 y의 관계를 y의 오른쪽 자식에게 물려주고 떠나야 함.
      rbtree_transplant(t, y, y->right); // y를 y의 오른쪽 자식으로 대체
      y->right = z->right;
      y->right->parent = y;
    }

    // z를 삭제(y로 대체)하고 기존 z의 왼쪽 자식의 관계를 y의 관계로 재설정
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  free(z);
  z = NULL;

  if (y_original_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);
  }
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
