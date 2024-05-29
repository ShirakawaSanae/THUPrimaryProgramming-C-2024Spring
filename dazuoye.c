#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <conio.h>
// 常量定义
//#define WIDTH 10
//#define HEIGHT 10
#define MAXN 50
#define WALL 16843009 // memset的特性
// 结构体定义
typedef struct findNode{
    int x;
    int y;
    //int px;
    //int py; // 记录前驱点
    int index; 
}node;
// 全局变量
int HEIGHT, WIDTH;
int maze[MAXN + 2][MAXN + 2];
int visited[MAXN + 2][MAXN + 2];
int path[MAXN + 2][MAXN + 2];
node queue[MAXN*MAXN*2];
int vis[MAXN + 2][MAXN + 2];
int dir_x[] = {0, 2, 0, -2};
int dir_y[] = {-2, 0, 2, 0}; 
int player_x, player_y;
int St_x = 1, St_y = 1, Ed_x, Ed_y;// 起点终点
int flag = 0;
char block = 219; // 方块字符的ASCII
// 函数声明
void createMaze(int h, int w);
void printMaze();
int isSafe(int x, int y);
void movePlayer(char dir);
void invalidInput();
void game();
void victory();
void createPath(int x, int y);
int isWall(int x, int y);
void findWay(int x, int y);

int isWall(int x, int y){ // 注意迷宫最外围一圈也是墙
    return (x >= 0 && x <= WIDTH + 1 && y >= 0 && y <= HEIGHT + 1);
}

void createPath(int x, int y){ // 破墙法生成路径保证 S E 联通
    //srand(time(NULL));
    int i = rand() % 3 + 1;
    int tmp1 = dir_x[0], tmp2 = dir_y[0]; // 方向随机保证不会一直是上下左右
    dir_x[0] = dir_x[i];
    dir_y[0] = dir_y[i];
    dir_x[i] = tmp1;
    dir_y[i] = tmp2;

    for(int dir = 0; dir < 4; dir++){
        int tx = x + dir_x[dir];
        int ty = y + dir_y[dir];
        if(!isWall(tx, ty)) continue; // 跨度为2，保证不会出现大片的 Path块
        tx = x + dir_x[dir]/2; // 打通中间的墙
        ty = y + dir_y[dir]/2;
        if(visited[ty][tx]) continue;
        int count = 0;
        for(int j = tx - 1; j < tx + 2; j++){ // 但是不要让两条路之间的墙被挖穿
            for(int k = ty - 1; k < ty + 2; k++){
                if(abs(j - tx) + abs(k - ty) == 1 && maze[k][j] == 0) count++;
            }
        }
        if(count > 1) continue;
        // 下面才可以挖
        visited[ty][tx] = 1;
        maze[ty][tx] = 0;
        //path[y][x] = 1;

        
        createPath(tx, ty); // 从新的路径开始 DFS
    }

    return;
    
}

void createMaze(int h, int w) {
    //int i, tx, ty;
    memset(visited, 0, sizeof(visited));
    memset(maze, 1, sizeof(maze)); // 1:墙  0:通道  2:玩家位置
    //srand(time(NULL));

    //Ed_x = w/2 + (rand() % (w/2) + 1); // 随机生成终点位置
    //Ed_y = h/2 + (rand() % (h/2) + 1);

    visited[St_y][St_x] = 1;
    maze[St_y][St_x] = 0; // 起点为通道
    //path[St_y][St_x] = 1; // 起点在起点到终点的路径上

    //St_x = x;
    //St_y = y;
    player_x = St_x;
    player_y = St_y;

    createPath(St_x, St_y);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(maze[HEIGHT - i][WIDTH - j] == 0){
                Ed_x = WIDTH - j;
                Ed_y = HEIGHT - i;
                flag = 1;
                break;
            }
        }
        if(flag) break;
    }
    
    /*for(int i = 2; i < HEIGHT + 1; i++){ // 不在 S E 路径上的墙，随机破除
        for(int j = 2; j < WIDTH + 1; j++){
            int p = rand() % 7;
            if(maze[i][j] == WALL && p == 1) maze[i][j] = 0;
        }
    }*/
    
    return;
}

void printMaze() {
    system("cls");
    printf("------- WELCOME TO MY MAZE ------\n"); // 用户友好界面
    printf("  Press 'w' 's' 'a' 'd' to move\n");
    printf(" Press 'f' to help you find a way\n");
    printf("        Press 'q' to quit\n\n");
    SetConsoleOutputCP(437);
    for (int y = 0; y < HEIGHT + 2; y++) {
        for (int x = 0; x < WIDTH + 2; x++) {
            if(x == St_x && y == St_y) printf("S");
            else if(x == Ed_x && y == Ed_y) printf("E");
            else if(x == player_x && y == player_y) printf("O");
            else if(path[y][x] && maze[y][x] != 2) printf("+");
            else printf("%c", (maze[y][x] == WALL) ? block : ' ');
        }
        printf("\n");
    }
    return;
}

int isSafe(int x, int y) {
    return (x >= 1 && x < WIDTH + 1 && y >= 1 && y < HEIGHT + 1 && maze[y][x] == 0);
}

