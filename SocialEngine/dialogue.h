#pragma once


enum DialogueResponseDirection {
    Greet,
    Deride, //Synonym for insult
    Fight,
    Ignore,
    Wilt,
    Accept,
    Disagree,
    Answer,
    Lie,
    Assist,
    Decline,
    Sabotage
};

struct Disposition {
    double friendliness = 0; // 0 is neutral, negative is bad, positive is good.

    /*
    Disposition()
    {
        this->friendliness = 0.5;
    }

    Disposition(double friendliness)
    {
        this->friendliness = friendliness;
    }
    */
};

struct CharacterKnowledge
{
    uint64_t face_id = 0;
    Disposition disposition;
    //TODO: Put some stuff here about things it's known this guy has/can do.
};

struct Knowledge {
    std::vector<uint64_t> known_faces() const
    {
        //TODO: Might need to cache this
        std::vector<uint64_t> face_ids;
        for (const CharacterKnowledge& person : known_people)  // Use a reference to avoid copying
        {
            face_ids.push_back(person.face_id);
        }
        return face_ids;
    }

    CharacterKnowledge get_character(uint64_t face_id) const
    {
        auto iterator = std::find_if(known_people.begin(), known_people.end(), [face_id](const CharacterKnowledge& item)
            {
                return item.face_id == face_id;
            });

        if (iterator != known_people.end()) {
            return (*iterator);
        }

        return CharacterKnowledge();
    }
    std::vector<CharacterKnowledge> known_people;
    std::vector<int> known_fact_ids;
};