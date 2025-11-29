export module exchangeability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class ExchangeAbility : public Ability {
public:
    ExchangeAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};