#include "pch.h"

#include "util.h"
#include "../SocialEngine/social.h"



TEST(disposition_direction, test_neurotic) {
	Personality p = get_default_personality();
	Appearance a = get_default_appearance();
	Knowledge k = get_default_knowledge();

	Disposition disposition = get_disposition(a, k, p);
	DialogueResponseDirection direction = get_greeting_response_direction(disposition, p);
}












