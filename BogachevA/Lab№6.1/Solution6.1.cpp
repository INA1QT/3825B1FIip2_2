#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
const int boardSize = 10;
const char rowLabels[] = { 'A','B','C','D','E','F','G','H','I','J' };
struct Coord {
    int x, y;
    Coord(int x_ = -1, int y_ = -1) : x(x_), y(y_) {}
};
class Ship {
protected:
    vector<int> xs, ys;
    bool hits[4];
    int shipSize;
public:
    Ship(int size) : shipSize(size) {
        for (int i = 0; i < 4; ++i) {
            hits[i] = false;
        }
    }
    virtual ~Ship() {}
    virtual int getSize() const {
        return shipSize;
    }
    void setCoords(const vector<int>& xs_, const vector<int>& ys_) {
        xs = xs_;
        ys = ys_;
    }
    bool hit(int x, int y) {
        for (int i = 0; i < shipSize; ++i) {
            if (xs[i] == x && ys[i] == y && !hits[i]) {
                hits[i] = true;
                return true;
            }
        }
        return false;
    }
    bool isSunk() const {
        for (int i = 0; i < shipSize; ++i) {
            if (!hits[i]) {
                return false;
            }
        }
        return true;
    }
    bool contains(int x, int y) const {
        for (int i = 0; i < shipSize; ++i) {
            if (xs[i] == x && ys[i] == y) {
                return true;
            }
        }
        return false;
    }
    void getCoords(vector<int>& outX, vector<int>& outY) const {
        outX = xs;
        outY = ys;
    }
};
class Launch : public Ship {
public:
    Launch() : Ship(1) {}
};
class Boat : public Ship {
public:
    Boat() : Ship(2) {}
};
class Cruiser : public Ship {
public:
    Cruiser() : Ship(3) {}
};
class AircraftCarrier : public Ship {
public:
    AircraftCarrier() : Ship(4) {}
};
class Board {
private:
    vector<vector<Ship*>> grid;
    vector<vector<bool>> hitMarkers;
    vector<Ship*> ships;
public:
    Board() {
        for (int i = 0; i < boardSize; ++i) {
            vector<Ship*> row;
            for (int j = 0; j < boardSize; ++j) {
                row.push_back(nullptr);
            }
            grid.push_back(row);
        }
        for (int i = 0; i < boardSize; ++i) {
            vector<bool> row;
            for (int j = 0; j < boardSize; ++j) {
                row.push_back(false);
            }
            hitMarkers.push_back(row);
        }
    }
    ~Board() {
        for (Ship* s : ships) {
            delete s;
        }
    }
    bool canPlace(Ship* ship, int x, int y, bool horizontal) const {
        int size = ship->getSize();
        if (horizontal) {
            if (y + size > boardSize) {
                return false;
            }
        }
        else {
            if (x + size > boardSize) {
                return false;
            }
        }
        for (int i = 0; i < size; ++i) {
            int cx = x + (horizontal ? 0 : i);
            int cy = y + (horizontal ? i : 0);
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize) {
                        if (grid[nx][ny] != nullptr) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    bool placeShip(Ship* ship, int x, int y, bool horizontal) {
        if (!canPlace(ship, x, y, horizontal)) {
            return false;
        }
        int size = ship->getSize();
        vector<int> xs, ys;
        for (int i = 0; i < size; ++i) {
            int cx = x + (horizontal ? 0 : i);
            int cy = y + (horizontal ? i : 0);
            grid[cx][cy] = ship;
            xs.push_back(cx);
            ys.push_back(cy);
        }
        ship->setCoords(xs, ys);
        ships.push_back(ship);
        return true;
    }
    struct AttackResult {
        bool hit, sunk;
        Ship* sunkShip;
    };
    AttackResult receiveAttack(int x, int y) {
        AttackResult res = { false, false, nullptr };
        if (hitMarkers[x][y]) {
            return res;
        }
        hitMarkers[x][y] = true;
        Ship* s = grid[x][y];
        if (s != nullptr) {
            res.hit = true;
            s->hit(x, y);
            if (s->isSunk()) {
                res.sunk = true;
                res.sunkShip = s;
            }
        }
        return res;
    }
    bool allSunk() const {
        for (Ship* s : ships) {
            if (!s->isSunk()) {
                return false;
            }
        }
        return true;
    }
    void print() const {
        cout << "  ";
        for (int i = 1; i <= boardSize; ++i) {
            cout << i << " ";
        }
        cout << endl;
        for (int i = 0; i < boardSize; ++i) {
            cout << rowLabels[i] << " ";
            for (int j = 0; j < boardSize; ++j) {
                if (hitMarkers[i][j]) {
                    cout << (grid[i][j] ? 'X' : 'O') << ' ';
                }
                else {
                    cout << (grid[i][j] ? '#' : '.') << ' ';
                }
            }
            cout << endl;
        }
    }
    Ship* getShipAt(int x, int y) const {
        return grid[x][y];
    }
};
class TrackingBoard {
private:
    vector<vector<char>> cells;
public:
    TrackingBoard() {
        for (int i = 0; i < boardSize; ++i) {
            vector<char> row;
            for (int j = 0; j < boardSize; ++j) {
                row.push_back('.');   
            }
            cells.push_back(row);
        }
    }
    bool isAttacked(int x, int y) const {
        return cells[x][y] != '.';   
    }
    void record(int x, int y, bool hit, bool sunk = false) {
        if (sunk) {
            cells[x][y] = 'S';
        }
        else if (hit) {
            cells[x][y] = 'H';
        }
        else {
            cells[x][y] = 'M';
        }
    }
    void markSunk(Ship* ship) {
        if (!ship) {
            return;
        }
        vector<int> xs, ys;
        ship->getCoords(xs, ys);
        for (size_t i = 0; i < xs.size(); ++i) {
            cells[xs[i]][ys[i]] = 'S';
        }
    }
    void print() const {
        cout << "  ";
        for (int i = 1; i <= boardSize; ++i) {
            cout << i << " ";
        }
        cout << endl;
        for (int i = 0; i < boardSize; ++i) {
            cout << rowLabels[i] << " ";
            for (int j = 0; j < boardSize; ++j) {
                cout << cells[i][j] << " ";
            }
            cout << endl;
        }
    }
};
class Player {
protected:
    string name;
public:
    Player(const string& n) : name(n) {}
    virtual ~Player() {}
    virtual void placeShips(Board& board) = 0;
    virtual Coord getMove(const TrackingBoard& enemyView) = 0;
    virtual void resultNotify(int, int, bool, bool, Ship*) {}       
    string getName() const {
        return name;
    }
};
class HumanPlayer : public Player {
    bool parse(const string& inp, int& x, int& y) const {
        if (inp.length() < 2 || inp.length() > 3) {
            return false;
        }
        char r = inp[0];
        int row = -1;
        for (int i = 0; i < boardSize; ++i) {
            if (rowLabels[i] == r) {
                row = i;
                break;
            }
        }
        if (row == -1) {
            return false;
        }
        string numPart = inp.substr(1);
        for (char ch : numPart) {
            if (!isdigit(ch)) {
                return false;
            }
        }
        int col;
        try {
            col = stoi(numPart) - 1;
        }
        catch (...) {
            return false;
        }
        if (col < 0 || col >= boardSize) {
            return false;
        }
        x = row;
        y = col;
        return true;
    }
public:
    HumanPlayer(const string& n) : Player(n) {}
    void placeShips(Board& board) override {
        int sizes[] = { 4,3,3,2,2,2,1,1,1,1 };
        cout << "Arrange your ships! Example (A1 H or A1 V), H - horizontal, V - vertical, letters must be UPPERCASE!" << endl;
        cout << "For 1-deck ships, direction is not needed" << endl;
        board.print();
        for (int sz : sizes) {
            Ship* ship = nullptr;
            if (sz == 1) {
                ship = new Launch();
            }
            else if (sz == 2) {
                ship = new Boat();
            }
            else if (sz == 3) {
                ship = new Cruiser();
            }
            else {
                ship = new AircraftCarrier();
            }
            bool placed = false;
            while (!placed) {
                string coord;
                cout << "Place " << sz << "-deck ship: ";
                if (sz == 1) {
                    cin >> coord;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    int x, y;
                    if (!parse(coord, x, y)) {
                        cout << "Invalid coordinates. Use A-J and 1-10, UPPERCASE letters" << endl;
                        continue;
                    }
                    if (board.placeShip(ship, x, y, true)) {
                        placed = true;
                        cout << "Ship placed" << endl;
                        board.print();
                    }
                    else {
                        cout << "Cannot place here (touching another ship or out of bounds)" << endl;
                    }
                }
                else {
                    string dir;
                    cin >> coord >> dir;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    int x, y;
                    if (!parse(coord, x, y)) {
                        cout << "Invalid coordinates. Use A-J and 1-10, UPPERCASE letters" << endl;
                        continue;
                    }
                    if (!(dir == "H" || dir == "h" || dir == "V" || dir == "v")) {
                        cout << "Invalid direction. Use H (horizontal) or V (vertical)" << endl;
                        continue;
                    }
                    bool horiz = (dir == "H" || dir == "h");
                    if (board.placeShip(ship, x, y, horiz)) {
                        placed = true;
                        cout << "Ship placed." << endl;
                        board.print();
                    }
                    else {
                        cout << "Cannot place here (touching another ship or out of bounds)" << endl;
                    }
                }
            }
        }
    }
    Coord getMove(const TrackingBoard& view) override {
        string inp;
        cout << name << ", its your turn: ";
        cin >> inp;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        int x, y;
        while (!parse(inp, x, y) || view.isAttacked(x, y)) {
            if (!parse(inp, x, y)) {
                cout << "Invalid format. Enter uppercase letter A-J and number 1-10" << endl;
            }
            else {
                cout << "You have already shot there. Choose another cell" << endl;
            }
            cout << name << ", its your turn: ";
            cin >> inp;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return Coord(x, y);
    }
};
class ComputerPlayer : public Player {
    vector<Coord> getUnknown(const TrackingBoard& view) const {
        vector<Coord> v;
        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                if (!view.isAttacked(i, j)) {
                    v.push_back(Coord(i, j));
                }
            }
        }
        return v;
    }
public:
    ComputerPlayer(const string& n) : Player(n) {
        srand(time(nullptr));
    }
    void placeShips(Board& board) override {
        int sizes[] = { 4,3,3,2,2,2,1,1,1,1 };
        for (int sz : sizes) {
            bool placed = false;
            while (!placed) {
                Ship* ship = nullptr;
                if (sz == 1) {
                    ship = new Launch();
                }
                else if (sz == 2) {
                    ship = new Boat();
                }
                else if (sz == 3) {
                    ship = new Cruiser();
                }
                else {
                    ship = new AircraftCarrier();
                }
                int x = rand() % boardSize;
                int y = rand() % boardSize;
                bool horiz = (rand() % 2 == 0);
                if (board.placeShip(ship, x, y, horiz)) {
                    placed = true;
                }
                else {
                    delete ship;
                }
            }
        }
    }
    Coord getMove(const TrackingBoard& view) override {
        vector<Coord> unknown = getUnknown(view);
        if (unknown.empty()) {
            return Coord(-1, -1);
        }
        int idx = rand() % unknown.size();
        return unknown[idx];
    }
};
class Game {
private:
    Player* player1;
    Player* player2;    
    Board board1, board2;
    TrackingBoard track1, track2;
    bool gameOver;
    void processTurn(Player* attacker, Player* defender, Board& defBoard, TrackingBoard& track, bool& turnContinues) {
        Coord c = attacker->getMove(track);
        if (c.x == -1) {
            return;
        }
        Board::AttackResult res = defBoard.receiveAttack(c.x, c.y);
        attacker->resultNotify(c.x, c.y, res.hit, res.sunk, res.sunkShip);
        track.record(c.x, c.y, res.hit, res.sunk);
        if (res.sunk && res.sunkShip) {
            track.markSunk(res.sunkShip);
        }
        cout << attacker->getName() << " shoots at " << rowLabels[c.x] << (c.y + 1) << ": ";
        if (res.hit) {
            cout << "Hit! ";
            if (res.sunk) {
                cout << "Ship sunk!";
            }
            cout << endl;
        }
        else {
            cout << "Miss." << endl;
        }
        if (attacker->getName() == "Human") {
            cout << "Your shots against the enemy: " << endl;
            track.print();
        }
        if (defBoard.allSunk()) {
            cout << endl;
            cout << attacker->getName() << " is winner!" << endl;
            gameOver = true;
        }
        turnContinues = res.hit;
    }
public:
    Game(Player* p1, Player* p2) : player1(p1), player2(p2), gameOver(false) {}
    void run() {
        cout << "Welcome to the BATTLESHIP" << endl;
        cout << endl;
        player1->placeShips(board1);
        cout << endl;
        player2->placeShips(board2);
        cout << endl;
        bool player1Turn = true;
        while (!gameOver) {
            bool continueTurn = false;
            if (player1Turn) {
                processTurn(player1, player2, board2, track1, continueTurn);
                if (!gameOver && !continueTurn) {
                    player1Turn = false;
                }
            }
            else {
                processTurn(player2, player1, board1, track2, continueTurn);
                if (!gameOver && !continueTurn) {
                    player1Turn = true;
                }
            }
        }
    }
};
int main() {
    Player* human = new HumanPlayer("Human");
    Player* computer = new ComputerPlayer("Computer");
    Game game(human, computer);
    game.run();
    delete human;
    delete computer;
    return 0;
}