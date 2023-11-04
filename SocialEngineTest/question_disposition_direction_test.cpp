#include "pch.h"

#include "test_util.h"
#include "../SocialEngine/social.h"


namespace fast
{
    TEST(QuestionResponseDirection, test_default_question) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Default personality.

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Answer);
    }

    TEST(QuestionResponseDirection, test_question_with_high_disposition) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //High disposition due to knowledge.
        a.face_id = 123;
        CharacterKnowledge c = { a.face_id, 0.9 };
        k.known_people.push_back(c);

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Answer);
    }

    TEST(QuestionResponseDirection, test_question_with_low_disposition) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Low disposition due to knowledge.
        a.face_id = 123;
        CharacterKnowledge c = { a.face_id, -0.5 };
        k.known_people.push_back(c);

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(QuestionResponseDirection, test_question_with_high_disposition_and_high_personality) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //High disposition due to knowledge.
        a.face_id = 123;
        CharacterKnowledge c = { a.face_id, 0.9 };
        k.known_people.push_back(c);

        //High agreeableness.
        p.traits.Politeness = 0.9;
        p.traits.Compassion = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Answer);
    }

    TEST(QuestionResponseDirection, test_derisive_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Compassion = -0.9;
        p.traits.Politeness = -0.9;
        p.morals.care_harm = -0.2;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Deride);
    }

    TEST(QuestionResponseDirection, test_high_volatility_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Volatility = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Deride);
    }

    TEST(QuestionResponseDirection, test_combative_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Assertiveness = 0.9;
        p.traits.Compassion = -0.9;
        p.traits.Politeness = -0.9;
        p.morals.authority_subversion = -0.9;
        p.morals.care_harm = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Threaten);
    }

    TEST(QuestionResponseDirection, test_low_agreeable_volatile_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Politeness = -0.9;
        p.traits.Compassion = -0.9;
        p.traits.Volatility = 0.9;
        p.morals.care_harm = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Threaten);
    }

    TEST(QuestionResponseDirection, test_indifferent_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Enthusiasm = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(QuestionResponseDirection, test_low_openness_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Openness = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(QuestionResponseDirection, test_low_politeness_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Politeness = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(QuestionResponseDirection, test_high_withdrawal_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Withdrawal = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(QuestionResponseDirection, test_low_assertiveness_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Withdrawal = 0.3;
        p.traits.Assertiveness = -0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(QuestionResponseDirection, test_high_intelligence_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.intelligence = 1.5;
        p.traits.Intellect = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Answer);
    }

    TEST(QuestionResponseDirection, test_high_openness_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.traits.Openness = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Answer);
    }

    TEST(QuestionResponseDirection, test_manipulative_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.morals.fairness_cheating = -0.9;
        p.traits.Politeness = -0.5;
        p.traits.Compassion = -0.5;
        p.morals.care_harm = -0.5;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Lie);
    }

    TEST(QuestionResponseDirection, test_low_morality_personality) {
        Personality p;
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        p.morals.care_harm = -0.9;
        p.morals.fairness_cheating = -0.9;
        p.traits.Politeness = -0.5;
        p.traits.Compassion = -0.5;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_question_response_direction(disposition, p);
        ASSERT_EQ(direction, Lie);
    }
}

