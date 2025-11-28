export module firewallability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class FirewallAbility : public Ability {
public:
    FirewallAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};