export module ambushability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class AmbushAbility : public Ability {
public:
    AmbushAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};