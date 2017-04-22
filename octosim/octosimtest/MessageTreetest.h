#pragma once

class MessageTree;
class MessageTreeNode;

class MessageTreetest
{
public:
    MessageTreetest();
    ~MessageTreetest();

    bool MessageTreeDoTest();
private:
    MessageTree * tree;
    int added;
    int maxBlack;
    int minBlack;
    int maxHops;
    int minHops;
    int counted;
    bool rootIsBlack;
    bool foundTwoReds;

    bool MessageTreeCheck();
    void MessageNodeCheck(const MessageTreeNode * node, int hops, int nb_blacks);
    bool CheckTreeOrder(int first_node, int last_node);

    bool InsertNodeTest(int node_number, bool expectedExist);
    bool DeleteNodeTest(int node_number, bool expectedExist);
};

