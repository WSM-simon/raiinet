export module downloadability;

import ability;
import game;
import player;
import util.abilityParams;
import util.abilityResult;

export class DownloadAbility : public Ability {
public:
    DownloadAbility();
    AbilityResult apply(Game& game, Player& user, Player& opponent,
                        AbilityParams& userParams, 
                        AbilityParams& opponentParams) override;
};