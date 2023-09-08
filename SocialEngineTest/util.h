#pragma once

#include "../SocialEngine/Personality.hpp"
#include "../SocialEngine/dialogue.h"



Personality get_default_personality()
{
	Personality personality = Personality();

	personality.intelligence = 1.0;
	personality.maturity = Adult;

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

