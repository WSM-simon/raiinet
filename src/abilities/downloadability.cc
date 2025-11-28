export module downloadability;

import ability;
import board;
import player;
import util.abilityParams;
import util.abilityResult;

export class DownloadAbility : public Ability {
public:
    DownloadAbility();
    AbilityResult apply(Board& board, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};