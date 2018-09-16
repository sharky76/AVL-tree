#pragma once

#include <stdlib.h>

#define MAX(a,b) (((a) > (b))? (a) : (b))

typedef struct tree_node
{
	struct tree_node* left;
	struct tree_node* right;
	int	height;
	void* value;
} tree_node;

typedef struct tree_handle
{
	tree_node* root;
	void* max_key;
	void(*deleter)(void*);
	int (*compare)(void*, void*);
} tree_handle;

void tree_add_value(tree_handle* handle, void* value);
void tree_remove_value(tree_handle* handle, void* value);
void* tree_find_value(tree_handle* handle, void* value);
void tree_split(tree_handle* handle, void* value, tree_handle** left_handle, tree_handle** right_handle);
tree_handle* tree_join(tree_handle* left_handle, tree_handle* right_handle);

tree_handle* tree_init(void(*deleter)(void*), int(*compare)(void*, void*))
{
	tree_handle* handle = (tree_handle*)malloc(sizeof(tree_handle));

	handle->deleter = deleter;
	handle->compare = compare;
	handle->root = NULL;
	handle->max_key = NULL;

	return handle;
}

inline int tree_height(tree_node* node)
{
	if (NULL == node)
	{
		return 0;
	}

	return node->height;
}

inline void rotateRight(tree_node** node)
{
	tree_node* tmp = *node;
	*node = (*node)->right;
	tmp->right = (*node)->left;
	(*node)->left = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	(*node)->height = 1 + MAX(tree_height((*node)->right), tree_height((*node)->left));
}

inline void rotateLeft(tree_node** node)
{
	tree_node* tmp = *node;
	*node = (*node)->left;
	tmp->left = (*node)->right;
	(*node)->right = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	(*node)->height = 1 + MAX(tree_height((*node)->right), tree_height((*node)->left));
}

inline void rotateLeftRight(tree_node** node)
{
	tree_node* tmp = (*node)->left;
	tree_node* left_right = tmp->right;

	tmp->right = left_right->left;

	(*node)->left = left_right;
	left_right->left = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	left_right->height = 1 + MAX(tree_height(left_right->right), tree_height(left_right->left));
}

inline void rotateRightLeft(tree_node** node)
{
	tree_node* tmp = (*node)->right;
	tree_node* right_left = tmp->left;

	tmp->left = right_left->right;

	(*node)->right = right_left;
	right_left->right = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	right_left->height = 1 + MAX(tree_height(right_left->right), tree_height(right_left->left));
}

void balance_tree(tree_node** node)
{
	if (NULL == (*node))
	{
		return;
	}

	int balance = tree_height((*node)->left) - tree_height((*node)->right);

	if (balance < -1) // Right side bigger, rotate left
	{
		int lower_balance = tree_height((*node)->right->right) - tree_height((*node)->right->left);

		if (lower_balance > 0)		// Right right case
		{
			rotateRight(node);
		}
		else // Right left case
		{
			rotateRightLeft(node);
			rotateRight(node);
		}
	}

	if (balance > 1)
	{
		int lower_balance = tree_height((*node)->left->right) - tree_height((*node)->left->left);

		if (lower_balance > 0)		// Left right case
		{
			rotateLeftRight(node);
			rotateLeft(node);
		}
		else // Left left case
		{
			rotateLeft(node);
		}
	}
}

inline void _add_value(tree_handle* handle, tree_node** root, void* value)
{
	if ((*root) == NULL)
	{
		tree_node* new_node = (tree_node*)malloc(sizeof(tree_node));
		memset(new_node, 0, sizeof(tree_node));
		new_node->value = value;
		new_node->height = 1;

		*root = new_node;
		if (handle->compare(value, handle->max_key) > 0)
		{
			handle->max_key = value;
		}
	}
	else if (handle->compare(value, (*root)->value) < 0)
	{
		_add_value(handle, &(*root)->left, value);
	}
	else if (handle->compare(value, (*root)->value) > 0)
	{
		_add_value(handle, &(*root)->right, value);
	}

	(*root)->height = 1 + MAX(tree_height((*root)->right), tree_height((*root)->left));

	balance_tree(root);
}

