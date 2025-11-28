export module ability;

import <string>;
import <utility>;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

using std::string;

export class Ability {
private: 
    string name;
    char code;
    bool used;

public:
    Ability(string name, char code);
    virtual ~Ability();

    string getName() const;
    char getCode() const;
    bool isUsed() const;
    void markUsed();
    virtual AbilityResult apply (Game& game, Player& user, Player& opponent, 
                                 AbilityParams& userParams,
                                 AbilityParams& opponentParams) = 0;
};