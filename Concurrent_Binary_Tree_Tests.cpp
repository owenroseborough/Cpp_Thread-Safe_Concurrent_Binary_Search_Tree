#include <gtest/gtest.h>
#include <thread>
#include <random>
#include "../Concurrent Binary Tree/Concurrent_Binary_Tree.hpp"

void insert_random(std::vector<int>& insertedVals, Concurrent_Binary_Tree<int>& tree, size_t num, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, max);

    int random_num;
    for (size_t i = 0; i < num; i++) {
        random_num = distrib(gen);
        if (tree.addIterative(random_num)) {
            insertedVals.push_back(random_num);
        }
    }
}
/*
TEST(ConcurrentBinaryTreeTests, TwoThreadsInsert50Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> insertedValsT1;
    std::vector<int> insertedValsT2;

    size_t num_to_insert = 50;
    int maxRandomNumber = 100;

    std::thread t1(insert_random, std::ref(insertedValsT1), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t2(insert_random, std::ref(insertedValsT2), std::ref(tree), num_to_insert, maxRandomNumber);
    t1.join();
    t2.join();

    std::set<int> inserted;
    for (auto i = insertedValsT1.begin(); i != insertedValsT1.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT2.begin(); i != insertedValsT2.end(); i++) {
        inserted.insert(*i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i = inorder.begin(); i != inorder.end(); i++) {
        EXPECT_TRUE(inserted.contains(*i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(tree.getNumNodes() == inserted.size());
    EXPECT_TRUE(inorder.size() == inserted.size());
}

TEST(ConcurrentBinaryTreeTests, TwoThreadsInsert5000Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> insertedValsT1;
    std::vector<int> insertedValsT2;

    size_t num_to_insert = 5000;
    int maxRandomNumber = 10000;

    std::thread t1(insert_random, std::ref(insertedValsT1), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t2(insert_random, std::ref(insertedValsT2), std::ref(tree), num_to_insert, maxRandomNumber);
    t1.join();
    t2.join();

    std::set<int> inserted;
    for (auto i = insertedValsT1.begin(); i != insertedValsT1.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT2.begin(); i != insertedValsT2.end(); i++) {
        inserted.insert(*i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i = inorder.begin(); i != inorder.end(); i++) {
        EXPECT_TRUE(inserted.contains(*i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(tree.getNumNodes() == inserted.size());
    EXPECT_TRUE(inorder.size() == inserted.size());
}

TEST(ConcurrentBinaryTreeTests, TwoThreadsInsert50000Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> insertedValsT1;
    std::vector<int> insertedValsT2;

    size_t num_to_insert = 50000;
    int maxRandomNumber = 100000;

    std::thread t1(insert_random, std::ref(insertedValsT1), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t2(insert_random, std::ref(insertedValsT2), std::ref(tree), num_to_insert, maxRandomNumber);
    t1.join();
    t2.join();

    std::set<int> inserted;
    for (auto i = insertedValsT1.begin(); i != insertedValsT1.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT2.begin(); i != insertedValsT2.end(); i++) {
        inserted.insert(*i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i = inorder.begin(); i != inorder.end(); i++) {
        EXPECT_TRUE(inserted.contains(*i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(tree.getNumNodes() == inserted.size());
    EXPECT_TRUE(inorder.size() == inserted.size());
}

TEST(ConcurrentBinaryTreeTests, FourThreadsInsert50000Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> insertedValsT1;
    std::vector<int> insertedValsT2;
    std::vector<int> insertedValsT3;
    std::vector<int> insertedValsT4;

    size_t num_to_insert = 50000;
    int maxRandomNumber = 100000;

    std::thread t1(insert_random, std::ref(insertedValsT1), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t2(insert_random, std::ref(insertedValsT2), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t3(insert_random, std::ref(insertedValsT3), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t4(insert_random, std::ref(insertedValsT4), std::ref(tree), num_to_insert, maxRandomNumber);
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::set<int> inserted;
    for (auto i = insertedValsT1.begin(); i != insertedValsT1.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT2.begin(); i != insertedValsT2.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT3.begin(); i != insertedValsT3.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT4.begin(); i != insertedValsT4.end(); i++) {
        inserted.insert(*i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i = inorder.begin(); i != inorder.end(); i++) {
        EXPECT_TRUE(inserted.contains(*i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(tree.getNumNodes() == inserted.size());
    EXPECT_TRUE(inorder.size() == inserted.size());
}

TEST(ConcurrentBinaryTreeTests, EightThreadsInsert50000Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> insertedValsT1;
    std::vector<int> insertedValsT2;
    std::vector<int> insertedValsT3;
    std::vector<int> insertedValsT4;
    std::vector<int> insertedValsT5;
    std::vector<int> insertedValsT6;
    std::vector<int> insertedValsT7;
    std::vector<int> insertedValsT8;

    size_t num_to_insert = 50000;
    int maxRandomNumber = 100000;

    std::thread t1(insert_random, std::ref(insertedValsT1), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t2(insert_random, std::ref(insertedValsT2), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t3(insert_random, std::ref(insertedValsT3), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t4(insert_random, std::ref(insertedValsT4), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t5(insert_random, std::ref(insertedValsT5), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t6(insert_random, std::ref(insertedValsT6), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t7(insert_random, std::ref(insertedValsT7), std::ref(tree), num_to_insert, maxRandomNumber);
    std::thread t8(insert_random, std::ref(insertedValsT8), std::ref(tree), num_to_insert, maxRandomNumber);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    std::set<int> inserted;
    for (auto i = insertedValsT1.begin(); i != insertedValsT1.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT2.begin(); i != insertedValsT2.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT3.begin(); i != insertedValsT3.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT4.begin(); i != insertedValsT4.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT5.begin(); i != insertedValsT5.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT6.begin(); i != insertedValsT6.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT7.begin(); i != insertedValsT7.end(); i++) {
        inserted.insert(*i);
    }
    for (auto i = insertedValsT8.begin(); i != insertedValsT8.end(); i++) {
        inserted.insert(*i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i = inorder.begin(); i != inorder.end(); i++) {
        EXPECT_TRUE(inserted.contains(*i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(tree.getNumNodes() == inserted.size());
    EXPECT_TRUE(inorder.size() == inserted.size());
}
*/
void delete_random(std::vector<int>& deletedVals, Concurrent_Binary_Tree<int>& tree, size_t num, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, max);

    int random_num;

    for (size_t i = 0; i < num; i++) {
        random_num = distrib(gen);
        if (tree.deleteIterative(random_num)) {
            deletedVals.push_back(random_num);
        }
    }
}

