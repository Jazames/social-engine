#pragma once

#include "bert_classifier.h"
#include "Personality.h"
#include "dialogue.h"


void init_social_engine();

DialogueType get_classification(std::string dialogue);

std::string get_default_response(std::string);
std::string get_npc_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge);

Disposition get_disposition(Appearance appearance, Knowledge knowledge, Personality personality);

DialogueResponseDirection get_greeting_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_compliment_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_insult_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_question_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_statement_response_direction(Disposition disposition, Personality personality);
DialogueResponseDirection get_request_response_direction(Disposition disposition, Personality personality);







