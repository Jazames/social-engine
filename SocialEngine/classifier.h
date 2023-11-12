#pragma once

#include <string>

enum DialogueType {
    IncoherentRambling, //This is used in the case of error.
    Greeting,
    Compliment,
    Insult,
    Request,
    Question,
    Statement
};

inline std::string get_dialogue_type_name(DialogueType type) {
	const std::string dialogue_type_names[] = {
		"IncoherentRambling",
		"Greeting",
		"Compliment",
		"Insult",
		"Request",
		"Question",
		"Statement"
	};
	return dialogue_type_names[type];
}
