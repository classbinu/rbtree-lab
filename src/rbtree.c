#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

// 예외 메시지 타입을 정의합니다.
typedef enum
{
  MALLOC_FAILED,
  TREE_GENERATE_FAILED,
  NODE_GENERATE_FAILED,
  TREE_LOAD_FAILED,
  NODE_LOAD_FAILED,
  ETC
} message_t;

// 타입에 맞는 예외 메시지를 출력하는 함수입니다.
void print_message(message_t type)
{
  switch (type) {
  case MALLOC_FAILED:
    printf("메모리 할당에 실패하였습니다.\n");
    break;
  case TREE_LOAD_FAILED:
    printf("트리를 불러올 수 없습니다.\n");
    break;
  case NODE_LOAD_FAILED:
    printf("노드를 불러올 수 없습니다.\n");
    break;
  case TREE_GENERATE_FAILED:
    printf("트리 생성에 실패하였습니다.\n");
    break;
  case NODE_GENERATE_FAILED:
    printf("노드 생성에 실패하였습니다.\n");
    break;
  case ETC:
    printf("기타 예외가 발생하였습니다.\n");
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

// 좌회전 함수입니다.
void left_rotate(rbtree *tree, node_t *x)
{
  // 중심 노드 또는 오른쪽 자식이 nil이면 좌회전을 하지 않습니다.
  if (x == tree->nil || x->right == tree->nil) {
    return;
  }

  // y를 설정합니다.(좌회전 대기)
  node_t *y = x->right;

  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮깁니다.
  x->right = y->left;
  if (y->left != tree->nil) { // nil 노드의 부모는 변경할 수 없습니다.
    y->left->parent = x;
  }

  // y를 부모 자리(현 x)로 좌회전합니다.
  y->parent = x->parent;
  if (x->parent == tree->nil) { // x가 루트 노드였다면 좌회전된 y노드를 루트 노드로 설정합니다.
    tree->root = y;
  } else if (x == x->parent->left) { // x가 부모의 왼쪽 자식이었다면 좌회전된 y를 원부모의 왼쪽 자식으로 설정합니다.
    x->parent->left = y;
  } else if (x == x->parent->right) { // x가 부모의 오른쪽 자식이었다면 좌회전된 y를 원부모의 오른쪽 자식으로 설정합니다.
    x->parent->right = y;
  }

  // x를 y의 왼쪽 자식으로 좌회전합니다.
  y->left = x;
  x->parent = y;
}

// 우회전 함수입니다.
void right_rotate(rbtree *tree, node_t *x)
{
  // 중심 노드 또는 왼쪽 자식이 nil이면 우회전이 하지 않습니다.
  if (x == tree->nil || x->left == tree->nil) {
    return;
  }

  // y를 설정합니다.(우회전 대기)
  node_t *y = x->left;

  // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 옮깁니다.
  x->left = y->right;
  if (y->right != tree->nil) { // nil 노드의 부모는 변경할 수 없습니다.
    y->right->parent = x;
  }

  // y를 부모 자리(현 x)로 우회전합니다.
  y->parent = x->parent;
  if (x->parent == tree->nil) { // x가 루트 노드였다면 우회전된 y노드를 루트 노드로 설정합니다.
    tree->root = y;
  } else if (x == x->parent->left) { // x가 부모의 왼쪽 자식이었다면 우회전된 y를 원부모의 왼쪽 자식으로 설정합니다.
    x->parent->left = y;
  } else if (x == x->parent->right) { // x가 부모의 오른쪽 자식이었다면 우회전된 y를 원부모의 오른쪽 자식으로 설정합니다.
    x->parent->right = y;
  }

  // x를 y의 오른쪽 자식으로 우회전합니다.
  y->right = x;
  x->parent = y;
}

// 트리에 노드를 삽입한 후 레드블랙트리의 특성을 복구하는 함수입니다.
void rbtree_insert_fixup(rbtree *tree, node_t *current_node)
{
  // 주목 노드가 레드인 경우에는 트리 재조정을 진행합니다.
  while (current_node != tree->root && current_node->parent->color == RBTREE_RED) {
    // case A. 현재 노드가 왼쪽 서브트리에 속한 경우입니다.(최종적으로 우회전 필요)
    if (current_node->parent == current_node->parent->parent->left) {
      node_t *uncle = current_node->parent->parent->right;
      if (uncle->color == RBTREE_RED) {                    // case1: 삼촌이 레드인 경우
        current_node->parent->color = RBTREE_BLACK;        // 부모 노드를 블랙으로 설정합니다.
        uncle->color = RBTREE_BLACK;                       // 삼촌 노드를 블랙으로 설정합니다.
        current_node->parent->parent->color = RBTREE_RED;  // 조부모를 레드로 설정합니다.(조부모가 부모, 삼촌에게 블랙을 물려줌.)
        current_node = current_node->parent->parent;       // 블랙을 물려준 조부모를 주목 노드로 재설정합니다.
      } else {                                             // case 2 & 3: 삼촌이 블랙인 경우
        if (current_node == current_node->parent->right) { // case2: 삼촌이 블랙이고, 주목 노드가 오른쪽 자식인 경우(노드 꺾임)
          current_node = current_node->parent;             // 부모 노드를 기준으로 설정하고,
          left_rotate(tree, current_node);                 // 좌회전 해서 case3로 전환합니다.
        }
        // case3: 삼촌이 블랙이고, 신규 노드가 왼쪽 자식인 경우(노드 직선)
        current_node->parent->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        right_rotate(tree, current_node->parent->parent); // 조부모를 기준으로 우회전해서 전체 트리의 균형을 맞춥니다.
      }
      // case B. 현재 노드가 오른쪽 서브트리에 속한 경우입니다.(최종적으로 좌회전 필요)
    } else if (current_node->parent == current_node->parent->parent->right) {
      node_t *uncle = current_node->parent->parent->left;
      if (uncle->color == RBTREE_RED) {
        current_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        current_node = current_node->parent->parent;
      } else {
        if (current_node == current_node->parent->left) {
          current_node = current_node->parent;
          right_rotate(tree, current_node);
        }
        current_node->parent->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        left_rotate(tree, current_node->parent->parent);
      }
    }
  }

  // 루트 노드를 항상 블랙으로 설정합니다.
  tree->root->color = RBTREE_BLACK;
}

// 트리에 새로운 노드를 삽입하는 함수입니다.
node_t *rbtree_insert(rbtree *tree, const key_t key)
{
  // 신규 노드를 생성합니다.
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
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
  node_t *parent = tree->nil;   // 초기 부모를 nil노드로 설정합니다.

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
  if (parent == tree->nil) { // 트리가 비어있으면 신규 노드를 루트 노드로 설정합니다.
    tree->root = new_node;
  } else if (key < parent->key) { // 부모 노드보다 신규 노드의 값이 작으면 왼쪽 자식으로 설정합니다.
    parent->left = new_node;
  } else if (key > parent->key) { // 부모 노드보다 신규 노드의 값이 크면 오른쪽 자식으로 설정합니다.
    parent->right = new_node;
  } else if (key == parent->key) { // 중복된 값일 경우 오른쪽 자식으로 설정합니다.
    parent->right = new_node;
  }

  // 레드블랙트리의 특성을 복구합니다.
  rbtree_insert_fixup(tree, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *tree, const key_t key)
{
  node_t *current = tree->root;

  while (current != tree->nil) {
    if (key == current->key) {
      return current;
    }

    if (key < current->key) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return NULL;
}

// 전체 트리에서 최솟값 노드를 반환하는 함수입니다.
node_t *rbtree_min(const rbtree *tree)
{
  if (!tree) {
    return NULL;
  }

  node_t *current_node = tree->root;
  while (current_node != tree->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

// 전체 트리에서 최댓값 노드를 반환하는 함수입니다.
node_t *rbtree_max(const rbtree *tree)
{
  if (!tree) {
    return NULL;
  }

  node_t *current_node = tree->root;
  while (current_node != tree->nil) {
    current_node = current_node->right;
  }
  return current_node;
}

// 특정 노드를 기준으로 최솟값 노드를 반환하는 함수입니다.
node_t *rbtree_min_in_subtree(rbtree *tree, node_t *node)
{
  if (!tree) {
    print_message(TREE_LOAD_FAILED);
    return NULL;
  }

  if (!node) {
    print_message(NODE_LOAD_FAILED);
    return NULL;
  }

  node_t *current_node = node;
  while (current_node != tree->nil) {
    current_node = current_node->left;
  }

  return current_node;
}

// 특정 노드를 기준으로 최댓값 노드를 반환하는 함수입니다.
node_t *rbtree_max_in_subtree(rbtree *tree, node_t *node)
{
  if (!tree) {
    print_message(TREE_LOAD_FAILED);
    return NULL;
  }

  if (!node) {
    print_message(NODE_LOAD_FAILED);
    return NULL;
  }

  node_t *current_node = node;
  while (current_node != tree->nil) {
    current_node = current_node->right;
  }

  return current_node;
}

// 삭제할 노드의 서브트리를 삭제할 부모에 연결하는 함수입니다.(노드 삭제가 진행됩니다.)
void rbtree_transplant(rbtree *tree, node_t *target_node, node_t *replaced_subtree)
{
  if (!tree) {
    print_message(TREE_LOAD_FAILED);
    return;
  }

  if (!target_node || !replaced_subtree) {
    print_message(NODE_LOAD_FAILED);
    return;
  }

  if (target_node == tree->root) { // 삭제할 노드가 루트 노드라면 대체 서브트리의 루트를 전체 트리 루트로 설정합니다.
    tree->root = replaced_subtree;
  } else if (target_node == target_node->parent->left) {  // 삭제할 노드가 왼쪽 자식이라면
    target_node->parent->left = replaced_subtree;         // 대체 서브트리를 왼쪽으로 연결합니다.
  } else if (target_node == target_node->parent->right) { // 삭제할 노드가 오른쪽 자식이라면
    target_node->parent->right = replaced_subtree;        // 대체 서브트리를 오른쪽으로 연결합니다.
  }
}

// 트리에서 노드를 삭제한 후 레드블랙트리의 특성을 복구하는 함수입니다.
void rbtree_erase_fixup(rbtree *tree, node_t *current_node)
{
  node_t *sibling;

  while (current_node != tree->root && current_node->color == RBTREE_BLACK) {
    if (current_node == current_node->parent->left) { // case A. 주목 노드가 왼쪽 자식인 경우
      sibling = current_node->parent->right;

      // case 1. 형제 노드가 레드인 경우
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK; // 좌회전을 대비해서 형제 노드와 부모 노드의 색을 교환합니다.
        current_node->parent->color = RBTREE_RED;
        left_rotate(tree, current_node->parent);
        sibling = current_node->parent->right; // 새로운 형제노드를 설정해서 case 2~4로 변환합니다.
      }

      // case 2. (형제 노드는 블랙) 형제 노드의 두 자식이 모두 블랙인 경우
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        current_node = current_node->parent;                                                    // 만약 주목 노드가 루트 노드이거나, 블랙이면 조정이 완료됩니다.
      } else if (sibling->left->color == RBTREE_RED && sibling->right->color == RBTREE_BLACK) { // case 3. (형제 노드는 블랙) 형제 노드의 왼쪽 자식은 레드, 오른쪽 자식은 블랙인 경우
        sibling->left->color = RBTREE_BLACK;
        sibling->color = RBTREE_RED;
        right_rotate(tree, sibling);
        sibling = current_node->parent->right; // 새로운 형제가 설정되고, case 4로 변환됩니다.
      }

      // case 4. case2의 일부를 제외하고 모든 case는 case4로 귀결됩니다.
      sibling->color = current_node->parent->color;
      current_node->parent->color = RBTREE_BLACK;
      sibling->right->color = RBTREE_BLACK;
      left_rotate(tree, current_node->parent);
      current_node = tree->root;
    } else { // case B. 주목 노드가 오른쪽 자식인 경우
      sibling = current_node->parent->left;

      // case 1. 형제 노드가 레드인 경우
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK; // 좌회전을 대비해서 형제 노드와 부모 노드의 색을 교환합니다.
        current_node->parent->color = RBTREE_RED;
        right_rotate(tree, current_node->parent);
        sibling = current_node->parent->left; // 새로운 형제노드를 설정해서 case 2~4로 변환합니다.
      }

      // case 2. (형제 노드는 블랙) 형제 노드의 두 자식이 모두 블랙인 경우
      if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;
        current_node = current_node->parent;                                                      // 만약 주목 노드가 루트 노드이거나, 블랙이면 조정이 완료됩니다.
      } else if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) { // case 3. (형제 노드는 블랙) 형제 노드의 왼쪽 자식은 레드, 오른쪽 자식은 블랙인 경우
        sibling->left->color = RBTREE_BLACK;
        sibling->color = RBTREE_RED;
        left_rotate(tree, sibling);
        sibling = current_node->parent->left; // 새로운 형제가 설정되고, case 4로 변환됩니다.
      }

      // case 4. case2의 일부를 제외하고 모든 case는 case4로 귀결됩니다.
      sibling->color = current_node->parent->color;
      current_node->parent->color = RBTREE_BLACK;
      sibling->left->color = RBTREE_BLACK;
      right_rotate(tree, current_node->parent);
      current_node = tree->root;
    }
  }
  current_node->color = RBTREE_BLACK;
}

// 트리에서 특정 노드를 삭제하는 함수입니다.
int rbtree_erase(rbtree *tree, node_t *target_node)
{
  if (!tree) {
    print_message(TREE_LOAD_FAILED);
    return -1;
  }

  if (!target_node) {
    print_message(NODE_LOAD_FAILED);
    return -1;
  }

  node_t *successor = target_node;
  node_t *replacement_node;
  color_t successor_original_color = successor->color;

  if (target_node->left == tree->nil) {                          // 삭제할 노드의 왼쪽 자식이 nil이면
    replacement_node = target_node->right;                       // 오른쪽 자식을 대체 노드로 설정합니다.
    rbtree_transplant(tree, target_node, target_node->right);    // 노드를 삭제하고 부모-자식 관계를 재설정합니다.
  } else if (target_node->right == tree->nil) {                  // 삭제할 노드의 오른쪽 자식이 nil이면
    replacement_node = target_node->left;                        // 왼쪽 자식을 대체 노드로 설정합니다.
    rbtree_transplant(tree, target_node, target_node->left);     // 노드를 삭제하고 부모-자식 관계를 재설정합니다.
  } else {                                                       // 삭제할 노드에 모두 자녀가 있으면
    successor = rbtree_min_in_subtree(tree, target_node->right); // 후계자를 선정합니다.
    successor_original_color = successor->color;                 // 후계자의 색상을 저장합니다.
    replacement_node = successor->right;                         // 후계자가 기존 노드를 올라간 후 후계자의 자식을 저장합니다.

    if (successor->parent != target_node) {                 // 후계자가 바로 삭제할 노드를 대체하지 못하는 경우
      rbtree_transplant(tree, successor, replacement_node); // 후계자의 위치를 대체 노드가 차지하게 됩니다.
      successor->right = target_node->right;
      successor->right->parent = successor;
    }

    // 삭제할 노드의 위치에 후계자를 배치합니다.
    rbtree_transplant(tree, target_node, successor);
    successor->left = target_node->left;
    successor->left->parent = successor;
    successor->color = target_node->color;
  }

  free(target_node);
  target_node = NULL;

  if (successor_original_color == RBTREE_BLACK) {
    rbtree_erase_fixup(tree, replacement_node);
  }
  return 1;
}

// 트리를 전위 순회하는 재귀 함수입니다.
void inorder_recursion(const node_t *node, key_t *arr, size_t *index, const node_t *nil)
{
  if (node == nil) {
    return;
  }

  inorder_recursion(node->left, arr, index, nil);
  arr[(*index)++] = node->key;
  inorder_recursion(node->right, arr, index, nil);
}

// 트리를 전위 순회하여 배열에 삽입하는 함수입니다.
int rbtree_to_array(const rbtree *tree, key_t *arr, const size_t n)
{
  if (!tree || !arr) {
    return -1;
  }

  size_t index = 0;
  inorder_recursion(tree->root, arr, &index, tree->nil);

  if (index != n) {
    return -1;
  }

  return 0;
}