#include <stdlib.h>
#include "MessageTree.h"

MessageTree::MessageTree()
{
    root = NULL;
    count = 0;
}


MessageTree::~MessageTree()
{
    Release();
}

MessageTreeNode * MessageTree::Insert(MessageTreeNode * data)
{
    int r = 0;
    /* We start at the root of the tree */
    MessageTreeNode	*node = root;
    MessageTreeNode	*parent = NULL;

    /* Lets find the new parent... */
    while (node != NULL) {
        /* Compare two keys, do we have a duplicate? */
        if ((r = MessageTreeNode::Compare(data, node)) == 0) {
            return NULL; /* special case, the node already exists */
        }

        parent = node;

        if (r < 0) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    /* Initialize the new node */
    data->parent = parent;
    data->left = data->right = NULL;
    data->color = MessageNodeRed;
    count++;

    /* Insert it into the tree... */
    if (parent != NULL) {
        if (r < 0) {
            parent->left = data;
        }
        else {
            parent->right = data;
        }
    }
    else {
        root = data;
    }

    /* Fix up the red-black properties... */
    InsertFixup(data);

    return data;
}

MessageTreeNode * MessageTree::Remove(const MessageTreeNode * key)
{
    MessageTreeNode *to_delete;
    MessageTreeNode *child;

    if ((to_delete = Search(key)) == NULL)
        return NULL;

    count--;

    /* make sure we have at most one non-leaf child */
    if (to_delete->left != NULL && to_delete->right != NULL)
    {
        /* swap with smallest from right subtree (or largest from left) */
        MessageTreeNode *smright = to_delete->right;
        while (smright->left != NULL)
            smright = smright->left;
        /* swap the smright and to_delete elements in the tree,
        * but the MessageTreeNode is first part of user data struct
        * so cannot just swap the keys and data pointers. Instead
        * readjust the pointers left,right,parent */

        /* swap colors - colors are tied to the position in the tree */
        SwapColors(&to_delete->color, &smright->color);

        /* swap child pointers in parents of smright/to_delete */
        ChangeParentPtr(to_delete->parent, to_delete, smright);
        if (to_delete->right != smright)
            ChangeParentPtr(smright->parent, smright, to_delete);

        /* swap parent pointers in children of smright/to_delete */
        ChangeChildPtr(smright->left, smright, to_delete);
        ChangeChildPtr(smright->left, smright, to_delete);
        ChangeChildPtr(smright->right, smright, to_delete);
        ChangeChildPtr(smright->right, smright, to_delete);
        ChangeChildPtr(to_delete->left, to_delete, smright);
        if (to_delete->right != smright)
            ChangeChildPtr(to_delete->right, to_delete, smright);
        if (to_delete->right == smright)
        {
            /* set up so after swap they work */
            to_delete->right = to_delete;
            smright->parent = smright;
        }

        /* swap pointers in to_delete/smright nodes */
        SwapMessagePtr(&to_delete->parent, &smright->parent);
        SwapMessagePtr(&to_delete->left, &smright->left);
        SwapMessagePtr(&to_delete->right, &smright->right);

        /* now delete to_delete (which is at the location where the smright previously was) */
    }

    if (to_delete->left != NULL)
        child = to_delete->left;
    else child = to_delete->right;

    /* unlink to_delete from the tree, replace to_delete with child */
    ChangeParentPtr(to_delete->parent, to_delete, child);
    ChangeChildPtr(child, to_delete, to_delete->parent);

    if (to_delete->color == MessageNodeRed)
    {
        /* if node is red then the child (black) can be swapped in */
    }
    else if (child != NULL && child->color == MessageNodeRed)
    {
        /* change child to BLACK, removing a RED node is no problem */
        child->color = MessageNodeBlack;
    }
    else if (child != NULL || to_delete->parent != NULL)
    {
        DeleteFixup(child, to_delete->parent);
    }

    /* unlink completely */
    to_delete->parent = NULL;
    to_delete->left = NULL;
    to_delete->right = NULL;
    to_delete->color = MessageNodeBlack;

    return to_delete;
}

MessageTreeNode * MessageTree::Search(const MessageTreeNode * key)
{
    MessageTreeNode *node;

    if (FindLessEqual(key, &node)) {
        return node;
    }
    else {
        return NULL;
    }
}

#if 0
void MessageTree::InsertFixup(MessageTreeNode * node)
{
    while (node->parent != NULL && node->parent->color == MessageNodeRed)
    {
        /* it can be assumed that the node has a grandparent, 
         * because its parent is red, and if it were the root, 
         * it would be black
         */
        MessageTreeNode *uncle = node->Uncle();
        MessageTreeNode *grandparent = node->Grandparent();

        if ((uncle != NULL) && (uncle->color == MessageNodeRed)) {
            node->parent->color = MessageNodeBlack;
            uncle->color = MessageNodeBlack;
            grandparent->color = MessageNodeRed;
            node = grandparent; /* color checks continue at grandparent */
        } else {
            if ((node == node->parent->right) && (node->parent == grandparent->left)) {
                /*
                * rotate_left can be the below because of already having *g =  grandparent(n)
                *
                * struct node *saved_p=g->left, *saved_left_n=n->left;
                * g->left=n;
                * n->left=saved_p;
                * saved_p->right=saved_left_n;
                *
                * and modify the parent's nodes properly
                */
                RotateLeft(node->parent);
                node = node->left;
            }
            else if ((node == node->parent->left) && (node->parent == grandparent->right)) {
                /*
                * rotate_right can be the below to take advantage of already having *g =  grandparent(n)
                *
                * struct node *saved_p=g->right, *saved_right_n=n->right;
                * g->right=n;
                * n->right=saved_p;
                * saved_p->left=saved_right_n;
                *
                */
                RotateRight(node->parent);
                node = node->right;
            }
            /*
            insert_case5(n);
            {
            struct node *g = grandparent(n);

            n->parent->color = BLACK;
            g->color = RED;
            if (n == n->parent->left)
            rotate_right(g);
            else
            rotate_left(g);
            }
            */
        }
    }

    if (node->parent == NULL)
    {
        /* the node was inserted at the root ! */
        node->color = MessageNodeBlack;
    }
}
#else
void MessageTree::InsertFixup(MessageTreeNode * node)
{
    MessageTreeNode	*uncle;

    /* While not at the root and need fixing... */
    while (node != root && node->parent != NULL && node->parent->color == MessageNodeRed) {
        /* If our parent is left child of our grandparent... */
        if (node->parent->parent != NULL && node->parent == node->parent->parent->left) {
            uncle = node->parent->parent->right;

            /* If our uncle is red... */
            if (uncle != NULL && uncle->color == MessageNodeRed) {
                /* Paint the parent and the uncle black... */
                node->parent->color = MessageNodeBlack;
                uncle->color = MessageNodeBlack;

                /* And the grandparent red... */
                node->parent->parent->color = MessageNodeRed;

                /* And continue fixing the grandparent */
                node = node->parent->parent;
            }
            else {				/* Our uncle is black... */
                                /* Are we the right child? */
                if (node == node->parent->right) {
                    node = node->parent;
                    RotateLeft(node);
                }
                /* Now we're the left child, repaint and rotate... */
                node->parent->color = MessageNodeBlack;
                node->parent->parent->color = MessageNodeRed;
                RotateRight(node->parent->parent);
            }
        }
        else {
            if (node->parent != NULL && node->parent->parent != NULL)
            {
                uncle = node->parent->parent->left;
            }
            else
            {
                uncle = NULL;
            }

            /* If our uncle is red... */
            if (uncle != NULL && uncle->color == MessageNodeRed) {
                /* Paint the parent and the uncle black... */
                node->parent->color = MessageNodeBlack;
                uncle->color = MessageNodeBlack;

                /* And the grandparent red... */
                node->parent->parent->color = MessageNodeRed;

                /* And continue fixing the grandparent */
                node = node->parent->parent;
            }
            else {				/* Our uncle is black... */
                                /* Are we the right child? */
                if (node == node->parent->left) {
                    node = node->parent;
                    RotateRight(node);
                }
                /* Now we're the right child, repaint and rotate... */
                node->parent->color = MessageNodeBlack;

                if (node->parent->parent != NULL)
                {
                    node->parent->parent->color = MessageNodeRed;
                    RotateLeft(node->parent->parent);
                }
            }
        }
    }
    root->color = MessageNodeBlack;
}
#endif

void MessageTree::RotateRight(MessageTreeNode * node)
{
    MessageTreeNode *left = node->left;
    node->left = left->right;
    if (left->right != NULL)
        left->right->parent = node;

    left->parent = node->parent;

    if (node->parent != NULL) {
        if (node == node->parent->right) {
            node->parent->right = left;
        }
        else {
            node->parent->left = left;
        }
    }
    else {
        root = left;
    }
    left->right = node;
    node->parent = left;
}


void MessageTree::RotateLeft(MessageTreeNode * node)
{
    MessageTreeNode *right = node->right;
    node->right = right->left;
    if (right->left != NULL)
        right->left->parent = node;

    right->parent = node->parent;

    if (node->parent != NULL) {
        if (node == node->parent->left) {
            node->parent->left = right;
        }
        else {
            node->parent->right = right;
        }
    }
    else {
        root = right;
    }
    right->left = node;
    node->parent = right;
}

void MessageTree::SwapColors(MessageNodeColor * x, MessageNodeColor * y)
{
    MessageNodeColor c = *x;
    *x = *y;
    *y = c;
}

void MessageTree::SwapMessagePtr(MessageTreeNode ** x, MessageTreeNode ** y)
{
    MessageTreeNode * p = *x;
    *x = *y;
    *y = p;
}

void MessageTree::ChangeParentPtr(MessageTreeNode * parent, 
    MessageTreeNode * oldParent, MessageTreeNode * newParent)
{
    if (parent == NULL)
    {
        if (root == oldParent) 
            root = newParent;
    }
    else
    {
        if (parent->left == oldParent)
        {
            parent->left = newParent;
        }

        if (parent->right == oldParent)
        {
            parent->right = newParent;
        }
    }
}

void MessageTree::ChangeChildPtr(MessageTreeNode * child, MessageTreeNode * oldChild, MessageTreeNode * newChild)
{
    if (child == NULL) return;

    if (child->parent == oldChild) 
        child->parent = newChild;

}

void MessageTree::DeleteFixup(MessageTreeNode * child, MessageTreeNode * child_parent)
{
    MessageTreeNode* sibling;
    int go_up = 1;

    /* determine sibling to the node that is one-black short */
    if (child_parent->right == child)
    {
        sibling = child_parent->left;
    }
    else
    {
        sibling = child_parent->right;
    }

    while (go_up)
    {
        if (child_parent == NULL)
        {
            /* removed parent==black from root, every path, so ok */
            return;
        }

        if (sibling != NULL && sibling->color == MessageNodeRed)
        {	/* rotate to get a black sibling */
            child_parent->color = MessageNodeRed;
            sibling->color = MessageNodeBlack;
            if (child_parent->right == child)
            {
                RotateRight(child_parent);
            }
            else
            {
                RotateLeft(child_parent);
            }
            /* new sibling after rotation */
            if (child_parent->right == child)
            {
                sibling = child_parent->left;
            } 
            else 
            {
                sibling = child_parent->right;
            }
        }

        if (child_parent->color == MessageNodeBlack
            && (sibling == NULL ||
            (sibling->color == MessageNodeBlack
                && (sibling->left == NULL || sibling->left->color == MessageNodeBlack)
                && (sibling->right == NULL || sibling->right->color == MessageNodeBlack))))
        {	/* fixup local with recolor of sibling */
            if (sibling != NULL)
            {
                sibling->color = MessageNodeRed;
            }

            child = child_parent;

            child_parent = child_parent->parent;

            if (child_parent != NULL)
            {
                /* prepare to go up, new sibling */
                if (child_parent->right == child)
                {
                    sibling = child_parent->left;
                }
                else
                {
                    sibling = child_parent->right;
                }
            }
            else
            {
                sibling = NULL;
            }
        }
        else go_up = 0;
    }

    if (child_parent->color == MessageNodeRed
        && (sibling == NULL || (sibling->color == MessageNodeBlack 
        && (sibling->left == NULL || sibling->left->color == MessageNodeBlack)
        && (sibling->right == NULL || sibling->right->color == MessageNodeBlack))))
    {
        /* move red to sibling to rebalance */
        if (sibling != NULL)
        {
            sibling->color = MessageNodeRed;
        }
        child_parent->color = MessageNodeBlack;
        return;
    }

    /* get a new sibling, by rotating at sibling. See which child
    of sibling is red */
    if (child_parent->right == child
        && sibling != NULL
        && sibling->color == MessageNodeBlack
        && sibling->right != NULL
        && sibling->right->color == MessageNodeRed
        && (sibling->left == NULL || sibling->left->color == MessageNodeBlack))
    {
        sibling->color = MessageNodeRed;
        sibling->right->color = MessageNodeBlack;
        RotateLeft(sibling);
        /* new sibling after rotation */
        if (child_parent->right == child)
        {
            sibling = child_parent->left;
        }
        else
        {
            sibling = child_parent->right;
        }
    }
    else if (child_parent->left == child
        && sibling != NULL
        && sibling->color == MessageNodeBlack
        && sibling->left != NULL
        && sibling->left->color == MessageNodeRed
        && (sibling->right == NULL || sibling->right->color == MessageNodeBlack))
    {
        sibling->color = MessageNodeRed;
        sibling->left->color = MessageNodeBlack;
        RotateRight(sibling);
        /* new sibling after rotation */
        if (child_parent->right == child)
        {
            sibling = child_parent->left;
        }
        else
        {
            sibling = child_parent->right;
        }
    }

    /* now we have a black sibling with a red child. rotate and exchange colors. */
    if (sibling != NULL)
    {
        sibling->color = child_parent->color;
        child_parent->color = MessageNodeBlack;
        if (child_parent->right == child)
        {
            if (sibling->left != NULL)
            {
                sibling->left->color = MessageNodeBlack;
            }
            RotateRight(child_parent);
        }
        else
        {
            if (sibling->right != NULL)
            {
                sibling->right->color = MessageNodeBlack;
            }
            RotateLeft(child_parent);
        }
    }
}

bool MessageTree::FindLessEqual(const MessageTreeNode * key, MessageTreeNode ** result)
{
    bool ret = false;
    int r = 0;
    MessageTreeNode *node;

    /* We start at root... */
    node = root;

    *result = NULL;

    /* While there are children... */
    while (node != NULL) {
        r = MessageTreeNode::Compare(key, node);
        if (r == 0) {
            /* Exact match */
            *result = node;
            ret = true;
            break;
        }
        if (r < 0) {
            node = node->left;
        }
        else {
            /* Temporary match */
            *result = node;
            node = node->right;
        }
    }

    return ret;
}

MessageTreeNode * MessageTree::First()
{
    MessageTreeNode *node;

    for (node = root; node != NULL && node->left != NULL; node = node->left);

    return node;
}

MessageTreeNode * MessageTree::Last()
{
    MessageTreeNode *node;

    for (node = root; node != NULL && node->right != NULL; node = node->right);

    return node;
}

void MessageTree::Release()
{
    if (root != NULL)
    {
        delete root;
        root = NULL;
    }

    count = 0;
}

const MessageTreeNode * MessageTree::Root() {
    return (const MessageTreeNode *)root;
}

const int MessageTree::Count() {
    return count;
}

MessageTreeNode::MessageTreeNode()
{
    parent = NULL;
    left = NULL;
    right = NULL;
    color = MessageNodeRed;
    target = NULL;
    message = NULL;
    scheduled_time = 0;
    insertion_rank = 0;
}

MessageTreeNode::MessageTreeNode(
    ISimObject * t, ISimMessage * m, unsigned long long s_time, unsigned long long rank)

{
    parent = NULL;
    left = NULL;
    right = NULL;
    color = MessageNodeRed;
    target = t;
    message = m;
    scheduled_time = s_time;
    insertion_rank = rank;
}

MessageTreeNode::~MessageTreeNode()
{
    Release();

    if (message != NULL && message->Dereference())
    {
            delete message;
    }
}

MessageTreeNode * MessageTreeNode::Grandparent()
{
    if (parent != NULL)
        return parent->parent;
    else
        return NULL;
}

MessageTreeNode * MessageTreeNode::Uncle()
{
    MessageTreeNode *g = Grandparent();
    if (g == NULL)
        return NULL; // No grandparent means no uncle
    if (parent == g->left)
        return g->right;
    else
        return g->left;
}

MessageTreeNode * MessageTreeNode::Next()
{
    MessageTreeNode *p;
    MessageTreeNode *node;

    if (right != NULL) {
        /* One right, then keep on going left... */
        for (node = right; node->left != NULL; node = node->left);
    } else {
        p = parent;
        node = this;
        while (p != NULL && node == p->right) {
            node = p;
            p = p->parent;
        }
        node = p;
    }
    return node;
}

MessageTreeNode * MessageTreeNode::Previous()
{
    MessageTreeNode *p;
    MessageTreeNode *node; 

    if (left != NULL) {
        /* One left, then keep on going right... */
        for (node = left; node->right != NULL; node = node->right);
    }
    else {
        p = parent;
        node = this;
        while (p != NULL && node == p->left) {
            node = p;
            p = p->parent;
        }
        node = p;
    }
    return node;
}

void MessageTreeNode::Release()
{
    if (left != NULL)
    {
        left->Release();
        left = NULL;
    }


    if (right != NULL)
    {
        right->Release();
        right = NULL;
    }

    parent = NULL;

    target = NULL;

    if (message != NULL && message->Dereference())
    {
        delete message;
    }
    message = NULL;
}

int MessageTreeNode::Compare(const MessageTreeNode * m1, const MessageTreeNode * m2)
{
    int r = 0;

    if (m1 == NULL)
    {
        r = (m2 == NULL) ? 0 : -1;
    }
    else if (m2 == NULL)
    {
        r = 1;
    }
    else if (m1->scheduled_time < m2->scheduled_time)
    {
        r = -1;
    }
    else if (m1->scheduled_time > m2->scheduled_time)
    {
        r = 1;
    }
    else if (m1->insertion_rank < m2->insertion_rank)
    {
        r = -1;
    }
    else if (m1->insertion_rank > m2->insertion_rank)
    {
        r = 1;
    }
    else
    {
        r = 0;
    }

    return r;
}
