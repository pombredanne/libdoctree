#include "common.h"
#include "libdoctree.h"

void*
DTalloc(void *old, size_t size)
{
	void *new = NULL;

	if (NULL == old)
		new = calloc(1, size);
	else
		new = realloc(old, size);

	return new;
}

/*
 * Allocate a new node. Returns NULL if out of memory.
 */
DTnode *
DTnewNode(DTnode *parent, const DTchar *label, int flags)
{
	DTnode	*node = NULL;
	int		appendResult = 0;

	// allocate
	node = DTalloc(NULL, sizeof(DTnode));
	if (NULL == node)
		return NULL;

	node->flags = flags;

	// duplicate and assign label
	node->label = DTstrdup(label);
	if (NULL == node->label) {
		free(node);
		return NULL;
	}

	// assign parent
	if (NULL != parent) {
		appendResult = DTappendNode(parent, node);
		if (0 == appendResult) {
			free(node);
			return NULL;
		}
	}

	return node;
}

/*
 * Append a node to another node.
 *
 * Returns the number of child nodes owned by the parent after the append
 * operation is complete or -1 on error.
 */
int
DTappendNode(DTnode *parent, DTnode *child)
{
	int childCount = 0;

	// check for NULLs
	if (NULL == parent || NULL == child)
		return -1;

	// count children
	childCount = DTchildCount(parent);

	// realloc child array with an extra slot for the new child and NULL
	// terminator
	parent->children = DTalloc(parent->children, sizeof(DTnode*) * (childCount + 2));
	if (NULL == parent->children)
		return -1;

	// assign new child
	parent->children[childCount] = child;
	parent->children[childCount + 1] = NULL;
	child->parent = parent;

	return childCount + 1;
}

/*
 * Return the number of children owned by a node.
 */
int
DTchildCount(DTnode *node)
{
	int		n = 0;
	DTnode	**child = NULL;

	for (child = node->children; child && *child; child++)
		n++;

	return n;
}

/*
 * Free a node and all of its descendants.
 */
void
DTfreeNode(DTnode *node)
{
	DTnode		**child = NULL;
	DTattribute	**att = NULL;

	if (NULL == node)
		return;

	for (att = node->attributes; att && *att; att++)
		DTfreeAtt(*att);
	
	for (child = node->children; child && *child; child++)
		DTfreeNode(*child);
		
	DTfree(node->label);	
	DTfree(node);
}
