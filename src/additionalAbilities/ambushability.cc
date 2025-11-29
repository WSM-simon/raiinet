export module ambushability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class AmbushAbility : public Ability {
public:
    AmbushAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};