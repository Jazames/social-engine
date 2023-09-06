#pragma once

#include "classifier.h"
#include "Personality.hpp"
#include "dialogue.h"

DialogueType get_classification(std::string dialogue);

std::string get_npc_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge);










