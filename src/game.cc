export module game;

import <string>;
import <vector>;
import <memory>;

import subject;              // Game inherits from Subject for observer pattern
import board;                // Board class
import player;               // Player class
import link;                 // Link class
import util.direction;       // Direction enum
import util.abilityParams;   // AbilityParams struct
import util.moveResult;      // MoveResult struct
import util.abilityResult;   // AbilityResult struct
import util.playerSetup;     // PlayerSetup struct
import ability;              // Ability class

/**
 * Game class - Coordinates the game state and player actions
 * 
 * This class manages:
 * - The game board
 * - Two players and their abilities (stored in Game)
 * - Current player turn
 * - Win condition checking
 * - Link movement and downloads
 * - Ability usage
 * 
 * Game inherits from Subject to notify observers (displays) of state changes
 */
export class Game : public Subject {
    Board board_;                                          // The game board
    Player players_[3];                                    // Players at index 1 and 2 (index 0 unused)
    std::vector<std::unique_ptr<Ability>> abilities_[3];   // Abilities for each player (index 0 unused)
    int currentPlayer_ = 1;                                // Current player (1 or 2)
    int winnerId_ = -1;                                    // -1 = no winner yet, 1 or 2 = winner
    bool gameOver_ = false;
    bool useGraphics_ = true;
    bool extraFeatures_ = false;                           // Extra features (colors, split lines)
    bool enableTextFlip_ = false;                          // Enable board flipping in text display
    bool abilityUsedThisTurn_ = false;                     // Can only use 1 ability per turn

    void handleDownload(Link& link, int downloaderId);
    void handleMoveResult(MoveResult& result);
    void switchPlayer();
    void checkWinCondition();

public:
    Game(bool useGraphics = true, bool extraFeatures = false, bool enableTextFlip = false);

    void initializeGame(PlayerSetup& p0, PlayerSetup& p1);
    
    bool hasExtraFeatures() const { return extraFeatures_; }
    bool hasTextFlip() const { return enableTextFlip_; }

    Board& getBoard() { return board_; }

    Player& getPlayer(int idx) { return players_[idx]; }

    Player& getCurrentPlayer() { return players_[currentPlayer_]; }

    int getCurrentPlayerIndex() const { return currentPlayer_; }

    // Ability access - abilities stored in Game, not Player
    Ability* getAbility(int playerIdx, int abilityIdx);
    std::vector<std::unique_ptr<Ability>>& getAbilities(int playerIdx) { return abilities_[playerIdx]; }

    MoveResult moveLink(char linkId, Direction dir);

    AbilityResult useAbility(int abilityIndex, AbilityParams& userParams, AbilityParams& opponentParams);

    void endTurn();

    void reset();

    bool isGameOver();

    int getWinnerId();
};

