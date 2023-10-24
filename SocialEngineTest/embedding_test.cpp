#include "pch.h"
#include "../SocialEngine/global_knowledge.h"

class GlobalKnowledgeTest : public ::testing::Test {
protected:
    // This method will be called before each test
    void SetUp() override {
        // Setup code...
    }

    // This method will be called after each test
    void TearDown() override {
        //Clear out the mappings between each test so they don't interfere with each other.
        GlobalKnowledge::get_instance().reset();
    }
};

TEST_F(GlobalKnowledgeTest, SingletonTest) {
    auto& gk1 = GlobalKnowledge::get_instance();
    auto& gk2 = GlobalKnowledge::get_instance();
    ASSERT_EQ(&gk1, &gk2);  // Ensures that both references point to the same instance
}

TEST_F(GlobalKnowledgeTest, AddKnowledgeTest) {
    auto& gk = GlobalKnowledge::get_instance();
    int index = gk.add_knowledge("hello");
    ASSERT_EQ(index, 0);  // First item should be at index 0
    index = gk.add_knowledge("world");
    ASSERT_EQ(index, 1);  // Second item should be at index 1
    index = gk.add_knowledge("hello");
    ASSERT_EQ(index, 0);  // "hello" already exists, should return index 0
}

TEST_F(GlobalKnowledgeTest, GetKnowledgeTest) {
    auto& gk = GlobalKnowledge::get_instance();
    int index = gk.add_knowledge("hello");
    ASSERT_EQ(gk.get_knowledge(index), "hello");
    ASSERT_EQ(gk.get_knowledge(index + 1), "");  // Index out of range, should return empty string
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsSimpleTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("hello");
    gk.add_knowledge("world");
    auto closest_items = gk.get_closest_items("hello", 2);
    ASSERT_EQ(closest_items.size(), 2);
    ASSERT_EQ(closest_items[0], "hello");  // "hello" should be the closest item to itself
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsMildTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("shark");
    gk.add_knowledge("cat");
    auto closest_items = gk.get_closest_items("kitten", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], "cat");  // "cat" should be the closest item to kitten
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsMidTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("spoon");
    gk.add_knowledge("knife");
    gk.add_knowledge("fork");
    gk.add_knowledge("napkin");
    gk.add_knowledge("glass");
    gk.add_knowledge("chopsticks");
    auto closest_items = gk.get_closest_items("spork", 2);
    ASSERT_EQ(closest_items.size(), 2);
    ASSERT_TRUE(std::find(closest_items.begin(), closest_items.end(), "spoon") != closest_items.end());  // "spoon" should be one of the closest item to spork
    ASSERT_TRUE(std::find(closest_items.begin(), closest_items.end(), "fork") != closest_items.end());  // "fork" should be one of the closest item to spork
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsQuestionTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("The cat is on the roof.");
    gk.add_knowledge("The dog barks at the moon.");
    gk.add_knowledge("Birds fly in the sky.");

    auto closest_items = gk.get_closest_items("Tell me what animal is on the roof.", 1);

    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], "The cat is on the roof.");  // The sentence about the cat should be the closest
}


