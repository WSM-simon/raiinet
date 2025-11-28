export module polarizeability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class PolarizeAbility : public Ability {
public:
    PolarizeAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};