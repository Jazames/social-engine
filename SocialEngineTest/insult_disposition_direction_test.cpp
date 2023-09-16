#include "pch.h"

#include "util.h"
#include "../SocialEngine/social.h"



TEST(InsultResponseDirection, test_default_insult) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    //Default personality should ignore

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Ignore);
}

TEST(InsultResponseDirection, test_disagreeable_insult) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    //Low agreeableness personality should insult back
    p.traits.Compassion = -0.8;
    p.traits.Politeness = -0.8;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, InsultVerb);
}

TEST(InsultResponseDirection, test_polite_insult) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    //High agreeableness personality should greet or ignore.
    p.traits.Compassion = 0.7;
    p.traits.Politeness = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    EXPECT_TRUE(direction == Greet || direction == Ignore);
}

TEST(InsultResponseDirection, test_high_politeness_greet) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High politeness should result in greet response to insult
    p.traits.Politeness = 0.9;
    p.traits.Assertiveness = 0.7;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Greet);
}

TEST(InsultResponseDirection, test_high_volatility_insult_verb) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High volatility should result in insult back (InsultVerb) response to insult
    p.traits.Volatility = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, InsultVerb);
}

TEST(InsultResponseDirection, test_high_industriousness_ignore) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High industriousness should result in ignore response to insult
    p.traits.Industriousness = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Ignore);
}

TEST(InsultResponseDirection, test_negative_care_harm_fight) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // Negative care_harm should result in fight response to insult
    p.morals.care_harm = -0.9;
    p.traits.Assertiveness = 0.5;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Fight);
}

TEST(InsultResponseDirection, test_shy_wilt) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High withdrawal (shy) should result in wilt response to insult
    p.traits.Withdrawal = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Wilt);
}

TEST(InsultResponseDirection, test_high_compassion_politeness_greet) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High compassion and politeness should result in greet response to insult
    p.traits.Compassion = 0.9;
    p.traits.Politeness = 0.9;
    p.traits.Assertiveness = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Greet);
}

TEST(InsultResponseDirection, test_high_withdrawal_low_assertiveness_wilt) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High withdrawal and low assertiveness should result in wilt response to insult
    p.traits.Withdrawal = 0.9;
    p.traits.Assertiveness = -0.8;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Wilt);
}

TEST(InsultResponseDirection, test_negative_loyalty_fight) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    //Highly loyal will insult back.
    p.morals.loyalty_betrayal = 0.8;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, InsultVerb);
}

TEST(InsultResponseDirection, test_low_openness_ignore) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // Low openness should result in ignore response to insult
    p.traits.Openness = -0.8;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Ignore);
}

TEST(InsultResponseDirection, test_low_politeness_insult_verb) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // Low politeness should result in insult back (InsultVerb) response to insult
    p.traits.Politeness = -0.8;
    p.morals.care_harm = 0.3;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, InsultVerb);
}

TEST(InsultResponseDirection, test_high_liberty_oppression_ignore) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High liberty_oppression should result in an ignore response to insult
    p.morals.liberty_oppression = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Ignore);
}

TEST(InsultResponseDirection, test_high_sanctity_degradation_wilt) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High sanctity degradation should result in a wilt response to insult
    p.morals.sanctity_degradation = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    EXPECT_TRUE(direction == Wilt || direction == Ignore);
}

TEST(InsultResponseDirection, test_negative_care_harm_high_volatility_fight) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // Negative care_harm and high volatility should result in a fight response to insult
    p.morals.care_harm = -0.9;
    p.traits.Volatility = 0.9;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, Fight);
}

TEST(InsultResponseDirection, test_high_fairness_greet) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High fairness should result in a greet response to insult
    p.morals.fairness_cheating = 0.9;
    p.traits.Politeness = 0.7;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    EXPECT_TRUE(direction == Greet || direction == Ignore);
}

TEST(InsultResponseDirection, test_high_assertiveness_low_politeness_insult_verb) {
    Personality p = get_default_personality();
    Appearance a = get_default_appearance();
    Knowledge k = get_default_knowledge();

    // High assertiveness and low politeness should result in insult back (InsultVerb) response to insult
    p.traits.Assertiveness = 0.9;
    p.traits.Politeness = -0.8;

    Disposition disposition = get_disposition(a, k, p);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, p);
    ASSERT_EQ(direction, InsultVerb);
}


















