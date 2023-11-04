#include "pch.h"

#include "test_util.h"
#include "../SocialEngine/social.h"


namespace fast
{
    TEST(GreetingResponseDirection, test_default_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Default personality.

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Greet);
    }


    TEST(GreetingResponseDirection, test_wilt) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Create neurotic personality.
        p.traits.Volatility = 0.8;
        p.traits.Withdrawal = 0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(GreetingResponseDirection, test_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Create a personality that is highly agreeable and extraverted
        p.traits.Compassion = 0.9;
        p.traits.Politeness = 0.9;
        p.traits.Enthusiasm = 0.9;
        p.traits.Assertiveness = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Greet);
    }

    TEST(GreetingResponseDirection, test_ignore) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Create a personality that is low in enthusiasm and high in withdrawal
        p.traits.Enthusiasm = -0.5;
        p.traits.Withdrawal = 0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        EXPECT_TRUE(direction == Ignore || direction == Wilt);
    }

    TEST(GreetingResponseDirection, test__ignore) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Create a personality that is highly volatile and low in compassion
        p.traits.Volatility = 0.9;
        p.traits.Compassion = -0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(GreetingResponseDirection, test_disagreeable_insult) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Create a personality that is very low in agreeableness
        p.traits.Compassion = -0.8;
        p.traits.Politeness = -0.8;
        p.morals.loyalty_betrayal = -0.2;
        p.morals.authority_subversion = -0.2;
        p.morals.care_harm = -0.4;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Deride);
    }

    TEST(GreetingResponseDirection, test_enthusiastic_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Highly enthusiastic and open personality
        p.traits.Enthusiasm = 0.9;
        p.traits.Openness = 0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Greet);
    }

    TEST(GreetingResponseDirection, test_polite_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Polite but with a tendency to withdraw
        p.traits.Politeness = 0.9;
        p.traits.Withdrawal = 0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Greet);
    }

    TEST(GreetingResponseDirection, test_volatile_ignore) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Highly volatile personality
        p.traits.Volatility = 0.9;
        p.traits.Orderliness = -0.8;
        p.traits.Politeness = -0.8;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(GreetingResponseDirection, test_assertive_fight) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Highly assertive personality with negative moral foundations
        p.traits.Assertiveness = 0.9;
        p.morals.care_harm = -0.9;
        p.morals.liberty_oppression = -0.3;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Threaten);
    }

    TEST(GreetingResponseDirection, test_introverted_wilt) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Low enthusiasm and assertiveness leading to insult
        p.traits.Enthusiasm = -0.8;
        p.traits.Assertiveness = -0.8;
        p.traits.Withdrawal = 0.7;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(GreetingResponseDirection, test_childish_wilt) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Childish maturity leading to wilt
        p.traits.Withdrawal = 0.9;
        p.traits.Enthusiasm = -0.8;
        p.maturity = 0.1;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Wilt);
    }

    TEST(GreetingResponseDirection, test_wise_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Wise maturity with high compassion
        p.maturity = 0.9;
        p.traits.Compassion = 0.9;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Greet);
    }

    TEST(GreetingResponseDirection, test_high_iq_ignore) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // High intelligence but low agreeableness
        p.intelligence = 1.5;
        p.traits.Compassion = -0.6;
        p.traits.Politeness = -0.6;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Ignore);
    }

    TEST(GreetingResponseDirection, test_aggressive_fight) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // High assertiveness with negative care_harm morality
        p.traits.Assertiveness = 0.9;
        p.morals.care_harm = -0.8;
        p.morals.liberty_oppression = -0.3;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Threaten);
    }

    TEST(GreetingResponseDirection, test_unordered_insult) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        // Low orderliness leading to insult
        p.traits.Orderliness = -0.8;
        p.traits.Politeness = -1.0;
        p.traits.Compassion = -1.0;
        p.morals.loyalty_betrayal = -0.2;
        p.morals.authority_subversion = -0.2;
        p.morals.care_harm = -0.4;

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
        ASSERT_EQ(direction, Deride);
    }
}







