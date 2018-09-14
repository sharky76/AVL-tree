#pragma once

#include <stdlib.h>

#define MAX(a,b) (((a) > (b))? (a) : (b))

typedef struct tree_node
{
	struct tree_node* left;
	struct tree_node* right;
	int	height;
	int data;
} tree_node;

int tree_height(tree_node* node)
{
	if (NULL == node)
	{
		return 0;
	}

	return node->height;
}

void rotateRight(tree_node** node)
{
	tree_node* tmp = *node;
	*node = (*node)->right;
	tmp->right = (*node)->left;
	(*node)->left = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	(*node)->height = 1 + MAX(tree_height((*node)->right), tree_height((*node)->left));
}

void rotateLeft(tree_node** node)
{
	tree_node* tmp = *node;
	*node = (*node)->left;
	tmp->left = (*node)->right;
	(*node)->right = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	(*node)->height = 1 + MAX(tree_height((*node)->right), tree_height((*node)->left));
}

void rotateLeftRight(tree_node** node)
{
	tree_node* tmp = (*node)->left;
	tree_node* left_right = tmp->right;

	tmp->right = left_right->left;

	(*node)->left = left_right;
	left_right->left = tmp;

	tmp->height = 1 + MAX(tree_height(tmp->right), tree_height(tmp->left));
	left_right->height = 1 + MAX(tree_height(left_right->right), tree_height(left_right->left));
}

void rotateRightLeft(tree_node** node)
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

void add_value(int value, tree_node** root)
{
	if (*root == NULL)
	{
		tree_node* new_node = (tree_node*)malloc(sizeof(tree_node));
		memset(new_node, 0, sizeof(tree_node));
		new_node->data = value;
		new_node->height = 1;

		*root = new_node;
	}
	else if (value < (*root)->data)
	{
		add_value(value, &(*root)->left);
	}
	else if (value >(*root)->data)
	{
		add_value(value, &(*root)->right);
	}

	(*root)->height = 1 + MAX(tree_height((*root)->right), tree_height((*root)->left));

	balance_tree(root);
}

void remove_value(int value, tree_node** root)
{
	if (*root == NULL)
	{
		return;
	}
	else if (value < (*root)->data)
	{
		//(*root)->height++;
		remove_value(value, &(*root)->left);
	}
	else if (value >(*root)->data)
	{
		//(*root)->height++;
		remove_value(value, &(*root)->right);
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
				//root = NULL;
				free(temp);
			}
			else
			{
				// temp is a child of root
				tree_node* victim = *root;
				*root = temp;
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
			free(temp);
		}
	}

	if (*root == NULL)
	{
		return;
	}

	(*root)->height = 1 + MAX(tree_height((*root)->right), tree_height((*root)->left));
	// now lets balance new root...
	balance_tree(root);
}

tree_node* find_value(int value, tree_node* root)
{
	if (NULL == root)
	{
		return NULL;
	}

	if (root->data == value)
	{
		return root;
	}

	if (root->data > value)
	{
		return find_value(value, root->left);
	}
	else
	{
		return find_value(value, root->right);
	}
}