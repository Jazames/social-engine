#pragma once
#include <string>
#include "classifier.h"
#include "dialogue.h"
#include "personality.h"

struct InteractionParameters {
	std::string dialogue;
	DialogueType classification;
	DialogueResponseDirection response_direction;
	Appearance appearance;
	Personality personality;
	Knowledge knowledge;
	std::string supplemental_info = "";
	bool use_llama = true;
};
