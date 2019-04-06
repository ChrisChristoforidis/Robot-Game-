#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <map>

using namespace std;
#ifdef __LINUX__
#include "colormod.h"
Color::Modifier red(Color::FG_RED);
Color::Modifier blue(Color::FG_BLUE);
Color::Modifier green(Color::FG_GREEN);
Color::Modifier def(Color::FG_DEFAULT);
#elif _WIN32
const char *red = "";
const char *blue = "";
const char *green = "";
const char *def = "";
#endif
enum direction
{
    North,
    South,
    West,
    East
};
std::ostream &operator<<(std::ostream &out, const direction value)
{
    static std::map<direction, std::string> strings;
    if (strings.size() == 0)
    {
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(North);
        INSERT_ELEMENT(South);
        INSERT_ELEMENT(West);
        INSERT_ELEMENT(East);
#undef INSERT_ELEMENT
    }
    return out << strings[value];
}
//Declariotion of variables;
int EnemyNum; //number of enemy robots
const int grid_y = 25;
const int grid_x = 25;
char grid[grid_y][grid_x];
bool flag1 = true;
bool flag2 = true;
const char *command[] = {"move forward", "move backwards", "turn left", "turn right"};
pair<int, int> getGoal();
int y_goal;
int x_goal;

//Definition of robot and enemy robot classes
class Robot
{
  protected:
    int x_last = 24;
    int y_last = 24;

  public:
    int x_;
    int y_;
    direction theta_;

  public:
    Robot(int x, int y) : x_(x),
                          y_(y),
                          theta_(static_cast<direction>(rand() % 4))
    {
    }
    Robot() : x_(rand() % 16 + 5),
              y_(rand() % 8 + 6),
              theta_(static_cast<direction>(rand() % 4))
    {
    }

    void moveForward()
    {
        y_last = y_;
        x_last = x_;
        if (theta_ == North)
            y_--;
        else if (theta_ == South)
            y_++;
        else if (theta_ == West)
            x_--;
        else if (theta_ == East)
            x_++;
    }
    void moveBackward()
    {
        y_last = y_;
        x_last = x_;
        if (theta_ == North)
            y_++;
        else if (theta_ == South)
            y_--;
        else if (theta_ == West)
            x_++;
        else if (theta_ == East)
            x_--;
    }
    void turnLeft()
    {
        if (theta_ == North)
        {
            theta_ = West;
        }
        else if (theta_ == South)
        {
            theta_ = East;
        }
        else if (theta_ == West)
        {
            theta_ = South;
        }
        else if (theta_ == East)
        {
            theta_ = North;
        }
    }
    void turnRight()
    {
        if (theta_ == North)
        {
            theta_ = East;
        }
        else if (theta_ == South)
        {
            theta_ = West;
        }
        else if (theta_ == West)
        {
            theta_ = North;
        }
        else if (theta_ == East)
        {
            theta_ = South;
        }
    }
    void updateGrid()
    {
        grid[y_last][x_last] = 'O';
        grid[y_][x_] = 'B';
    }
    void printState()
    {
        std::cout << "\n"
                  << blue << "Bob " << def << " is facing :" << green << theta_ << def << "\n"
                  << std::endl;
    }
    pair<int, int> getState()
    {
        pair<int, int> state(x_, y_);
        return state;
    }
};
class enemyRobot : public Robot
{

  public:
    int dist_x;
    int dist_y;

  public:
    enemyRobot() {}

    void makeMove(Robot &bob)
    {
        //First read the distance from the enemy position
        dist_x = bob.x_ - x_;
        dist_y = bob.y_ - y_;

        if ((dist_x > 0))
        {
            if (theta_ == East)
            {

                moveForward();
                goto endloop;
            }
            else if (theta_ == West)
            {
                moveBackward();
                goto endloop;
            }
        }
        else if (dist_x < 0)
        {
            if (theta_ == East)
            {
                moveBackward();
                goto endloop;
            }
            else if (theta_ == West)
            {
                moveForward();
                goto endloop;
            }
        }

        if ((dist_y > 0))
        {
            if (theta_ == North)
            {
                moveBackward();
                goto endloop;
            }
            else if (theta_ == South)
            {
                moveForward();
                goto endloop;
            }
        }
        else if (dist_y < 0)
        {
            if (theta_ == North)
            {
                moveForward();
                goto endloop;
            }
            else if (theta_ == South)
            {
                moveBackward();

                goto endloop;
            }
        }

        turnLeft();
        cout << "Enemy Robot made a turn left." << endl;

    endloop:
        dist_x = bob.x_ - x_;
        dist_y = bob.y_ - y_;
    }
    void updateGrid(char grid[grid_y][grid_x])
    {
        grid[y_last][x_last] = 'O';
        grid[y_][x_] = 'H';
    }
};

bool winCondition(pair<int, int> &, Robot &);
enemyRobot *Enemies[10]; //array storing pointers to enemy robot  objects

