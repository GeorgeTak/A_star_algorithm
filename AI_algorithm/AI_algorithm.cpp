#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <tuple>  // For std::tie
#include <algorithm> // For std::reverse

using namespace std;

struct Cell {
    int x, y, cost, heuristic;
    bool operator>(const Cell& other) const { // Overloading operator > to compare the cells based on their total estimated cost
        return (cost + heuristic) > (other.cost + other.heuristic);
    }
};

const vector<pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }; // Directions down, up, right, left (as pairs of integers)

void generateMaze(vector<vector<char>>& maze, int n) {
    srand(time(NULL));
    int i, j;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if ((i == 0 && j == 0) || (i == n - 1 && j == n - 1)) {
                maze[i][j] = 'E'; // Mark exit points
            }
            else {
                int randNum = rand() % 10; //Pick a number between 0-9
                if (randNum < 2) maze[i][j] = 'X'; // 20% chance to be a block
                else if (randNum < 4) maze[i][j] = 'T'; // 20% chance to be a toll
                else maze[i][j] = '.';
            }
        }
    }
}

int heuristic(int x1, int y1, int x2, int y2) { // Manhattan distance between 2 points
    return abs(x1 - x2) + abs(y1 - y2);
}

vector<pair<int, int>> a_star(vector<vector<char>>& maze, int startX, int startY, int endX, int endY) {
    int n = maze.size();
    int i;
    vector<vector<int>> cost(n, vector<int>(n, INT_MAX));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1, -1 }));
    priority_queue<Cell, vector<Cell>, greater<Cell>> pq;

    pq.push({ startX, startY, 0, heuristic(startX, startY, endX, endY) });
    cost[startX][startY] = 0;

    const int dx[] = { 1, -1, 0, 0 }; // x directions: down, up, right, left
    const int dy[] = { 0, 0, 1, -1 }; // y directions: down, up, right, left

    while (!pq.empty()) {
        Cell current = pq.top();
        pq.pop();

        if (current.x == endX && current.y == endY) {
            vector<pair<int, int>> path;
            int x = endX, y = endY;
            while (x != -1 && y != -1) {
                path.push_back({ x, y });
                tie(x, y) = parent[x][y];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (i = 0; i < 4; ++i) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            if (newX >= 0 && newX < n && newY >= 0 && newY < n && maze[newX][newY] != 'X') {
                int newCost = current.cost + (maze[newX][newY] == 'T' ? 2 : 1);
                if (newCost < cost[newX][newY]) {
                    cost[newX][newY] = newCost;
                    parent[newX][newY] = { current.x, current.y };
                    pq.push({ newX, newY, newCost, heuristic(newX, newY, endX, endY) });
                }
            }
        }
    }
    return {}; // Path not found
}
void displayMaze(const vector<vector<char>>& maze) {
    int i, j;

    for (i = 0; i < maze.size(); ++i) {
        for (j = 0; j < maze[i].size(); ++j) {
            char cell = maze[i][j];
            switch (cell) {
            case 'X':
                cout << "\033[1;31m" << cell << "\033[0m "; // Red for blocks
                break;
            case 'T':
                cout << "\033[1;34m" << cell << "\033[0m "; // Blue for tolls
                break;
            case 'E':
            case 'R':
                cout << "\033[1;33m" << cell << "\033[0m "; // Yellow for exits and start
                break;
            default:
                cout << "\033[1;37m" << cell << "\033[0m "; // Grey for empty cells
                break;
            }
        }
        cout << endl;
    }
}


void displayPath(vector<vector<char>>& maze, const vector<pair<int, int>>& path, pair<int, int> start) {
    maze[start.first][start.second] = 'R'; // Mark start cell

    int i, j;

    for (i = 0; i < maze.size(); ++i) {
        for (j = 0; j < maze[0].size(); ++j) {
            bool isPath = find(path.begin(), path.end(), make_pair(i, j)) != path.end();
            if (isPath) {
                cout << "\033[1;32m" << maze[i][j] << "\033[0m "; // Green for path
            }
            else {
                switch (maze[i][j]) {
                case 'X':
                    cout << "\033[1;31m" << maze[i][j] << "\033[0m "; // Red for blocks
                    break;
                case 'T':
                    cout << "\033[1;34m" << maze[i][j] << "\033[0m "; // Blue for tolls
                    break;
                case 'E':
                case 'R':
                    cout << "\033[1;33m" << maze[i][j] << "\033[0m "; // Yellow for exits and start
                    break;
                default:
                    cout << "\033[1;37m" << maze[i][j] << "\033[0m "; // Grey for empty cells
                    break;
                }
            }
        }
        cout << endl;
    }
}

int main() {
    int n;
    cout << "Enter the size of the maze (n x n): ";
    cin >> n;

    vector<vector<char>> maze(n, vector<char>(n));
    generateMaze(maze, n);

    vector<pair<int, int>> startPoints = { {n / 2, n / 2}, {n / 2 + 1, n / 2}, {n / 2, n / 2 + 1}, {n / 2 + 1, n / 2 + 1} };
    pair<int, int> start = startPoints[rand() % startPoints.size()];

    cout << "Generated Maze: " << endl;
    displayMaze(maze);

    cout << "Starting at: (" << start.first << ", " << start.second << ")" << endl;

    vector<pair<int, int>> pathToExit1 = a_star(maze, start.first, start.second, 0, 0);
    vector<pair<int, int>> pathToExit2 = a_star(maze, start.first, start.second, n - 1, n - 1);

    vector<pair<int, int>> bestPath;
    if (!pathToExit1.empty() && !pathToExit2.empty()) {
        bestPath = (pathToExit1.size() <= pathToExit2.size()) ? pathToExit1 : pathToExit2;
    }
    else if (!pathToExit1.empty()) {
        bestPath = pathToExit1;
    }
    else if (!pathToExit2.empty()) {
        bestPath = pathToExit2;
    }

    if (!bestPath.empty()) {
        cout << "Minimum cost path found." << endl;
        cout << "Path:" << endl;
        displayPath(maze, bestPath, start);
    }
    else {
        cout << "No path to an exit found." << endl;
    }

    return 0;
}

