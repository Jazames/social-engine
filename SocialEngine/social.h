#pragma once

#include "classifier.h"
#include "Personality.h"
#include "dialogue.h"

DialogueType get_classification(std::string dialogue);

std::string get_npc_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge);

Disposition get_disposition(Appearance appearance, Knowledge knowledge, Personality personality);

DialogueResponseDirection get_greeting_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_insult_response_direction(Disposition disposition, Personality personality);







