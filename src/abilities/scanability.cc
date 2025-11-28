export module scanability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class ScanAbility : public Ability {
public:
    ScanAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};