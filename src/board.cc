export module board;

import <vector>;

import cell;
import link;
import util.position;
import util.direction;
import util.serverport;
import util.moveresult;
import util.firewallinfo;

using std::vector;

export class Board {
    int ROWS_ = 8;
    int COLS_ = 8;

    Cell grid_[ROWS_][COLS_];
    vector<ServerPort> serverPorts_;     // one per player
    vector<FirewallInfo> firewalls_;     // storage for firewall objects
}