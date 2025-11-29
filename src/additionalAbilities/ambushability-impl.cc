module ambushability;

import ability;
import board;
import player;
import link;
import util.abilityParams;
import util.abilityResult;


AmbushAbility::AmbushAbility() : Ability("Ambush", 'A') {}

AbilityResult AmbushAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)board;
    (void)opponent;
    (void)opponentParams; // Don't need these for AmbushAbility

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

    if (!userLink->isVisible()) {
        res.header.success = false;
        res.header.msg = "Link already invisible.";
        return res;
    }

    userLink->setVisible(false);

    markUsed();
    res.used = true;
    res.header.success = true;
    res.header.msg = "";
    return res;
}


