#include <stdlib.h>
#include <algorithm>    // std::max
#include "../octosimlib/MessageTree.h"
#include "TestModels.h"
#include "MessageTreetest.h"

MessageTreetest::MessageTreetest()
    :
    tree(NULL),
    added(0)
{
}

MessageTreetest::~MessageTreetest()
{
    if (tree != NULL)
        delete tree;
}

/*
 * Create a message tree.
 * This is tested by creating enough values to get an interesting tree,
 * in our case 15 values.
 *
 * The test will:
 * 1) create all odd values from 1 to 13, in order.
 * 2) create all even values from 14 to 2, in order.
 * 3) create the value 15.
 * 4) delete all even numbers, from 2 to 14.
 * 5) delete 15.
 * 6) delete 1.
 * 7) check that repeated insert/delete work as expected.
 * After each action, the test will check tree correctness:
 * 1) Check that root is black.
 * 2) Check that all leaf paths have the same number of black tokens,
 *    counting NULL leaves as black.
 * 3) Check that no red child has a red parent.
 */

bool MessageTreetest::MessageTreeDoTest()
{
    bool ret = true;
    int node_first = 1;
    int node_last = 15; // expect node last to be odd.


    if (tree != NULL)
    {
        tree->Release();
    }
    else
    {
        tree = new MessageTree();
    }

    /* First, test a single message add and delete */
    ret = InsertNodeTest(1, false);

    if (ret)
    {
        ret = DeleteNodeTest(1, true);
    }

    /* Then, test Get First and Get Last on an empty tree */
    if (ret)
    {
        MessageTreeNode * mtn = tree->First();

        ret = (mtn == NULL);

        if (ret)
        {
            mtn = tree->Last();
            ret = (mtn == NULL);
        }
    }

    /* Now, proceed with the full construction test */
    for (int i = node_first; ret && i < node_last; i += 2)
    {
        ret = InsertNodeTest(i, false);
    }

    for (int i = node_last - 1; ret && i > node_first; i -= 2)
    {
        ret = InsertNodeTest(i, false);
    }

    if (ret)
    {
        ret = InsertNodeTest(node_last, false);
    }

    if (ret)
    {
        ret = tree->Count() == node_last - node_first + 1;
    }

    if (ret)
    {
        ret = CheckTreeOrder(node_first, node_last);
    }

    for (int i = node_first + 1; ret && i < node_last; i += 2)
    {
        ret = DeleteNodeTest(i, true);
    }

    if (ret)
    {
        ret = DeleteNodeTest(node_last, true);
    }

    if (ret)
    {
        ret = DeleteNodeTest(node_first, true);
    }

    if (ret)
    {
        ret = DeleteNodeTest(node_first, false);
    }

    if (ret)
    {
        ret = InsertNodeTest(node_first, false);
    }

    if (ret)
    {
        ret = InsertNodeTest(node_first, true);
    }

    for (int i = 0; ret && i < 3; i++)
    {
        MessageTreeNode * mtn = tree->First();

        if (mtn == NULL)
        {
            break;
        }
        else
        {
            ret = DeleteNodeTest((int) mtn->scheduled_time, true);
        }
    }

    tree->Release();

    return ret;
}

bool MessageTreetest::MessageTreeCheck()
{
    bool ret = true;
    maxBlack = 0;
    minBlack = 1000000;
    maxHops = 0;
    minHops = 1000000;
    counted = 0;
    foundTwoReds = false;

    rootIsBlack = true;

    if (tree->Root() != NULL)
    {
        if (tree->Root()->color == MessageNodeRed)
        {
            ret = false;
        }
        else
        {
            MessageNodeCheck(tree->Root(), 0, 0);

            
            if (maxBlack != minBlack)
            {
                ret = false;
            }
            else if (maxHops > 2 * minHops)
            {
                ret = false;
            }
            else if (foundTwoReds)
            {
                ret = false;
            }
            else if (counted != tree->Count())
            {
                ret = false;
            }
        }
    }

    return ret;
}

void MessageTreetest::MessageNodeCheck(const MessageTreeNode * node, int hops, int nb_blacks)
{
    hops++;
    if (node == NULL)
    {
        nb_blacks++;
        maxBlack = std::max(maxBlack, nb_blacks);
        minBlack = std::min(maxBlack, nb_blacks);
        maxHops = std::max(maxHops, hops);
        minHops = std::min(minHops, hops);
    }
    else
    {
        counted++;
        if (node->color == MessageNodeBlack)
        {
            nb_blacks++;
        }
        else if (node->parent->color == MessageNodeRed)
        {
            foundTwoReds = true;
        }

        MessageNodeCheck(node->left, hops, nb_blacks);
        MessageNodeCheck(node->right, hops, nb_blacks);
    }
}

bool MessageTreetest::CheckTreeOrder(int first_node, int last_node)
{
    bool ret = true;

    MessageTreeNode * mtn = tree->First();
    if (mtn == NULL || mtn->scheduled_time != first_node)
    {
        ret = false;
    }

    for (int i = first_node + 1; ret && i <= last_node; i++)
    {
        mtn = mtn->Next();

        if (mtn == NULL || mtn->scheduled_time != i)
        {
            ret = false;
        }
    }

    if (ret)
    {
        mtn = tree->Last();

        if (mtn == NULL || mtn->scheduled_time != last_node)
        {
            ret = false;
        }
    }

    for (int i = last_node - 1; ret && i >= first_node; i--)
    {
        mtn = mtn->Previous();

        if (mtn == NULL || mtn->scheduled_time != i)
        {
            ret = false;
        }
    }
    return ret;
}

bool MessageTreetest::InsertNodeTest(int node_number, bool expectedExist)
{
    bool ret = true;
    TestMessage * m = new TestMessage(node_number);
    MessageTreeNode * mtd = new MessageTreeNode(NULL, m,
        (unsigned long long)node_number, (unsigned long long) node_number);
    MessageTreeNode * inserted = tree->Insert(mtd);

    if (inserted == NULL)
    {
        delete mtd;
        if (!expectedExist)
        {
            ret = false;
        }
    }
    else if (expectedExist)
    {
        ret = false;
    }
    else
    {
        added++;

        ret = MessageTreeCheck();
    }

    return ret;
}

bool MessageTreetest::DeleteNodeTest(int node_number, bool expectedExist)
{
    bool ret = true;
    MessageTreeNode * key = new MessageTreeNode(NULL, NULL,
        (unsigned long long)node_number, (unsigned long long) node_number);
    MessageTreeNode * deleted = tree->Remove(key);

    if (deleted == NULL)
    {
        if (expectedExist)
        {
            ret = false;
        }
    }
    else
    {
        delete deleted;
        added--;

        if (!expectedExist)
        {
            ret = false;
        }
        else
        {
            ret = MessageTreeCheck();
        }
    }

    delete key;

    return ret;
}

