module linkboostability;

import ability;
import board;
import player;
import link;
import util.abilityParams;
import util.abilityResult;


LinkboostAbility::LinkboostAbility() : Ability("Linkboost", 'L') {}

AbilityResult LinkboostAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)board;
    (void)opponent;
    (void)opponentParams; // Don't need these for linkboostability

    AbilityResult res;

    Link* userLink = user.getLinkById(userParams.targetLinkId);

    if (!userLink) {
        res.header.success = false;
        res.header.msg = "No such link for this player.";
        return res;
    }

    if (!userLink->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    if (userLink->isBoosted()) {
        res.header.success = false;
        res.header.msg = "Link already boosted.";
        return res;
    }

    userLink->setBoosted(true);
    markUsed();
    res.used = true;
    return res;
}