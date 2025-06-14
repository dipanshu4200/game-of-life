#include <raylib.h>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;
int offset = 70;
Color grey = { 29,29,29,255 };  

double lastupdate = 0;
bool update(double interval) {
	double current = GetTime();
	if (current - lastupdate >= interval) {
		lastupdate = current;
		return true;
	}return false;

}

class Grid {
private:
	int rows, columns, cellsize;
	vector<vector<int>> cells;
public:
	// this is a constructor in which at start every grid is dead or assigned as 0
	Grid(int width, int height, int cellsize)
		: rows((height / cellsize)), columns((width / cellsize)), cellsize(cellsize), cells(rows, vector<int>(columns, 0)) {
	};
	void Draw() {
		for (int row = 0;row < rows;row++) {
			for (int column = 0;column < columns;column++) {
				Color color = cells[row][column] ? Color{ 0,255,0,255 } : Color{ 55,55,55,255 };
				DrawRectangle(column * cellsize, row * cellsize, cellsize, cellsize, color);
			}
		}
	}
	void fillrandom() {
		for (int row = 0;row < rows;row++) {
			for (int column = 0;column < columns;column++) {
				int randoms = GetRandomValue(0,4);
	
				cells[row][column] = (randoms == 4) ? 1 : 0; // this is to get low 1 and more 0
				// as for better simulation we need 20% live cell and around 80% dead cell
			}
		}
	}
	void setvalue(int row, int column, int value) { // in this we assigne value of grid with 1 to make it alive
		if (checkgrid(row,column)) {
			cells[row][column] = value;
		}
	}
	int getvalues(int row,int column) {
		if (checkgrid(row, column)) {
			return cells[row][column];
		}return 0;
	}
	int getrows() {
		return rows;
	}
	int getcolumn() {
		return columns;
	}
	
	void clear() {
		for (int i = 0;i < rows;i++) {
			for (int j = 0;j < columns;j++) {
				cells[i][j] = 0;
			}
		}
	}

	bool checkgrid(int row, int column) {
	if (row >= 0 && row < rows && column >= 0 && column < columns) {
		return true;
	}return false;
}
};

class Simulation {
public :
	Simulation(int width, int height, int cellsize)
		: grid(width, height, cellsize), tempgrid(width, height, cellsize),run(true) {
		grid.fillrandom();
	};

	void Draw() {
		grid.Draw();
	}
	bool isrunning() { return run; }
	void start() { run = true; }
	void end() { run = false; }
	void update() {
		if (isrunning()) {
			for (int row = 0;row < grid.getrows();row++) {
				for (int column = 0;column < grid.getcolumn();column++) {
					int liveneighbors = countlive(row, column); // this to get all live neigbours 
					int cellvalue = grid.getvalues(row, column); // this is to get value of current grid

					if (cellvalue == 1) { // if cell is alive or 1

						if (liveneighbors > 3 || liveneighbors < 2) {// 2 rules of game with same logic
							tempgrid.setvalue(row, column, 0); // we'll just delete the life
						}
						else {
							tempgrid.setvalue(row, column, 1);
						}
					}
					else { // if cell is dead or 0
						if (liveneighbors == 3) {// 3 rule to repopulate
							tempgrid.setvalue(row, column, 1);
						}
						else tempgrid.setvalue(row, column, 0); // last rule 
					}
				}
			}
			grid = tempgrid;
		}
	}
	int countlive(int row, int column) { // func to check count of live neighbour surrounding our give grid
		int liveneighbour = 0;
		vector<pair<int, int>> neighbouroff = {
			{-1,0}, // directely above cell
			{1,0},  // directly below
			{0,-1}, // to left
			{0,1},  // to right
			{-1,-1}, // diagonol  upper left
			{-1,1},  // diagonol  upper rigt
			{1,-1},
			{1,1}
		};
		for (const auto& offset : neighbouroff) {
			int neighrow = (row + offset.first + grid.getrows()) % grid.getrows(); // this is ti ensure a continume in border grid
			int neighcolumn = (column + offset.second + grid.getcolumn()) % grid.getcolumn();
			liveneighbour += grid.getvalues(neighrow, neighcolumn); // this is to check no of neighbour 
		}
		return liveneighbour;
	}
	void clear() {
		grid.clear();
	}
	void setvalue(int row, int column, int value) {
		grid.setvalue(row, column, value);
	}
	void toggle(int row, int column) {
		if (!isrunning()) {
			grid.setvalue(row, column, 1);
		}
	}

private:
	Grid grid;
	Grid tempgrid;
	bool run;
};


int main() {
	const int width = 800;
	const int height = 800;
	const int cell = 10;
	int fps = 10;
	InitWindow(width, height, "hello world");
	SetTargetFPS(fps);

	Simulation simulation(width, height, cell);

	Grid g(width,height,cell);
	while (!WindowShouldClose()) {
		simulation.update();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mousepos = GetMousePosition();
			int row = mousepos.y/cell;
			int column= mousepos.x/cell;
			simulation.toggle(row, column);
		}
		if (IsKeyPressed(KEY_ENTER)) {
			simulation.start();
			SetWindowTitle("SIMULATION HAS BEGUN");
		}
		else if (IsKeyPressed(KEY_SPACE)) {
			simulation.end();
			SetWindowTitle("SIMULATION HAS STOPED");
		}
		if (IsKeyPressed(KEY_RIGHT)) {
			if (fps < 30) {
				fps += 2;
				SetTargetFPS(fps);
			}
		}
		else if (IsKeyPressed(KEY_LEFT)) {
			if (fps > 5) {
				fps -= 2;
				SetTargetFPS(fps);
			}
		}
		else if (IsKeyPressed(KEY_RIGHT_SHIFT)) {
			simulation.clear();
		}

		BeginDrawing();
		ClearBackground(grey);
		simulation.Draw();
		EndDrawing();

	}
	CloseWindow();

	return 0;
}