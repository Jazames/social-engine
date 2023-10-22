
#include "pch.h"
#include "../SocialEngine/classifier.h"
#include "../SocialEngine/social.h"


class ClassificationEnvironment : public ::testing::Environment {
public:
    // Override this to define how to set up the environment.
    void SetUp() override {
        std::cout << "Classification setup\n";

        //warmup classification.
        Classifier::get_instance().get_classification("WHAT?");

        std::cout << "Classification Warmed Up" << std::endl;

    }

    // Override this to define how to tear down the environment.
    void TearDown() override {
        // Code here will run once after all tests finish.
        std::cout << "Classification teardown\n";
    }
};


TEST(DialogueClassificationTest, GreetingSuccess) {
    ASSERT_EQ(get_classification("Hello, how are you?"), DialogueType::Greeting);
}

TEST(DialogueClassificationTest, GreetingFailure) {
    ASSERT_NE(get_classification("Why is the sky blue?"), DialogueType::Greeting);
}

TEST(DialogueClassificationTest, InsultSuccess) {
    ASSERT_EQ(get_classification("You are an idiot."), DialogueType::InsultNoun);
}

TEST(DialogueClassificationTest, InsultFailure) {
    ASSERT_NE(get_classification("Can you help me?"), DialogueType::InsultNoun);
}

TEST(DialogueClassificationTest, RequestSuccess) {
    ASSERT_EQ(get_classification("Could you please pass the salt?"), DialogueType::Request);
}

TEST(DialogueClassificationTest, RequestFailure) {
    ASSERT_NE(get_classification("Nice to meet you."), DialogueType::Request);
}

TEST(DialogueClassificationTest, QuestionSuccess) {
    ASSERT_EQ(get_classification("What is your name?"), DialogueType::Question);
}

TEST(DialogueClassificationTest, QuestionFailure) {
    ASSERT_NE(get_classification("You look great today."), DialogueType::Question);
}

TEST(DialogueClassificationTest, RudeQuestion) {
    ASSERT_EQ(get_classification("I'm just curious, no judgement, but are you a prostitute?"), DialogueType::Question);
}

TEST(DialogueClassificationTest, RuderQuestion) {
    ASSERT_EQ(get_classification("I mean no disrespect, but are you a prostitute?"), DialogueType::Question);
}

TEST(DialogueClassificationTest, RudestQuestion) {
    auto classification = get_classification("Are you a prostitute?");
    EXPECT_TRUE(classification == Question || classification == DialogueType::InsultNoun);
}

TEST(DialogueClassificationTest, InsultingQuestion) {
    ASSERT_EQ(get_classification("Are you a whore?"), DialogueType::InsultNoun);
}

TEST(DialogueClassificationTest, InsultingQuestion2) {
    ASSERT_EQ(get_classification("Are you a retard?"), DialogueType::InsultNoun);
}

TEST(DialogueClassificationTest, DangerousQuestion) {
    ASSERT_EQ(get_classification("Where can I buy a bomb?"), DialogueType::Question);
}

TEST(DialogueClassificationTest, StatementSuccess) {
    ASSERT_EQ(get_classification("I like to eat cheese."), DialogueType::Statement);
}

TEST(DialogueClassificationTest, StatementFailure) {
    ASSERT_NE(get_classification("How do you do?"), DialogueType::Statement);
}

TEST(DialogueClassificationTest, ComplimentSuccess) {
    ASSERT_EQ(get_classification("I like your hat."), DialogueType::Compliment);
}

TEST(DialogueClassificationTest, ObviousComplimentSuccess) {
    ASSERT_EQ(get_classification("You're very smart."), DialogueType::Compliment);
}

TEST(DialogueClassificationTest, SubtleComplimentSuccess) {
    ASSERT_EQ(get_classification("I'm surprised by how much you can lift."), DialogueType::Compliment);
}

TEST(DialogueClassificationTest, ComplimentFailure) {
    ASSERT_NE(get_classification("It looks like it will rain today."), DialogueType::Compliment);
}


