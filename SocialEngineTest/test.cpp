#include "pch.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}



struct InitiatorCharacteristics
{
    Appearance Appearance;
    Interaction Interaction;
};

struct NpcTraits
{
    Personality Personality;
    Morality Morality;
    vector<Goal> Goals;
    WorldKnowledge WorldKnowledge;
};

struct InteractionResponse
{
    double Inclination;
    string message;
};

struct Interaction
{

};

TEST(ThingyTest, TestCase1) {
    InitiatorCharacteristics initiator;
    NpcTraits receiver;
    InteractionResponse expected_response;
    InteractionResponse actual_response = GetInteractionResponse(initiator, receiver);
    EXPECT_EQ(expected_response, actual_response);
}

