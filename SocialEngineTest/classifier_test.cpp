
#include "pch.h"
//#include "../SocialEngine/classifier.h"
#include "../SocialEngine/social.h"



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


