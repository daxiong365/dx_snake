/*******************************
 * 大雄公开课 daxiong365.com
 * Snake windows系统CMD窗口贪吃蛇C语言程序
 * dx_snake.c
*******************************/

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

/*
颜色属性由两个十六进制数字指定 – 第一个
对应于背景，第二个对应于前景。每个数字
可以为以下任何值:

0 = 黑色       8 = 灰色
1 = 蓝色       9 = 淡蓝色
2 = 绿色       A = 淡绿色
3 = 浅绿色     B = 淡浅绿色
4 = 红色       C = 淡红色
5 = 紫色       D = 淡紫色
6 = 黄色       E = 淡黄色
7 = 白色       F = 亮白色*/
#define COLOR_GREEN_GREEN  0x33
#define COLOR_GREEN_PURPLE 0x35
#define COLOR_GRAY_BLUE 0x81
#define COLOR_YELLOW_RED 0xEC

WORD old_screen_attr = 0;//保存旧颜色

void MoveCursorPosition(int x, int y)//移动光标位置
{
    //1.指定一个坐标
    COORD pos;
    pos.X = x;//指定横坐标
    pos.Y = y;//指定纵坐标

    //2.获得标准的输出设备的句柄 即获得cmd窗口的打印控制
    HANDLE hCmdWindow;    
    hCmdWindow = GetStdHandle(STD_OUTPUT_HANDLE);

    //3.移动光标到pos变量指定的位置
    SetConsoleCursorPosition(hCmdWindow, pos);
    return;
}
//打印字符在指定的位置
void PrintAtCursor(char *ch, int x , int y) {
    MoveCursorPosition(x,y);
    printf("%s",ch);
}


//修改当前打印将使用的颜色 （并且保存修改前的设置）
void SetColor(WORD  NewColor /*颜色值*/, WORD  *old /*记住修改前的屏幕属性*/)
{
    //1. 获得打印窗口的操作句柄
    HANDLE hOutput;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    
    //2. 记住当前正在使用的颜色
    if(old) {
        CONSOLE_SCREEN_BUFFER_INFO Info; 
        GetConsoleScreenBufferInfo(hOutput, &Info);
        *old = Info.wAttributes; 
    }

    //3. 根据函数参数指定的颜色值，修改屏幕的打印颜色属性
    SetConsoleTextAttribute(hOutput,NewColor);
    return;
}

//创建游戏区域
void create_game_area(int width, int height)
{
    int width_index = 0;
    int height_index = 0;

    //区域将被填充为绿色
    SetColor(COLOR_GREEN_GREEN,&old_screen_attr);

    for(width_index = 0; width_index < width ; width_index++) {
        for(height_index = 0; height_index < height ; height_index++) {
            //从(2,1)坐标开始，打印方块填充‘游戏区域’ 注意 一个■字符的大小为横向2，纵向1

            PrintAtCursor("■",2+width_index*2,1+height_index);
        }
    }

    //打印完，恢复为原来的打印颜色，不影响后面的打印
    SetColor(old_screen_attr,NULL);

    return;
}

void create_game_area_frame(int width,int height)
{
    int width_index = 0;
    int height_index = 0;

    //区域将被填充为绿色
    SetColor(COLOR_GRAY_BLUE,&old_screen_attr);

    //上边框
    for(width_index = 0; width_index < width ; width_index++) {
        //从(2,0)坐标开始，横向打印方块
        PrintAtCursor("■",2+width_index*2,0);
    }

    //左边框
    for(height_index = 0; height_index < height ; height_index++) {
        //从(0,1)坐标开始，纵向打印方块
        PrintAtCursor("■",0,1+height_index);
    }

    //下边框
    for(width_index = 0; width_index < width ; width_index++) {
        //从(2,0)坐标开始，横向打印方块
        PrintAtCursor("■",2+width_index*2,height+1);
    }

    //右边框
    for(height_index = 0; height_index < height ; height_index++) {
        //从(0,1)坐标开始，纵向打印方块
        PrintAtCursor("■",2+width*2,1+height_index);
    }

    //打印完，恢复为原来的打印颜色，不影响后面的打印
    SetColor(old_screen_attr,NULL);

    //打印完毕 移动光标位置到游戏外部
    MoveCursorPosition(0,42);

    return;
}