void movePlayer(char dir) {
    int new_x = player_x, new_y = player_y;

    switch (dir) {
        case 'w': case 'W': new_y--; break;
        case 's': case 'S': new_y++; break;
        case 'a': case 'A': new_x--; break;
        case 'd': case 'D': new_x++; break;
    }

    if (isSafe(new_x, new_y)) {
        maze[player_y][player_x] = 0;
        maze[new_y][new_x] = 2;
        player_x = new_x;
        player_y = new_y;
    }
    /*printf("%d------%d\n", player_x, player_y);
    system("pause");  
    system("cls");*/ // 输出调试
    if(new_x == Ed_x && new_y == Ed_y) victory(); // 到达终点，胜利界面
    return;
}

void invalidInput() { // 提示用户键入字符非法
    COORD coord;
    coord.X = WIDTH / 2;
    coord.Y = HEIGHT * 2;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("Invalid input! Press 'y' to continue playing."); // 设置光标位置在整个迷宫下方后输出
    while(1) {
        char ch = _getch();
        if(ch == 'y') {
            coord.X = 0;
            coord.Y = 0;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); // 设置光标回到（0，0）后再打印迷宫
            printMaze();
            break;
        }
    }
    return;
}

void findWay(int x, int y){ // 保证是最短路径，使用 BFS
    
    //memset(queue, 0, sizeof(queue));
    memset(path, 0, sizeof(path));
    memset(vis, 0, sizeof(vis));
    int head = 0, tail = 0, tx, ty, nowx = x, nowy = y;
    queue[tail++] = (node){x, y, 0}; // queue.push()
    vis[y][x] = 1;
    while(head != tail){
        //printf("head=(%d,%d), tail=%d\n", queue[head].x, queue[head].y, tail); 输出调试//
        for(int i = 0; i < 4; i++){
            nowx = queue[head].x;
            nowy = queue[head].y;
            //vis[nowy][nowx] = 1; // // 搜过了
            tx = nowx + dir_x[i]/2; // 从当前的队头结点开始搜索
            ty = nowy + dir_y[i]/2;
            if(vis[ty][tx] || !isSafe(tx, ty)) continue; // // 搜过了或不是路
            queue[tail++] = (node){tx, ty, head}; // head 是前驱节点的 index
            vis[ty][tx] = 1; // 入队，标记
            if(tx == Ed_x && ty == Ed_y){ // 找到终点
                int index = head;
                
                while(index){
                    path[queue[index].y][queue[index].x] = 1; //
                    index = queue[index].index; // 链表的思想
                }
                return;
            }
        }
        head++; // queue.pop()
    }


    return;
}

void init(){
    printf("\n Welcome to my maze! \n Please input the height and width of your game:\n");
    printf("HEIGHT:");
    scanf("%d", &HEIGHT);
    printf("\nWIDTH:");
    scanf("%d", &WIDTH);
    return;
}

clock_t beginTime;
void game() {
    init();
    beginTime = clock();
    char input;
    srand(time(NULL)); 
    // 初始化玩家位置
    player_x = 0;
    player_y = 0;
    flag = 0;
    memset(path, 0, sizeof(path));
    // 生成迷宫并打印
    createMaze(HEIGHT, WIDTH);
    // 修改为DFS生成迷宫（破墙法），规定 Start End ，递归方法//
    printMaze();

    while (1) {
        input = _getch();
        switch (input) {
            case 'w': case 'W': // 上
            case 's': case 'S': // 下
            case 'a': case 'A': // 左
            case 'd': case 'D': // 右
                movePlayer(input);
                break;
            case 'f': // 自动寻路
                //system("cls");
                /*for (int y = 0; y < HEIGHT + 2; y++) {
                    for (int x = 0; x < WIDTH + 2; x++) {
                        printf("%d ", maze[y][x]);
                    }
                    printf("\n");
                }
                system("pause");*/ // 输出调试
                findWay(player_x, player_y);
                /*for (int y = 0; y < HEIGHT + 2; y++) {
                    for (int x = 0; x < WIDTH + 2; x++) {
                        printf("%d ", maze[y][x]);
                    }
                    printf("\n");
                }
                system("pause");*/ // 输出调试
                //printMaze();
                break;
            case 'q': // 退出游戏
                exit(0);
                break;
            default:
                // 其他输入提示用户非法
                invalidInput();
                break;
        }

        printMaze(); // 更新显示迷宫
    }
    return;

}

clock_t endTime;
void victory(){
    system("cls");     // 用户友好界面
    endTime = clock();
    printf("######################################\n\n"
           "# You won the game, Congratulations! #\n\n"
           "# You spent %fs in this maze.       #\n\n"
           "# Do you want to play again? [y/n]   #\n\n"
           "######################################", (double)(endTime-beginTime)/CLOCKS_PER_SEC);
    char ch = _getch();
    while(1){
        if(ch == 'y' || ch == 'Y') game(); // 再来一局
        else if(ch == 'n' || ch == 'N') exit(0); // 退出游戏
    }
    return;
}

int main() {
    game(); // 这样写是为了方便玩家胜利后重开一局
    return 0;
}
