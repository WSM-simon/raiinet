export module polarizeability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class PolarizeAbility : public Ability {
public:
    PolarizeAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};