void tree_add_value(tree_handle* handle, void* value)
{
	_add_value(handle, &handle->root, value);
}

inline void _delete_value(tree_handle* handle, tree_node** root, void* value)
{
	if (*root == NULL)
	{
		return;
	}
	else if (handle->compare(value, (*root)->value) < 0)
	{
		_delete_value(handle, &(*root)->left, value);
	}
	else if (handle->compare(value, (*root)->value) > 0)
	{
		_delete_value(handle, &(*root)->right, value);
	}
	else
	{
		if ((*root)->left == NULL || (*root)->right == NULL)
		{
			tree_node* temp = (*root)->left ? (*root)->left : (*root)->right;

			if (NULL == temp)
			{
				temp = *root;
				*root = NULL;
				if (handle->max_key == temp->value)
				{
					handle->max_key = NULL;
				}

				handle->deleter(temp->value);
				free(temp);
			}
			else
			{
				// temp is a child of root
				tree_node* victim = *root;
				*root = temp;
				handle->deleter(victim->value);
				free(victim);
			}

		}
		else
		{
			struct tree_node* current = (*root);

			/* loop down to find the leftmost leaf */
			while (NULL != current->left)
			{
				current = current->left;
			}

			// replace root with current...
			current->left = ((*root)->left == current) ? NULL : (*root)->left;
			current->right = (*root)->right;
			tree_node* temp = *root;
			*root = current;
			handle->deleter(temp->value);
			free(temp);
		}
	}

	if (*root == NULL)
	{
		return;
	}

	(*root)->height = 1 + MAX(tree_height((*root)->right), tree_height((*root)->left));
	if (handle->compare((*root)->value, handle->max_key) > 0)
	{
		handle->max_key = (*root)->value;
	}

	// now lets balance new root...
	balance_tree(root);
}

void tree_remove_value(tree_handle* handle, void* value)
{
	_delete_value(handle, &handle->root, value);
}

inline tree_node* _find_value(tree_handle* handle, tree_node* root, void* value)
{
	if (NULL == root)
	{
		return NULL;
	}

	if (handle->compare(root->value, value) > 0)
	{
		return _find_value(handle, root->left, value);
	}
	else if (handle->compare(root->value, value) < 0)
	{
		return _find_value(handle, root->right, value);
	}
	else
	{
		return root;
	}
}

void* tree_find_value(tree_handle* handle, void* value)
{
	return _find_value(handle, handle->root, value);
}

inline void _join_tree_right(tree_handle* handle, tree_node** left_root, tree_node** right_root, tree_node* k)
{
	if (tree_height(*left_root) <= tree_height(*right_root) + 1)
	{
		k->left = *left_root;
		k->right = *right_root;
		*left_root = k;

		k->height = 1 + MAX(tree_height(k->right), tree_height(k->left));
		return;
	}
	else
	{
		_join_tree_right(handle, &(*left_root)->right, right_root, k);
	}

	(*left_root)->height = 1 + MAX(tree_height((*left_root)->right), tree_height((*left_root)->left));
	balance_tree(left_root);

}

inline void _join_tree_left(tree_handle* handle, tree_node** left_root, tree_node** right_root, tree_node* k)
{
	if (tree_height(*left_root) >= tree_height(*right_root) + 1)
	{
		k->left = *left_root;
		k->right = *right_root;
		*right_root = k;

		k->height = 1 + MAX(tree_height(k->right), tree_height(k->left));
		return;
	}
	else
	{
		_join_tree_left(handle, left_root, &(*right_root)->left, k);
	}

	(*right_root)->height = 1 + MAX(tree_height((*right_root)->right), tree_height((*right_root)->left));

	balance_tree(right_root);
}

