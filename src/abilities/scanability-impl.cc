module scanability;

import ability;
import board;
import player;
import link;
import util.abilityParams;
import util.abilityResult;


ScanAbility::ScanAbility() : Ability("Scan", 'S') {}

AbilityResult ScanAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)board;
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

    opponentLink->setRevealedToOpponent(true);
    
    markUsed();
    res.used = true;
    return res;
}