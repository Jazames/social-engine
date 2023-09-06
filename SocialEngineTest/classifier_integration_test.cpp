
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
    ASSERT_EQ(get_classification("You are an idiot."), DialogueType::Insult);
}

TEST(DialogueClassificationTest, InsultFailure) {
    ASSERT_NE(get_classification("Can you help me?"), DialogueType::Insult);
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

TEST(DialogueClassificationTest, StatementSuccess) {
    ASSERT_EQ(get_classification("The weather is nice today."), DialogueType::Statement);
}

TEST(DialogueClassificationTest, StatementFailure) {
    ASSERT_NE(get_classification("How do you do?"), DialogueType::Statement);
}

TEST(DialogueClassificationTest, IncoherentRamblingSuccess) {
    ASSERT_EQ(get_classification("asdfas dfas asdf asd fasd"), DialogueType::IncoherentRambling);
}

TEST(DialogueClassificationTest, IncoherentRamblingFailure) {
    ASSERT_NE(get_classification("This is a coherent sentence."), DialogueType::IncoherentRambling);
}
