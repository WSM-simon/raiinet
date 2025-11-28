export module util.serverPort;

import <vector>;
import util.position;

export struct ServerPort {
    int ownerId;
    std::vector<Position> loc;
};