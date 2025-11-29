export module controller;

import <iostream>;
import <fstream>;
import <sstream>;
import <string>;

import game;
import textdisplay;
import ability;
import util.direction;
import util.abilityParams;
import util.moveResult;
import util.abilityResult;

using std::istream;
using std::ostream;
using std::ifstream;
using std::istringstream;
using std::string;

export class Controller {
    Game& game_;
    TextDisplay& display_;
    istream& in_;
    ostream& out_;

    Direction parseDirection(const string& dir) {
        if (dir == "up") return Direction::Up;
        if (dir == "down") return Direction::Down;
        if (dir == "left") return Direction::Left;
        if (dir == "right") return Direction::Right;
        return Direction::Up;  // default
    }

    bool isValidDirection(const string& dir) {
        return dir == "up" || dir == "down" || dir == "left" || dir == "right";
    }

    char toLower(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
        return c;
    }

    char toUpper(char c) {
        if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
        return c;
    }

    // Flip direction for Player 2 (only when textFlip enabled, since board is flipped 180°)
    Direction adjustDirection(Direction dir, int playerId) {
        // Only flip if textFlip is enabled AND it's Player 2
        if (!game_.hasTextFlip() || playerId == 1) {
            return dir;  // No flip
        }
        // Player 2 with textFlip: flip directions (board is rotated 180°)
        switch (dir) {
            case Direction::Up:    return Direction::Down;
            case Direction::Down:  return Direction::Up;
            case Direction::Left:  return Direction::Right;
            case Direction::Right: return Direction::Left;
            default: return dir;
        }
    }

    void processMove(const string& linkIdStr, const string& dirStr) {
        if (linkIdStr.empty() || !isValidDirection(dirStr)) {
            out_ << "Invalid move command.\n";
            return;
        }

        char linkId = linkIdStr[0];
        int currentPlayer = game_.getCurrentPlayerIndex();
        
        Direction dir = parseDirection(dirStr);
        dir = adjustDirection(dir, currentPlayer);  // Flip for Player 2
        MoveResult result = game_.moveLink(linkId, dir);

        if (!result.header.success) {
            out_ << "Move failed: " << result.header.msg << "\n";
        } else {
            // Successful move - switch turns
            display_.printSplitLine();
            game_.endTurn();
            
            if (game_.isGameOver()) {
                display_.printWinner(game_.getWinnerId());
            } 
        }
    }

    void processAbility(int abilityIdx, const string& params) {
        AbilityParams userParams;
        AbilityParams opponentParams;
        userParams.targetLinkId = '\0';
        userParams.targetPos = {-1, -1};
        opponentParams.targetLinkId = '\0';
        opponentParams.targetPos = {-1, -1};

        // Parse ability parameters based on ability type
        // Some abilities need a link ID (Scan, Download, Polarize, LinkBoost, Ambush)
        // Some need a position (Firewall)
        // Some need two link IDs (Headshot, Exchange)
        if (!params.empty()) {
            istringstream iss(params);
            string param1, param2;
            iss >> param1 >> param2;
            
            // Check if this is Headshot or Exchange (need two link IDs)
            Ability* ability = game_.getAbility(game_.getCurrentPlayerIndex(), abilityIdx - 1);
            bool needsTwoLinks = false;
            if (ability) {
                char code = ability->getCode();
                if (code == 'H' || code == 'E') {
                    needsTwoLinks = true;
                }
            }
            
            if (needsTwoLinks && !param2.empty()) {
                // Two link IDs: first is user's, second is opponent's
                if ((param1[0] >= 'a' && param1[0] <= 'h') || 
                    (param1[0] >= 'A' && param1[0] <= 'H')) {
                    userParams.targetLinkId = param1[0];
                }
                if ((param2[0] >= 'a' && param2[0] <= 'h') || 
                    (param2[0] >= 'A' && param2[0] <= 'H')) {
                    opponentParams.targetLinkId = param2[0];
                }
            } else {
                // Single parameter: could be link ID or position
                if (params.length() == 1 && ((params[0] >= 'a' && params[0] <= 'h') || 
                                              (params[0] >= 'A' && params[0] <= 'H'))) {
                    userParams.targetLinkId = params[0];
                    opponentParams.targetLinkId = params[0];  // Same for most abilities
                } else {
                    // Try to parse as position (row col)
                    int row, col;
                    iss.clear();
                    iss.str(params);
                    if (iss >> row >> col) {
                        userParams.targetPos = {row, col};
                        opponentParams.targetPos = {row, col};
                    } else if (!params.empty()) {
                        // Could be a link ID
                        userParams.targetLinkId = params[0];
                        opponentParams.targetLinkId = params[0];
                    }
                }
            }
        }

        AbilityResult result = game_.useAbility(abilityIdx - 1, userParams, opponentParams);  // 1-indexed to 0-indexed

        if (!result.header.success) {
            out_ << "Ability failed: " << result.header.msg << "\n";
        } else if (result.used) {
            out_ << "Ability used successfully.\n";
            
            if (game_.isGameOver()) {
                display_.printWinner(game_.getWinnerId());
            }
        }
    }

    void processSequence(const string& filename) {
        ifstream file(filename);
        if (!file) {
            out_ << "Could not open file: " << filename << "\n";
            return;
        }

        string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            processCommand(line);
            if (game_.isGameOver()) break;
        }
    }

    void processCommand(const string& line) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') return;
        
        istringstream iss(line);
        string cmd;
        iss >> cmd;

        if (cmd == "move") {
            string linkId, dir;
            iss >> linkId >> dir;
            processMove(linkId, dir);
        } else if (cmd == "abilities") {
            display_.printAbilities(game_.getCurrentPlayerIndex());
        } else if (cmd == "ability") {
            int abilityIdx;
            if (iss >> abilityIdx) {
                // Rest of the line is parameters
                string params;
                std::getline(iss, params);
                // Trim leading whitespace
                size_t start = params.find_first_not_of(" \t");
                if (start != string::npos) {
                    params = params.substr(start);
                } else {
                    params = "";
                }
                processAbility(abilityIdx, params);
            } else {
                out_ << "Invalid ability command.\n";
            }
        } else if (cmd == "board") {
            display_.printBoard(game_.getCurrentPlayerIndex());
        } else if (cmd == "sequence") {
            string filename;
            iss >> filename;
            processSequence(filename);
        } else if (cmd == "quit") {
            // Will be handled in run()
        } else if (!cmd.empty()) {
            out_ << "Unknown command: " << cmd << "\n";
        }
    }

public:
    Controller(Game& game, TextDisplay& display, istream& in, ostream& out)
        : game_{game}, display_{display}, in_{in}, out_{out} {}

    void run() {
        // Initial board display
        display_.printBoard(game_.getCurrentPlayerIndex());

        string line;
        while (std::getline(in_, line)) {
            if (line.empty()) continue;
            
            istringstream iss(line);
            string cmd;
            iss >> cmd;

            if (cmd == "quit") {
                break;
            }

            processCommand(line);

            if (game_.isGameOver()) {
                break;
            }
        }
    }
};

