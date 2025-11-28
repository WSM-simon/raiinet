export module ability;

import <string>;
import <utility>;
import util.abilityParams;
import util.abilityResult;
import board;
import player;

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
    virtual AbilityResult apply(Board& board, Player& user, Player& opponent, 
                                AbilityParams& userParams,
                                AbilityParams& opponentParams) = 0;
};