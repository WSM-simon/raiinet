export module linkboostability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class LinkboostAbility : public Ability {
public:
    LinkboostAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};