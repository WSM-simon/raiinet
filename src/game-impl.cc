module game;

import <vector>;
import <string>;
import <memory>;

import board;
import player;
import link;
import util.direction;
import util.abilityParams;
import util.moveResult;
import util.abilityResult;
import util.linkType;
import util.abilityKind;
import util.linkSetup;
import util.playerSetup;
import ability;
import downloadability;
import firewallability;
import scanability;
import polarizeability;
import linkboostability;
import abilityFactory;

using std::vector;
using std::unique_ptr;
using std::move;
using std::make_unique;

/**
 * Game Constructor
 *
 * Initializes a new game with 2 players, their links and abilities.
 * NOTE: This does NOT place links on the board yet.
 * Call Game::setupDefaultLinks() or a custom setup method from Controller.
 */
Game::Game(bool useGraphics, bool extraFeatures, bool enableTextFlip)
    : board_{}
    , players_{}
    , abilities_{}
    , currentPlayer_{1}
    , winnerId_{-1}
    , gameOver_{false}
    , useGraphics_{useGraphics}
    , extraFeatures_{extraFeatures}
    , enableTextFlip_{enableTextFlip}
    , abilityUsedThisTurn_{false} {}

Ability* Game::getAbility(int playerIdx, int abilityIdx) {
    if (playerIdx < 1 || playerIdx > 2) return nullptr;
    if (abilityIdx < 0 || abilityIdx >= (int)abilities_[playerIdx].size()) return nullptr;
    return abilities_[playerIdx][abilityIdx].get();
}

void Game::initializeGame(PlayerSetup& p1Setup, PlayerSetup& p2Setup) {
    vector<Link> links1;
    vector<Link> links2;

    links1.reserve(p1Setup.linkSetups.size());
    links2.reserve(p2Setup.linkSetups.size());
    abilities_[1].reserve(p1Setup.abilities.size());
    abilities_[2].reserve(p2Setup.abilities.size());

    // ---- build links for player 1 ----
    for (LinkSetup& ls : p1Setup.linkSetups) {
        links1.emplace_back(1, ls.id, ls.type, ls.strength);
    }

    // ---- build abilities for player 1 ----
    for (AbilityKind& ak : p1Setup.abilities) {
        auto ability = makeAbility(ak);
        if (ability) abilities_[1].push_back(move(ability));
    }

    // ---- build links for player 2 ----
    for (LinkSetup& ls : p2Setup.linkSetups) {
        links2.emplace_back(2, ls.id, ls.type, ls.strength);
    }

    // ---- build abilities for player 2 ----
    for (AbilityKind& ak : p2Setup.abilities) {
        auto ability = makeAbility(ak);
        if (ability) abilities_[2].push_back(move(ability));
    }

    // Players own their links; Game owns abilities
    players_[1] = Player(1, move(links1));
    players_[2] = Player(2, move(links2));

    // ---- place links on board according to positions ----
    for (auto& ls : p1Setup.linkSetups) {
        Link* link = players_[1].getLinkById(ls.id);
        if (link) {
            board_.placeLink(link, ls.pos);
        }
    }

    for (auto& ls : p2Setup.linkSetups) {
        Link* link = players_[2].getLinkById(ls.id);
        if (link) {
            board_.placeLink(link, ls.pos);
        }
    }

    notifyObservers();
}

/**
 * Move a link on the board.
 *
 * 1. Finds the link belonging to current player
 * 2. Calls board_.moveLink to perform movement/battle/firewall/etc.
 * 3. Uses handleMoveResult to update players (downloads)
 * 4. Checks win condition
 * 5. Notifies observers
 */
MoveResult Game::moveLink(char linkId, Direction dir) {
    MoveResult result;

    if (isGameOver()) {
        result.header.success = false;
        result.header.msg = "Game is over.";
        return result;
    }

    Player& currentPlayer = getCurrentPlayer();

    // Find link in current player's links only
    Link* link = nullptr;
    for (auto& ln : currentPlayer.getLinks()) {
        if (ln.getId() == linkId) {
            link = &ln;
            break;
        }
    }

    if (!link) {
        result.header.success = false;
        result.header.msg = "That is not your link. You can only move your own links.";
        return result;
    }
    
    if (link->getOwnerId() != currentPlayer_) {
        result.header.success = false;
        result.header.msg = "You cannot move opponent's links.";
        return result;
    }

    // Delegate actual move to Board
    result = board_.moveLink(link, dir);

    // Update player download counts based on MoveResult
    handleMoveResult(result);

    // check win state after applying downloads
    checkWinCondition();

    // redraw displays
    notifyObservers();

    return result;
}

/**
 * Use an ability card.
 *
 * 1. Gets the ability from current player
 * 2. Checks used flag
 * 3. Calls ability->apply(...)
 * 4. If success and used, mark ability used
 * 5. If ability caused download(s), AbilityResult should reflect that
 * 6. Check win condition, notify observers
 */
