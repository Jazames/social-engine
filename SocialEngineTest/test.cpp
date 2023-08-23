#include "pch.h"
#include "../SocialEngine/Personality.hpp"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}



struct Interaction
{
    std::string thing = "";
};


struct InitiatorCharacteristics
{
    Appearance appearance;
    Interaction interaction;
};

struct NpcTraits
{
    Personality personality;
};

struct InteractionResponse
{
    double Inclination;
};
TEST(ThingyTest, TestCase1) {
    InitiatorCharacteristics initiator;
    NpcTraits receiver;
    InteractionResponse expected_response;
    InteractionResponse actual_response;// = GetInteractionResponse(initiator, receiver);
    EXPECT_EQ(expected_response.Inclination, actual_response.Inclination);
}

