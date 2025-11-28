export module linkboostability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class LinkboostAbility : public Ability {
public:
    LinkboostAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};