AbilityResult Game::useAbility(int abilityIndex, AbilityParams& userParams, AbilityParams& opponentParams) {
    AbilityResult result;

    if (isGameOver()) {
        result.header.success = false;
        result.header.msg = "Game is over.";
        return result;
    }

    // Check if ability already used this turn
    if (abilityUsedThisTurn_) {
        result.header.success = false;
        result.header.msg = "You can only use one ability per turn.";
        return result;
    }

    Player& currentPlayer = getCurrentPlayer();
    Player& opponent = players_[3 - currentPlayer_];  // 3-1=2 or 3-2=1

    Ability* ability = getAbility(currentPlayer_, abilityIndex);
    if (!ability) {
        result.header.success = false;
        result.header.msg = "Invalid ability index.";
        return result;
    }

    if (ability->isUsed()) {
        result.header.success = false;
        result.header.msg = "Ability already used.";
        return result;
    }

    // Params are already separated for user and opponent by the controller
    result = ability->apply(board_, currentPlayer, opponent, userParams, opponentParams);

    if (result.header.success && result.used) {
        ability->markUsed();
        abilityUsedThisTurn_ = true;  // Mark ability used this turn
    }

    // If ability downloaded links, AbilityResult should carry them;
    // you can optionally call handleDownload on them here if needed.

    checkWinCondition();
    notifyObservers();

    return result;
}

/**
 * End current player's turn, switch to next.
 */
void Game::endTurn() {
    if (isGameOver()) return;

    switchPlayer();
    checkWinCondition();
    notifyObservers();
}

/**
 * Reset game state (board + players + winner flag).
 *
 * NOTE: players_ reset is minimal here; if Player has a reset() method,
 * you should call it instead of just leaving links/abilities as-is.
 */
void Game::reset() {
    board_.reset();

    // If Player has reset(), call it here.
    // for (int i = 1; i <= 2; ++i) {
    //     players_[i].reset();
    // }

    currentPlayer_ = 1;
    winnerId_ = -1;
    gameOver_ = false;
    abilityUsedThisTurn_ = false;

    notifyObservers();
}

/**
 * Is the game over?
 */
bool Game::isGameOver() {
    return gameOver_ || winnerId_ != -1;
}

/**
 * Get winner id, or -1 if no winner yet.
 */
int Game::getWinnerId() {
    return winnerId_;
}

/**
 * Handle a single downloaded link:
 * - Use the downloaderId from MoveResult to determine who gets credit
 * - Increase correct counter (data / virus)
 * - DO NOT touch board here (Board already removed link).
 */
void Game::handleDownload(Link& link, int downloaderId) {
    if (downloaderId < 1 || downloaderId > 2) {
        // Fallback to opponent if downloaderId not set (shouldn't happen)
        downloaderId = 3 - link.getOwnerId();
    }
    
    Player& downloader = players_[downloaderId];

    if (link.isData()) {
        downloader.incrementDownloadedData();
    } else if (link.isVirus()) {
        downloader.incrementDownloadedViruses();
    }

    // Board::moveLink or abilities already updated the board and link.onBoard.
    // We only track score/state here.
}

/**
 * Process MoveResult:
 * - If move is illegal, do nothing
 * - If something was downloaded, call handleDownload
 * - Battle/firewall effects already applied by Board
 */
void Game::handleMoveResult(MoveResult& result) {
    if (!result.header.success) {
        // illegal move: do nothing (Controller will print msg)
        return;
    }

    if (result.downloaded && result.downloadedLink) {
        handleDownload(*result.downloadedLink, result.downloaderId);
    }

    // result.battle / result.firewallTriggered can be used for logging/FX if you want
}

/**
 * Switch current player 1 ↔ 2.
 */
void Game::switchPlayer() {
    currentPlayer_ = (currentPlayer_ == 1) ? 2 : 1;
    abilityUsedThisTurn_ = false;  // Reset for new turn
}

/**
 * Check win condition:
 * - A player wins by downloading 4 data links and < 4 viruses
 * - A player loses (opponent wins) if they get 4 viruses and < 4 data
 * - If both >= 4 (weird), treat as loss for that player
 */
void Game::checkWinCondition() {
    for (int i = 1; i <= 2; ++i) {
        int dataCount = players_[i].getDownloadedData();
        int virusCount = players_[i].getDownloadedViruses();

        // win: 4+ data, fewer than 4 virus
        if (dataCount >= 4 && virusCount < 4) {
            winnerId_ = i;
            gameOver_ = true;
            return;
        }

        // lose: 4+ virus, fewer than 4 data
        if (virusCount >= 4 && dataCount < 4) {
            winnerId_ = 3 - i;  // opponent wins
            gameOver_ = true;
            return;
        }

        // both >= 4: treat as loss for i
        if (dataCount >= 4 && virusCount >= 4) {
            winnerId_ = 3 - i;  // opponent wins
            gameOver_ = true;
            return;
        }
    }

    winnerId_ = -1;
    gameOver_ = false;
}