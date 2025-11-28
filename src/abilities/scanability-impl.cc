module scanability;

import ability;
import game;
import player;
import link;
import util.abilityParams;
import util.abilityResult;


ScanAbility::ScanAbility() : Ability("Scan", 'S') {}

AbilityResult ScanAbility::apply(Game& game, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)game;
    (void)user;
    (void)userParams; // Don't need this for ScanAbility

    AbilityResult res;

    Link* opponentLink = opponent.getLinkById(opponentParams.targetLinkId);

    if (!opponentLink) {
        res.header.success = false;
        res.header.msg = "No such link for this player.";
        return res;
    }

    if (!opponentLink->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    if (opponentLink->isRevealedToOpponent()) {
        res.header.success = false;
        res.header.msg = "Link already revealed.";
        return res;
    }

    opponentLink->revealToOpponent();
    markUsed();
    res.used = true;
    res.stateChanged = true;
    res.affectedLinks.push_back(opponentLink);
    return res;
}