#include "pch.h"

#include "test_util.h"
#include "../SocialEngine/social.h"

namespace fast
{
    TEST(ComplimentResponseDirection, test_default_thank) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Default personality.

        Disposition disposition = get_disposition(a, k, p);
        DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
        ASSERT_EQ(direction, Thank);
    }

    TEST(ComplimentResponseDirection, test_wilt) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		//Create neurotic personality.
		p.traits.Volatility = 0.8;
		p.traits.Withdrawal = 0.8;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Wilt);
	}

	TEST(ComplimentResponseDirection, test_thank) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a personality that is highly agreeable and extraverted
		p.traits.Compassion = 0.9;
		p.traits.Politeness = 0.9;
		p.traits.Enthusiasm = 0.9;
		p.traits.Assertiveness = 0.9;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Thank);
	}

	TEST(ComplimentResponseDirection, test_ignore) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a personality that is low in enthusiasm and high in withdrawal
		p.traits.Enthusiasm = -0.5;
		p.traits.Withdrawal = 0.8;
		p.traits.Assertiveness = 0.2;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Ignore);
	}

	TEST(ComplimentResponseDirection, test_insult) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a personality that is low in compassion and politeness
		p.traits.Compassion = -0.5;
		p.traits.Politeness = -0.5;
		p.morals.care_harm = -0.8;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Deride);
	}

	TEST(ComplimentResponseDirection, test_psychopath_threaten) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a personality that is low in compassion and politeness
		p.traits.Compassion = -1.0;
		p.traits.Politeness = -1.0;
		p.traits.Volatility = 1.0;
		p.morals.care_harm = -1.0;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Threaten);
	}

	TEST(ComplimentResponseDirection, test_shy_wilt) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a shy personality
		p.traits.Politeness = -0.2;
		p.traits.Withdrawal = 0.8;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Wilt);
	}

	TEST(ComplimentResponseDirection, test_shy_thank) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		// Create a shy personality
		p.traits.Politeness = 0.4;
		p.traits.Withdrawal = 0.8;
		p.traits.Compassion = 0.8;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Thank);
	}

	TEST(ComplimentResponseDirection, test_volatile_thank) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		p.traits.Volatility = 0.8;
		p.traits.Compassion = 0.8;
		p.traits.Politeness = 0.8;
		p.traits.Enthusiasm = 0.8;
		p.traits.Assertiveness = 0.8;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Thank);
	}

	TEST(ComplimentResponseDirection, test_disagreeable_deride) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();
		
		p.traits.Compassion = -0.8;
		p.traits.Politeness = -0.8;
		p.morals.loyalty_betrayal = -0.2;
		p.morals.authority_subversion = -0.2;
		p.morals.care_harm = -0.4;

		Disposition disposition = get_disposition(a, k, p);
		DialogueResponseDirection direction = get_compliment_response_direction(disposition, p);
		ASSERT_EQ(direction, Deride);
	}
}