TEST(ConcurrentBinaryTreeTests, TwoThreadsDelete50Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> deletedValsT1;
    std::vector<int> deletedValsT2;

    size_t num_to_delete = 50;
    int maxRandomNumber = 100;

    std::vector<int> insertedVals;
    insert_random(insertedVals, tree, 200, 100);

    size_t num_nodes_before = tree.getNumNodes();

    std::thread t1(delete_random, std::ref(deletedValsT1), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t2(delete_random, std::ref(deletedValsT2), std::ref(tree), num_to_delete, maxRandomNumber);
    t1.join();
    t2.join();

    size_t num_nodes_after = tree.getNumNodes();

    std::set<int> deleted;
    for(auto i : deletedValsT1){
        deleted.insert(i);
    }
    for (auto i : deletedValsT2) {
        deleted.insert(i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for(auto i : inorder){
        EXPECT_TRUE(!deleted.contains(i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(num_nodes_before - num_nodes_after == deleted.size());
}

TEST(ConcurrentBinaryTreeTests, FourThreadsDelete50Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> deletedValsT1;
    std::vector<int> deletedValsT2;
    std::vector<int> deletedValsT3;
    std::vector<int> deletedValsT4;

    size_t num_to_delete = 50;
    int maxRandomNumber = 200;

    std::vector<int> insertedVals;
    insert_random(insertedVals, tree, 400, 100);

    size_t num_nodes_before = tree.getNumNodes();

    std::thread t1(delete_random, std::ref(deletedValsT1), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t2(delete_random, std::ref(deletedValsT2), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t3(delete_random, std::ref(deletedValsT3), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t4(delete_random, std::ref(deletedValsT4), std::ref(tree), num_to_delete, maxRandomNumber);
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    size_t num_nodes_after = tree.getNumNodes();

    std::set<int> deleted;
    for (auto i : deletedValsT1) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT2) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT3) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT4) {
        deleted.insert(i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i : inorder) {
        EXPECT_TRUE(!deleted.contains(i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(num_nodes_before - num_nodes_after == deleted.size());
}

TEST(ConcurrentBinaryTreeTests, EightThreadsDelete100Random)
{
    Concurrent_Binary_Tree<int> tree;
    std::vector<int> deletedValsT1;
    std::vector<int> deletedValsT2;
    std::vector<int> deletedValsT3;
    std::vector<int> deletedValsT4;
    std::vector<int> deletedValsT5;
    std::vector<int> deletedValsT6;
    std::vector<int> deletedValsT7;
    std::vector<int> deletedValsT8;

    size_t num_to_delete = 100;
    int maxRandomNumber = 1600;

    std::vector<int> insertedVals;
    insert_random(insertedVals, tree, 1600, 100);

    size_t num_nodes_before = tree.getNumNodes();

    std::thread t1(delete_random, std::ref(deletedValsT1), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t2(delete_random, std::ref(deletedValsT2), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t3(delete_random, std::ref(deletedValsT3), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t4(delete_random, std::ref(deletedValsT4), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t5(delete_random, std::ref(deletedValsT5), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t6(delete_random, std::ref(deletedValsT6), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t7(delete_random, std::ref(deletedValsT7), std::ref(tree), num_to_delete, maxRandomNumber);
    std::thread t8(delete_random, std::ref(deletedValsT8), std::ref(tree), num_to_delete, maxRandomNumber);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    size_t num_nodes_after = tree.getNumNodes();

    std::set<int> deleted;
    for (auto i : deletedValsT1) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT2) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT3) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT4) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT5) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT6) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT7) {
        deleted.insert(i);
    }
    for (auto i : deletedValsT8) {
        deleted.insert(i);
    }

    std::vector<int> inorder = tree.getInorderTraversal();
    for (auto i : inorder) {
        EXPECT_TRUE(!deleted.contains(i));
    }
    for (size_t i = 0; i < inorder.size() - 1; i++) {
        EXPECT_TRUE(inorder[i] < inorder[i + 1]);
    }
    EXPECT_TRUE(num_nodes_before - num_nodes_after == deleted.size());
}

void getMins(Concurrent_Binary_Tree<int>& tree, std::vector<int>& mins) {
    for (size_t i = 0; i < 1000; i++) {
        mins.push_back(tree.getMin());
    }
}

TEST(ConcurrentBinaryTreeTests, TwoThreadsGetMin)
{
    Concurrent_Binary_Tree<int> tree;

    for (int i = 0; i < 1000; i++) {
        tree.addIterative(i);
    }
    for (int i = -1; i > -1000; i--) {
        tree.addIterative(i);
    }

    std::vector<int> t1Mins;
    std::vector<int> t2Mins;
    
    std::thread t1(getMins, std::ref(tree), std::ref(t1Mins));
    std::thread t2(getMins, std::ref(tree), std::ref(t2Mins));
    t1.join();
    t2.join();

    for (auto i : t1Mins) {
        EXPECT_TRUE(i == -999);
    }
    for (auto i : t2Mins) {
        EXPECT_TRUE(i == -999);
    }
}