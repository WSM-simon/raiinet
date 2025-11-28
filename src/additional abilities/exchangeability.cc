export module exchangeability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class ExchangeAbility : public Ability {
public:
    ExchangeAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};