typedef struct SNAKE_NODE //蛇身的一个节点
{
    int x;//此节点横坐标
    int y;//此节点纵坐标
    struct SNAKE_NODE *next_node; //指向下一个节点的指针
} snake_node_s;

//向贪吃蛇添加一个新节点
//(x/y)为新节点的坐标，节点将添加到蛇的头部
//返回新的贪吃蛇的蛇头指针
snake_node_s* snake_add_node_to_head(snake_node_s * snake_head, int x, int y) {

    snake_node_s* new_node = (snake_node_s*)malloc(sizeof(snake_node_s));

    //填写新节点的坐标
    new_node->x = x;
    new_node->y = y;

    new_node->next_node = snake_head;//将新节点链接到蛇的头部

    return new_node;
}

//初始化贪吃蛇 
//在x/y指定的位置
//长度为number_of_node
snake_node_s* snake_init(snake_node_s* snake_head,    
    int x, int y,
    int number_of_node) {

    while(number_of_node--) {        
        snake_head = snake_add_node_to_head(snake_head,x,y);
        x = x + 1;//向右逐步创建下一个蛇身节点
    }

    return snake_head;
}

//打印贪吃蛇
void print_snake(snake_node_s* snake) {

    //绿底紫字的颜色
    SetColor(COLOR_GREEN_PURPLE,&old_screen_attr);

    //打印蛇头
    if(snake) {
        PrintAtCursor("◆",snake->x*2,snake->y);
        snake = snake->next_node;
    }
    //打印蛇身
    while(snake) {
        PrintAtCursor("●",snake->x*2,snake->y);
        snake = snake->next_node;
    }

    //打印完，恢复为原来的打印颜色，不影响后面的打印
    SetColor(old_screen_attr,NULL);

    //打印完毕 移动光标位置到游戏外部
    MoveCursorPosition(0,42);

    return;
}

snake_node_s* food = NULL;//食物节点

//获得一个随机整数数字
int get_random_number() {
    //使用一个'当前时间值'加一个'伪随机数'来作为当前的‘随机种子’
    srand((unsigned)time(NULL)+rand());

    //得到‘随机’数字
    return rand();
}

//获得随机整数作为x/y坐标值
void get_random_position(int *x,int *y){
    //用随机数作为x/y坐标
    *x = get_random_number()%40; //不超过边框
    *y = get_random_number()%40; //不超过边框

    if(0 != ((*x)%2)) {
        *x = *x - 1; //横坐标保证是偶数
    }

    *x+=1; //相对左边框偏移一个格子的横向距离
    *y+=1; //相对上边框偏移一个格子的纵向距离

    return;
}
void generate_new_food() {

    snake_node_s* new_node = (snake_node_s*)malloc(sizeof(snake_node_s));

    //new_node->x = 10;
    //new_node->y = 15;
    //获得随机x y坐标
    get_random_position(&new_node->x,&new_node->y);

    new_node->next_node = NULL;
    food = new_node;

    //黄底红字的颜色
    SetColor(COLOR_YELLOW_RED,&old_screen_attr);

    PrintAtCursor("★",food->x*2,food->y);

    //打印完，恢复为原来的打印颜色，不影响后面的打印
    SetColor(old_screen_attr,NULL);

    //打印完毕 移动光标位置到游戏外部
    MoveCursorPosition(0,42);

    return;
}

//检查参数给定的food和snake是否重合
int check_food_is_on_snake(snake_node_s *food, snake_node_s *snake)
{
    while(food && snake) {
        //如果food和snake坐标相同，则表示重合
        if((snake->x == food->x)
            && (snake->y == food->y)) {
                return TRUE;
            }
        snake = snake->next_node;
    }
    return FALSE; //未发现重合 返回false
}

snake_node_s* the_snake_head = NULL; //贪吃蛇的蛇头
int clock_mseconds = 350; //游戏运行每步的时间间隔  毫秒
int round_elapsed = 0; //游戏已经运行了多少步
int user_input_direction = 4; //1 上， 2下， 3 左， 4 右
char* direction_str = "右";
int food_count = 0; //吃了几个食物

