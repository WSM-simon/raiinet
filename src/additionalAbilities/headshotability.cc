export module headshotability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class HeadshotAbility : public Ability {
public:
    HeadshotAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};