void printGrid()
{
    for (char k = 'a'; k <= 'w'; k++)
    {

        if (k == 'w')
        {
            std::cout << k << std::endl;
        }
        else if (k == 'a')
        {
            std::cout << "  a"
                      << ".";
        }
        else
        {
            std::cout << k << ".";
        }
    }
    char z = 'A';
    for (int i = 0; i <= 16; i++)
    {

        std::cout << z << " ";
        z++;
        for (int j = 0; j <= 22; j++)
        {
            if (grid[i][j] == 'B')
                std::cout << blue << grid[i][j] << " " << def;
            else if (grid[i][j] == 'H')
                std::cout << red << grid[i][j] << " " << def;

            else if (grid[i][j] == 'X')
                std::cout << green << grid[i][j] << " " << def;
            else
                std::cout << grid[i][j] << " ";
        }

        std::cout << std::endl;
    }
}

void inputCommand(Robot &pbob)
{
    cout << "Write your command (\"move forward\",\"move backwards\",\"turn left\",\"turn right\")" << endl;
    string input;

    for (bool exit_flag = true; exit_flag;)
    {
        getline(std::cin, input);
        if (input == command[0])
        {
            pbob.moveForward();
            exit_flag = false;
        }
        else if (input == command[1])
        {
            pbob.moveBackward();
            exit_flag = false;
        }
        else if (input == command[2])
        {
            pbob.turnLeft();
            exit_flag = false;
        }
        else if (input == command[3])
        {
            pbob.turnRight();
            exit_flag = false;
        }
        else
        {
            cout << "Your input : " << input << std::endl;
            cout << "Please provide valid input command."
                 << endl;
        }
    }
    pbob.updateGrid();
}
bool gameOver()
{
    for (int i = 0; i < EnemyNum; i++)
    {
        if ((Enemies[i]->dist_x == 0) && (Enemies[i]->dist_y == 0))
        {
            return false;
        }
    }
    return true;
}
int main()
{

    srand(time(NULL)); // get seed number from cpu clock to get true pseudo random  sequence.

    //Initialize grid
    for (int i = 0; i <= grid_y; i++)
    {
        for (int j = 0; j <= grid_x; j++)
        {
            grid[i][j] = 'O';
        }
    }

    pair<int, int> goal = getGoal();

    //Initialize user robot;
    Robot bob(11, 8);
    bob.updateGrid();
    //Input the number of enemy robots;

    std::cout << "How many robots do you want to face? (up to 10)" << std::endl;
    std::cin >> EnemyNum;
    string input;
    std::getline(std::cin, input); // just to get rid of /n so the next getline command works.
    // Initialize enemy agents.(as pointers to enemyRobot objects in a normal array)
    for (int i = 0; i < EnemyNum; i++)
    {
        enemyRobot *e1 = new enemyRobot;

        Enemies[i] = e1;
        Enemies[i]->updateGrid(grid);
    }
    // Printing the initial board state.
    std::cout << "\n"
              << "You are" << blue << " Bob " << def << "the robot , try to go to the " << green << "X" << def << " spot on \
the grid without getting caught by the enemy robots. The game is turn based , \
you can only move forwards-backwards each turn ,depending on your orientation. Be carefull , \
changing orientation counts as a turn. Enemies  have the same capabilities as Bob and take their turns sequencially after yours."
              << std::endl;
    std::cout << "Good Luck!!!" << '\n'
              << std::endl;
    while (flag1 && flag2)
    {
        vector<pair<int, int>> positions;

        printGrid();
        //Player turn
        bob.printState();
        inputCommand(bob);

        //Enemy Robot Turn
        for (int i = 0; i < EnemyNum; i++)
        {
            Enemies[i]->makeMove(bob);
            Enemies[i]->updateGrid(grid);
        }

        flag1 = gameOver();
        flag2 = winCondition(goal, bob);
    }
    for (int i = 0; i < EnemyNum; i++)
    {
        delete Enemies[i];
    }
    if (!flag1)
        std::cout << "\n"
                  << red << "GAME OVER !!!!!" << def << std::endl;

    else if (!flag2)
        std::cout << "\n"
                  << green << "YOU WIN !!!!!" << def << std::endl;

    return 0;
}

pair<int, int> getGoal()
{
    int goal_side = rand() % 4;
    int goal_squre = rand() % 17;
    pair<int, int> goal;
    if (goal_side == 0)
    {
        grid[0][goal_squre] = 'X';
        y_goal = 0;
        x_goal = goal_squre;
    }
    else if (goal_side == 1)
    {
        grid[16][goal_squre] = 'X';
        y_goal = 16;
        x_goal = goal_squre;
    }
    else if (goal_side == 3)
    {
        grid[goal_squre][0] = 'X';
        y_goal = goal_squre;
        x_goal = 0;
    }
    else
    {
        grid[goal_squre][22] = 'X';
        y_goal = goal_squre;
        x_goal = 22;
    }
    goal.first = x_goal;
    goal.second = y_goal;
    return goal;
}
bool winCondition(pair<int, int> &vector, Robot &bob)
{
    bool result = true;
    if (!(bob.x_ - vector.first) && !(bob.y_ - vector.second))
        result = false;

    return result;
}