void print_game_info() {
    char str_buff[128];

    snprintf(str_buff,sizeof(str_buff),"贪吃蛇已经爬行了%d步",round_elapsed);
    PrintAtCursor(str_buff,86,8);

    snprintf(str_buff,sizeof(str_buff),"蛇头位置[X=%d,Y=%d]",the_snake_head->x,the_snake_head->y);
    PrintAtCursor(str_buff,86,9);

    snprintf(str_buff,sizeof(str_buff),"食物位置[X=%d,Y=%d]",food->x,food->y);
    PrintAtCursor(str_buff,86,10);

    snprintf(str_buff,sizeof(str_buff),"吃了%d个食物",food_count);
    PrintAtCursor(str_buff,86,11);

    snprintf(str_buff,sizeof(str_buff),"游戏速度 %d毫秒/步",clock_mseconds);
    PrintAtCursor(str_buff,86,12);
    
    snprintf(str_buff,sizeof(str_buff),"用户输入方向%s",direction_str);
    PrintAtCursor(str_buff,86,13);

    //打印完毕 移动光标位置到游戏外部
    MoveCursorPosition(0,42);

    return;
}
void game_exit() {
    //询问用户是否确定要退出 
    char option;
    char char_enter;

    do {
        MoveCursorPosition(0,42);
        PrintAtCursor("退出游戏?[y] ",86,15);
        option = getchar();
        char_enter = getchar();
    } while ( option != 'y' && option != 'n');

    if(option == 'n') {
        //用户选择了no，所以不退出游戏
        PrintAtCursor("                 ",86,15);//清除掉‘退出游戏’的字符
        return;
    }

    MoveCursorPosition(0,42);//移动光标位置到游戏外部

    //用户选择了Yes，退出游戏
    exit(0);//退出
    return;
}

void game_pause()//暂停
{
    PrintAtCursor("暂停 按空格继续...",86,15);

    while (1)
    {
        //无限循环等待 直到用户按了空格
        Sleep(500);

        if (GetAsyncKeyState(VK_SPACE))
        {
            //再按一下空格，结束暂停
            break;
        }
    }
    //暂停结束
    PrintAtCursor("                    ",86,15);//清除掉‘暂停’的字符
    return;
}
void snake_tail_remove() {
    snake_node_s * node = the_snake_head;
    snake_node_s * tail_node = NULL;

    while(node) {
        if(node->next_node == NULL) {
            //只有一个蛇头（实际本程序不会发生）
            //直接退出
            return;
        } else if (node->next_node->next_node == NULL){
            //找到了蛇尾节点 'the_snake_head->next_node'
            tail_node = node->next_node;

            //在屏幕打印覆盖掉蛇尾的显示
            SetColor(COLOR_GREEN_GREEN,&old_screen_attr); //设置为草坪的颜色
            PrintAtCursor("  ",tail_node->x*2,tail_node->y);//打印空格覆盖掉蛇尾节点
            //打印完，恢复为原来的打印颜色，不影响后面的打印
            SetColor(old_screen_attr,NULL);

            //现在删掉它的数据
            free(tail_node);//释放内存
            node->next_node = NULL;//'node'成为新的蛇尾
            break;

        }
        node = node->next_node;
    }
}

int game_over_check()
{
    //检测触墙
    if (the_snake_head->x == 0 || the_snake_head->x == 40 
        || the_snake_head->y == 0 || the_snake_head->y == 40)
    {
        //已经触墙，结束游戏
        return 1;
    }

    //检测咬到自己
    snake_node_s * node = the_snake_head->next_node;
    while (node != NULL) {
        if (the_snake_head->x == node->x && the_snake_head->y == node->y)
        {
            //如果蛇头和蛇身上的任意节点重合，则说明咬到了自己
            return 1;
        } 
        node = node->next_node;
    }
    return 0;
}
void snake_move() {
    //蛇的移动，实际上就是在移动方向上添加一个蛇头节点
    //并且把旧的蛇尾节点删除，所以:

    //1, 创建新的蛇头节点
    //计算新蛇头节点的位置
    int x = 0;
    int y = 0;
    switch(user_input_direction) {
        case 1:
            //向上 
            x = the_snake_head->x; //横坐标不变
            y = the_snake_head->y - 1; //纵坐标减一
            break;
        case 2:
            //向下
            x = the_snake_head->x; //横坐标不变
            y = the_snake_head->y + 1; //纵坐标加一
            break;
        case 3:
            //向左
            x = the_snake_head->x - 1; //横坐标减一
            y = the_snake_head->y; //纵坐标不变
            break;
        case 4:
            //向右
            x = the_snake_head->x + 1; //横坐标加一
            y = the_snake_head->y; //纵坐标不变
            break;
        default:
            break;
    }
    //添加这个节点到蛇头
    the_snake_head = snake_add_node_to_head(the_snake_head,x,y);

    //2, 删掉蛇尾节点
    //如果吃到了食物，则无需删除蛇尾
    if((the_snake_head->x == food->x) 
        && (the_snake_head->y == food->y)){
            food_count++;
            //食物被吃掉，生成一个新的食物
                do {
                    generate_new_food();
                }  while (TRUE == check_food_is_on_snake(food,the_snake_head));     
            
    } else {
        snake_tail_remove();
    }

    print_snake(the_snake_head);//打印最新蛇形

    return;
}

