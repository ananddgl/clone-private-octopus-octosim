#pragma once
#include "IModels.h"
/*
 * RB Tree containing a list of messages and events.
 */

enum MessageNodeColor {
    MessageNodeRed,
    MessageNodeBlack
};

class MessageTree;

 /**
  * The Node definition
  */

class MessageTreeNode
{
public:
    MessageTreeNode();
    MessageTreeNode(ISimObject * t, ISimMessage * m, unsigned long long s_time, unsigned long long rank);
    ~MessageTreeNode();

    MessageTreeNode * Grandparent();
    MessageTreeNode * Uncle();
    MessageTreeNode * Next();
    MessageTreeNode * Previous();

    MessageTreeNode * parent;
    MessageTreeNode * left;
    MessageTreeNode * right;
    MessageNodeColor color;
    ISimObject * target;
    ISimMessage * message;
    unsigned long long scheduled_time;
    unsigned long long insertion_rank;

    void Release();

    static int Compare(const MessageTreeNode * m1, const MessageTreeNode * m2);
};


class MessageTree
{
public:
    MessageTree();
    ~MessageTree();

    MessageTreeNode *Insert(MessageTreeNode *data);
    MessageTreeNode *Remove(const MessageTreeNode *key);
    MessageTreeNode *Search(const MessageTreeNode *key);
    bool FindLessEqual(const MessageTreeNode *key,
        MessageTreeNode **result);
    MessageTreeNode *First();
    MessageTreeNode *Last();

    void Release();

    const MessageTreeNode * Root();
    const int Count();
  
private:
    /** The root of the red-black tree */
    MessageTreeNode    *root;
    
    /** The number of the nodes in the tree */
    size_t          count;

    void InsertFixup(MessageTreeNode *node);
    void RotateRight(MessageTreeNode *node);
    void RotateLeft(MessageTreeNode *node);
    void SwapColors(MessageNodeColor* x, MessageNodeColor* y);
    void SwapMessagePtr(MessageTreeNode ** x, MessageTreeNode ** y);
    void ChangeParentPtr(MessageTreeNode* parent,
        MessageTreeNode* oldParent, MessageTreeNode* newParent);
    void ChangeChildPtr(MessageTreeNode* child, MessageTreeNode* oldChild,
        MessageTreeNode* newChild);
    void DeleteFixup(MessageTreeNode* child,
        MessageTreeNode* child_parent);
};