inline tree_node* _tree_join(tree_handle* handle, tree_node** left_root, tree_node** right_root, tree_node* k)
{
	int balance = 0;
	if (NULL != *left_root && NULL != *right_root)
	{
		balance = tree_height(*left_root) - tree_height(*right_root);
	}

	if (balance > 1)
	{
		// Left tree is bigger, join at right side
		_join_tree_right(handle, left_root, right_root, k);
		return *left_root;
	}
	else if (balance < -1)
	{
		// right tree is bigger, join at left size
		_join_tree_left(handle, left_root, right_root, k);
		balance_tree(right_root);
		return *right_root;
	}
	else
	{
		if (NULL != k)
		{
			k->left = *left_root;
			k->right = *right_root;

			*left_root = k;
		}
		(*left_root)->height = 1 + MAX(tree_height((*left_root)->right), tree_height((*left_root)->left));
		balance_tree(left_root);
		return *left_root;
	}
}

inline tree_node* _remove_left_most_node(tree_node** root)
{
	if (NULL == *root)
	{
		return NULL;
	}
	if ((*root)->left)
	{
		return _remove_left_most_node(&(*root)->left);
	}
	else
	{	
		tree_node* tmp = *root;
		*root = NULL;
		return tmp;
	}
}

tree_handle* tree_join(tree_handle* left_handle, tree_handle* right_handle)
{
	// Find smallest (left-most) node in the right tree...
	tree_node* k = _remove_left_most_node(&right_handle->root);
	

	if (left_handle->compare(left_handle->max_key, k->value) < 0 &&
		right_handle->compare(right_handle->max_key, k->value) > 0)
	{
		int balance = tree_height(left_handle->root) - tree_height(right_handle->root);

		if (balance > 1)
		{
			// Left tree is bigger, join at right side
			_join_tree_right(left_handle, &left_handle->root, &right_handle->root, k);
			left_handle->max_key = right_handle->max_key;
			return left_handle;
		}
		else if (balance < -1)
		{
			// right tree is bigger, join at left size
			_join_tree_left(right_handle, &left_handle->root, &right_handle->root, k);
			return right_handle;
		}
		else
		{
			left_handle->max_key = k->value;
			k->left = left_handle->root;
			k->right = right_handle->root;

			*&left_handle->root = k;
			left_handle->root->height = 1 + MAX(tree_height(left_handle->root->right), tree_height(left_handle->root->left));
			return left_handle;
		}
	}

	return NULL;
}

inline void _tree_split(tree_handle* handle, tree_node* root, tree_node** left_root, tree_node** right_root, void* value)
{
	tree_node* new_node = NULL;
	if (handle->compare(root->value, value) == 0)
	{
		// Append left children to left_root and right children to right_root
		*left_root = (tree_node*)malloc(sizeof(tree_node));
		memset(*left_root, 0, sizeof(tree_node));
		*left_root = root->left;

		*right_root = (tree_node*)malloc(sizeof(tree_node));
		memset(*right_root, 0, sizeof(tree_node));
		*right_root = root->right;
	}
	else if (handle->compare(root->value, value) < 0)
	{
		// path goes to the right
		_tree_split(handle, root->right, left_root, right_root, value);
		*left_root = _tree_join(handle, &root->left, left_root, _remove_left_most_node(left_root));
	}
	else if (handle->compare(root->value, value) > 0)
	{
		// path goes to the left
		_tree_split(handle, root->left, left_root, right_root, value);
		*right_root = _tree_join(handle, right_root, &root->right, _remove_left_most_node(&root->right));
	}
}

void tree_split(tree_handle* handle, void* value, tree_handle** left_handle, tree_handle** right_handle)
{
	if (NULL != tree_find_value(handle, value))
	{
		//*right_handle = tree_init(handle->deleter, handle->compare);
		//_tree_join(_tree_split(handle, &handle->root, &(*right_handle)->root, value);

		tree_node* right_root = NULL;
		tree_node* left_root = NULL;

		_tree_split(handle, handle->root, &left_root, &right_root, value);

		*left_handle = (tree_handle*)malloc(sizeof(tree_handle));
		(*left_handle)->root = left_root;
		(*left_handle)->max_key = NULL;
		(*left_handle)->deleter = handle->deleter;
		(*left_handle)->compare = handle->compare;

		*right_handle = (tree_handle*)malloc(sizeof(tree_handle));
		(*right_handle)->root = right_root;
		(*right_handle)->max_key = NULL;
		(*right_handle)->deleter = handle->deleter;
		(*right_handle)->compare = handle->compare;
	}
}