//游戏的主循环
void game_run_loop() {

    //先设置一遍所有按键的监控 用来忽略在此之前用户的按键输入
    GetAsyncKeyState(VK_UP);
    GetAsyncKeyState(VK_DOWN);
    GetAsyncKeyState(VK_LEFT);
    GetAsyncKeyState(VK_RIGHT);
    GetAsyncKeyState(VK_ESCAPE);
    GetAsyncKeyState(VK_SPACE);
    GetAsyncKeyState(VK_F1);
    GetAsyncKeyState(VK_F2);

    //从现在开始检查用户的按键输入
    while(1) {
        if(GetAsyncKeyState(VK_UP) && (user_input_direction != 2)/*当前向下，不可以改成上*/) {
            //用户输入了‘上’按键
            user_input_direction = 1;
            direction_str = "上";
        }else if(GetAsyncKeyState(VK_DOWN)  && (user_input_direction != 1)/*当前向上，不可以改成下*/) {
            //用户输入了‘下’按键
            user_input_direction = 2;
            direction_str = "下";
        } else if(GetAsyncKeyState(VK_LEFT) && (user_input_direction != 4)/*当前向右，不可以改成左*/) {
            //用户输入了‘左’按键
            user_input_direction = 3;
            direction_str = "左";
        } else if(GetAsyncKeyState(VK_RIGHT) && (user_input_direction != 3)/*当前向左，不可以改成右*/) {
            //用户输入了‘右’按键
            user_input_direction = 4;
            direction_str = "右";
        } else if (GetAsyncKeyState(VK_ESCAPE)) {
            //用户输入了ESC
            game_exit();
        } else if (GetAsyncKeyState(VK_SPACE)){
            //输入了空格 暂停游戏运转
            game_pause();
        } else if (GetAsyncKeyState(VK_F1)) {
            //用户输入了F1加速
            if(clock_mseconds > 50) {
                clock_mseconds -= 50;
            }
        } else if (GetAsyncKeyState(VK_F2)) {
            //用户输入了F1减速
            if(clock_mseconds < 850) {
                clock_mseconds += 50;
            }
        }

        print_game_info();
        snake_move();//蛇移动一步

        //检查游戏是否失败
        if(game_over_check()) {
            SetColor(COLOR_YELLOW_RED, &old_screen_attr); //设置醒目的颜色
            PrintAtCursor("游戏失败退出！",86,15);
            //打印完，恢复为原来的打印颜色，不影响后面的打印
            SetColor(old_screen_attr,NULL);
            MoveCursorPosition(0,42);//移动光标位置到游戏外部
            exit(0);//退出
        }
        Sleep(clock_mseconds);//tick
        round_elapsed++;//循环次数累加
    }
}
int main()
{
    system("chcp"); //显示当前cmd窗口的显示字符集
    system("chcp 65001");//调整cmd窗口输出的字符集为UTF-8 用来显示中文字符
    system("cls");//清屏

    //调整整个console窗口的大小
    system("mode con cols=140 lines=50");

    //绘制游戏区域 40x40 
    create_game_area(40,40);
    create_game_area_frame(40,40);//游戏边框

    //在区域中央创建贪吃蛇
    the_snake_head = 
        snake_init(the_snake_head,40/2,40/2,3);

    //将初始化的贪吃蛇打印出来
    print_snake(the_snake_head);

    //创建食物
    do {
        generate_new_food();
    } while (TRUE == check_food_is_on_snake(food,the_snake_head));

    game_run_loop();//运行主循环

    return 0;
}
