/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

