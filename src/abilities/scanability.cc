export module scanability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class ScanAbility : public Ability {
public:
    ScanAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};