#pragma once

#include "../SocialEngine/Personality.hpp"
#include "../SocialEngine/dialogue.h"



Personality get_default_personality()
{
	Personality personality = Personality();

	personality.intelligence = 1.0;
	personality.maturity = Adult;
	personality.morals.care_harm = 0.5;
	personality.morals.fairness_cheating = 0.5;
	personality.morals.authority_subversion = 0.25;
	personality.morals.sanctity_degradation = 0.25;
	personality.morals.loyalty_betrayal = 0.25;
	personality.morals.liberty_oppression = 0.25;

	return personality;
}

Appearance get_default_appearance()
{
	return Appearance();
}

Knowledge get_default_knowledge()
{
	return Knowledge();
}

