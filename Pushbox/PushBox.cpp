#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>

/****************常量定义****************/
#define  N 30

#define  TEXT_COLOR 13

#define  AISLE      "  " //走廊
#define  N_AISLE    0
#define  AISLE_COLOR 0xf

#define  WALL       "■" //墙壁
#define  WALL_COLOR 14
#define  N_WALL     1

#define  AIM        "※" //未放箱子的目标
#define  N_AIM      2
#define  AIM_COLOR  10

#define  BOX        "□" //未在目标位置的箱子
#define  N_BOX      3
#define  BOX_COLOR  9

#define  BOX_OK     "★" //已在目标位置的箱子[即已放箱子的目标]
#define  N_BOX_OK   5
#define  BOX_OK_COLOR BOX_COLOR

#define  MAN        "♀" //人
#define  N_MAN      4
#define  MAN_COLOR  11

#define  MAN_IN     "* "//在目标中的人
#define  N_MAN_IN   6
#define  MAN_IN_COLOR MAN_COLOR

#define  ESC     0x1B
#define  UP      'w'
#define  DOWN    's'
#define  LEFT    'a'
#define  RIGHT   'd'
#define  BACK    'f'
#define  RESTART 'r'
#define  SAVE    'x'

#define  PF(a) printf("%s",a)
/****************常量定义****************/

/****************外部变量定义****************/
struct Point{
    int x,y;
}manPosition;	/*小人的坐标*/

int mission=1;	/*关数*/
int step;		/*步数*/
int push;		/*推的次数*/
int sum;		/*总步数*/
int Y,X;		/*地图大小*/
int map[N][N];	/*地图*/
int Laststep[1000]={0};		/*记录已走步数 1：上，2：下，3：左， 4：右*/
int ismove[1000]={0};       /*记录走的步数时箱子是否移动 1：移动，0：没移动*/
int count;                  /*记录连过的关数*/
int time=0;                 /*所走步数的下标*/
HANDLE handle;
int Record;                 /*每一关的最少记录*/
char order[10];
/****************外部变量定义****************/

/****************函数声明****************/
void gameInit();
void color(unsigned short c);
void gotoYX(int y,int x);
void textOut(short y,short x,unsigned short c,const char s[]);
void drawInfo();/*显示信息*/
void drawData(int k);
void drawMapYX(Point p);/*在指定地点显示map数组元素的信息*/
void drawStep();/*显示步数*/
void preMenu();
void menu();
void menuSelect();
void playGame(int map[][N]);/*玩游戏*/
unsigned char getKey();/*获得有效按键，返回按键值*/
void moveMan(Point manM1_p,int map[][N]);/*人移动*/
void moveBox(Point box_p,Point boxM1_p,int map[][N]);/*移箱子*/
int win();			/*判断当无※时，赢*/
void readMissionMap(int);	/*读m关卡的地图*/
void drawMap();	/*显示地图*/
void save();	/*保存当前状态*/
int read();		/*读取存档*/
void help();    /*帮助*/
void backlast();    /*返回上一步*/
void Writere(int m);	//写记录
void Readre(int m);		//读记录
/****************函数声明****************/





int main()
{
    gameInit();
    preMenu();

	while(1){
	    menu();
	    menuSelect();
        playGame(map);
	}

	return 0;
}

void gameInit()
{
	CONSOLE_CURSOR_INFO cursorInfo;

    handle=GetStdHandle(STD_OUTPUT_HANDLE);//获取标准输出设置句柄

	//检索有关指定的控制台屏幕缓冲区的光标的可见性和大小信息
    GetConsoleCursorInfo(handle, &cursorInfo);

    cursorInfo.bVisible=false;
    SetConsoleCursorInfo(handle, &cursorInfo);//设置控制台光标信息
    SetConsoleOutputCP(936);//设置代码页.936代表简体中文
}
void color(unsigned short c)
{
    SetConsoleTextAttribute(handle,c);//设置WriteConsole等函数的字符属性
}
void gotoYX(int y,int x)
{
    COORD cursorPosition;/*coordinate*/
	cursorPosition.Y=y;
	cursorPosition.X=x*2;
    SetConsoleCursorPosition(handle,cursorPosition);
}
void textOut(short y,short x,unsigned short c,const char s[])
{
    gotoYX(y,x);
    color(c);
    printf("%s",s);
}
void save()	/*保存当前状态*/
{
	int i,j;
	FILE *in;
    if((in=fopen("save game","w"))==NULL){
	    printf("cannot open this file\n");
        exit(0);
	}
	for(i=0;i<14;i++){
		for(j=0;j<16;j++){
			fwrite(&map[i][j],sizeof(int),1,in);
		}
	}
	fwrite(&mission,sizeof(int),1,in);
	fwrite(&step,sizeof(int),1,in);
	fwrite(&push,sizeof(int),1,in);
	printf("\n任务 %d 保存成功!\n",mission);
	system("pause");
	fclose(in);
}
int read()		/*读取存档*/
{
	int i,j;
	FILE *out;
    if((out=fopen("save game","r"))==NULL){
	    printf("\n不能打开文件\n");
        return 0;
	}
	else{
		for(i=0;i<14;i++){
            for(j=0;j<16;j++){
                fread(&map[i][j],sizeof(int),1,out);
			}
		}
        fread(&mission,sizeof(int),1,out);
        fread(&step,sizeof(int),1,out);
        fread(&push,sizeof(int),1,out);
        printf("\nM=%d\n",mission);
		printf("Game read!\n");

		fclose(out);
		return 1;
	}
}
void readMissionMap(int m)/*读m关卡的地图*/
{
    char filename[N];
    char t[N];
    int i,j;
    FILE *fp=NULL;
    sprintf(filename,"puzzle\\map%d.txt",m);
    fp=fopen(filename,"r");
    i=0;
    while(fgets(t,N,fp)!=NULL){
        for(j=0;t[j]!='\0';j++){
            map[i][j]=t[j]-'0';
            if(map[i][j]==N_MAN||map[i][j]== N_MAN_IN){
                manPosition.x=j;
                manPosition.y=i;
            }
        }

        i++;
    }
    Y=i;
    X=j-1;
    step=0;
    push=0;

    fclose(fp);
}

void drawMap()/*显示地图*/
{
    int i,j;
    system("cls");

    for(i=0;i<Y;i++){
	    gotoYX(i,0);
        for(j=0;j<X;j++){
            drawData(map[i][j]);
        }
	}
}



void drawData(int k)
{
    switch(k)
    {
        case N_AISLE:
            color(AISLE_COLOR);
            PF(AISLE);
            break;
        case N_WALL:
            color(WALL_COLOR);
            PF(WALL);
            break;
        case N_AIM:
            color(AIM_COLOR);
            PF(AIM);
            break;
        case N_BOX:
            color(BOX_COLOR);
            PF(BOX);
            break;
        case N_MAN:
            color(MAN_COLOR);
            PF(MAN);
            break;
        case N_BOX_OK:
            color(BOX_OK_COLOR);
            PF(BOX_OK);
            break;
        case N_MAN_IN:
            color(MAN_IN_COLOR);
            PF(MAN_IN);
        default:
			break;
    }
}

void drawMapYX(Point p)/*在指定地点显示map数组元素的信息*/
{
    gotoYX(p.y, p.x);
    drawData(map[p.y][p.x]);
}



void drawInfo()/*显示信息*/
{
    char s[N];

    sprintf(s,">>>>>  步数：%d  推：%d  总：%d  ",step,push,step+push);

    textOut(1, X, 0xf, "\t提示：");
    textOut(3, X, TEXT_COLOR, ">> 上：w,下：s,左：a,右：d");
    textOut(5, X, TEXT_COLOR, ">> 返回上一步：f");
    textOut(7, X, TEXT_COLOR, ">> 重新开始：r");
    textOut(9, X, TEXT_COLOR, ">> 保存：x");
    textOut(11, X, TEXT_COLOR, ">> 返回主菜单：ESC");
    textOut(13, X, TEXT_COLOR, s);
    sprintf(s, "第%d关", mission);
    textOut(Y+1, (X-4)/2, 0xf, s);
}

void drawStep()/*显示步数*/
{
    char s[40];
    sprintf(s,">>>>>  步数：%d  推：%d  总：%d  ",step,push,step+push);
    textOut(13,X,TEXT_COLOR,s);
}

void help()
{
	char c;

	while(1){
	    system("cls");
        color(0xf);
        printf("\t\t\t╭───────────╮\n");
        printf("\t\t\t│    游戏规则及说明    │\n");
        printf("\t\t\t╰───────────╯\n\n");

        color(MAN_COLOR);
        printf("   %s         ",MAN);
        color(0xf);
        printf("别看了，这就是你了，还挺酷的吧，嘿嘿！\n\n");
        color(MAN_IN_COLOR);
        printf("   %s         ",MAN_IN);
        color(0xf);
        printf("这也是你了，只是你踩到");
		color(AIM_COLOR);
		printf(AIM);
		color(0xf);
		printf("了！\n");
		printf("\n");
        color(WALL_COLOR);
        printf("   %s         ",WALL);
        color(0xf);
        printf("这个是墙，放心，不是豆腐渣工程，要不你可以去撞撞看，\n");
        printf("              你会懂得......\n");
        printf("\n");
        color(BOX_COLOR);
        printf("   %s         ",BOX);
        color(0xf);
        printf("这个是箱子，就是要你去推的那个，不要试图一下推两个箱\n");
        printf("              子，更不要想推着箱子把墙撞开，没用的...\n");
        printf("              如果到了走不下去的地步，也不要沮丧，可以按f返回上一步哟！\n");
        printf("\n");
        color(AIM_COLOR);
        printf("   %s         ",AIM);
        color(0xf);
                      printf("这里就是箱子们的最终归宿了，箱子要是回不到家，哼哼，\n");
        printf("              你自己掂量着办吧。另外，只有他们都接到了自己的小箱子，\n");
        printf("              你才能找到出口哦！\n");
        printf("\n");
        color(BOX_OK_COLOR);
        printf("   %s         ",BOX_OK);
        color(0xf);
        printf("箱子到家后就会变身啦！\n");
        printf("              a d s w   按这个控制方向就可以了，啥也不说了，开推吧那就！\n");
        color(11);
        printf("\t\t\t\t\t\tby zjb @2015/03/21\n");
        color(14);
        printf("\t\t\t\t>>返回请按 B<<\n");
        c=getche();/*输入后立即从控制台取字符，不以回车为结束*/
        if(c=='B'||c=='b'){
            system("cls");
            return;
        }
	}
}


void preMenu()
{
    printf("\n\n\n\n");
    int x=0;
    for(x=0;x<25;x++){
        textOut(0,x,10,"        ╭╮      ■ ■      □       □         ◆◆◆◆◆◆◆◆");
        textOut(1,x,10,"        ││    ■   ■      □□□□ □□□□                 ◆  \n");
        textOut(2,x,10,"    ╭─┘└╮ ■ ■■■■   □  ■   □  ■         ◆       ◆  \n");
        textOut(3,x,10,"    ╰─┐┌ ■ ■   ■         □   ┏━━━┓        ◆   ◆   \n");
        textOut(4,x,10,"        ││    ■ ■■■    □□□□┃┏━┓┃          ◆◆    \n");
        textOut(5,x,10,"    ╭─┘└╮  ■   ■         □   ┃┏━┓┃  ┏━━━━◆━━┓\n");
        textOut(6,x,10,"    ╰─┐┌╯  ■ ■■■      □□  ┃┏━┓┃  ┗━━━━◆━━┛\n");
        textOut(7,x,10,"    ╭─┘│    ■   ■      □ □ □┃┗━┛┃            ◆   \n");
        textOut(8,x,10,"    ╰──╯    ■■■■■  □  □ □┗━━━┛  ◆◆◆◆◆◆        \n\n");
        Sleep(20);
        for(int j=0;j<9;j++){
			textOut(j, x, 10,"                                                          ");
        }
		Sleep(5);
    }
    system("cls");
}
void menu()
{
    int x=0;
	textOut(0,x,10,"        ╭╮      ■ ■      □       □         ◆◆◆◆◆◆◆◆ \n");
	textOut(1,x,10,"        ││    ■   ■      □□□□ □□□□                 ◆  \n");
	textOut(2,x,10,"    ╭─┘└╮ ■ ■■■■   □  ■   □  ■         ◆       ◆  \n");
	textOut(3,x,10,"    ╰─┐┌ ■ ■   ■         □   ┏━━━┓        ◆   ◆   \n");
	textOut(4,x,10,"        ││    ■ ■■■    □□□□┃┏━┓┃          ◆◆    \n");
	textOut(5,x,10,"    ╭─┘└╮  ■   ■         □   ┃┏━┓┃  ┏━━━━◆━━┓\n");
	textOut(6,x,10,"    ╰─┐┌╯  ■ ■■■      □□  ┃┏━┓┃  ┗━━━━◆━━┛\n");
	textOut(7,x,10,"    ╭─┘│    ■   ■      □ □ □┃┗━┛┃            ◆   \n");
	textOut(8,x,10,"    ╰──╯    ■■■■■  □  □ □┗━━━┛  ◆◆◆◆◆◆        \n\n");
}

void menuSelect()
{
    char x;

    color(0xf);

    printf("                              >>1  开始游戏\n\n");
	printf("                              >>2  选择关卡\n\n");
	printf("                              >>3  载入档案\n\n");
	printf("                              >>4  游戏帮助\n\n");
	printf("                              >>5  退出游戏\n\n");
    while(1){
        switch(x=getch()){
            case '1':
                mission=1;
                readMissionMap(mission);
                return;
            case '2':
                system("cls");
                menu();
                color(14);
                printf("\t\t\t请选择关卡(1-45):");
                while(scanf("%d",&mission)){
                	if(mission>45){
                		printf("请输入1-45! ");
                	}
                	else{
                		readMissionMap(mission);
						return;
                	}
                }
                
            case '3':
                if(read()==1){
                    printf("**打开第%d关\n",mission);
                    return;
                }
                break;
            case '4':
                help();
                menu();
                menuSelect();
                break;
            case '5':
                exit(0);
                break;
            default:/*敲了其它键*/
                printf("请选择1-4\n");
				break;
        }
    }
}

void playGame(int map[][N])/*玩游戏*/
{
	unsigned char key;
    Point manM1_p;/*小人将要移动1步后的坐标*/
	Point manM2_p;/*小人将要移动2步后的坐标*/

	readMissionMap(mission);
	Readre(mission);
	drawMap();
	drawInfo();

	time=1;     //下标从1开始，初始化
	count=0;    //初始化

	while(1){
		Readre(mission);
		gotoYX(17,3);
    	color(12);
    	printf("恭喜你，已经连过%d关了！",count);
    	gotoYX(19,3);
    	color(11);
		switch(count){
			case 0:
				break;
			case 1:
				printf("First Blood!");
				break;
			case 2:
				printf("Double Kill!");
				break;
			case 3:
				printf("Triple Kill!");
				break;
			case 4:
				printf("Quadar Kill!");
                break;
			case 5:
				printf("Penta Kill!");
                break;
			case 6:
				printf("Unstoppable!");
				break;
			case 7:
				printf("Dominating!");
				break;
			case 8:
				printf("God like!");
				break;
			case 9:
				printf("Legendary!");
				break;
			default:
				printf("You are the God!");
				break;
		}
        switch(key=getKey()){
            case UP:/*上*/
                manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y-1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y-2;
				Laststep[time]=2;
                break;
            case DOWN:/*下*/
                manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y+1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y+2;
				Laststep[time]=1;
                break;
            case LEFT:/*左*/
                manM1_p.x=manPosition.x-1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x-2;
				manM2_p.y=manPosition.y;
				Laststep[time]=4;
                break;
            case RIGHT:/*右*/
                manM1_p.x=manPosition.x+1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x+2;
				manM2_p.y=manPosition.y;
				Laststep[time]=3;
                break;
            case  BACK:
				if(time!=1){
					backlast();
				}
				else{
					textOut(15, X, 10, "不能返回啦！");
				}
				continue;
            case  SAVE:
                 save();
                 continue;
            case   RESTART:
                  readMissionMap(mission);
                  Readre(mission);
                  drawMap();
                  drawInfo();
                  continue;
            case	ESC:
                  system("cls");
                  return;
            default:
				break;
        }
        switch(map[manM1_p.y][manM1_p.x]){
            case N_AISLE:	//0:走廊
            case N_AIM:		//2:目标
                 moveMan(manM1_p,map);
                 ismove[time]=0;
				 time++;
                 switch(key){
                 	case UP:
                 		textOut(15, X, 10, "上                 ");
                 		Laststep[time]=2;
                 		break;
				  	case DOWN:
				  		textOut(15, X, 10, "下                 ");
                 		Laststep[time]=1;
                 		break;
					case LEFT:
						textOut(15, X, 10, "左                 ");
                 		Laststep[time]=4;
                 		break;
                 	case RIGHT:
                 		textOut(15, X, 10, "右                 ");
                 		Laststep[time]=3;
                 		break;
                 	default:
						break;
                 }
                 step++;
                 drawStep();
                 break;
            case N_BOX:		//3:未在目标位置的箱子
            case N_BOX_OK:	//5:已在目标位置的箱子
                if(map[manM2_p.y][manM2_p.x]==N_AISLE
						|| map[manM2_p.y][manM2_p.x]==N_AIM){
                    moveBox(manM1_p,manM2_p,map);
                    ismove[time]=1;
                    moveMan(manM1_p,map);
                    time++;
                    switch(key){
	                 	case UP:
	                 		textOut(15, X, 10, "上                 ");
	                 		Laststep[time]=2;
	                 		break;
					  	case DOWN:
					  		textOut(15, X, 10, "下                 ");
	                 		Laststep[time]=1;
	                 		break;
						case LEFT:
							textOut(15, X, 10, "左                 ");
	                 		Laststep[time]=4;
	                 		break;
	                 	case RIGHT:
	                 		textOut(15, X, 10, "右                 ");
	                 		Laststep[time]=3;
	                 		break;
	                 	default:
							break;
	                }
                    push++;
                    drawStep();
                }
                break;
            default:
				break;
        }
        if(win()){
        	sum=push+step;
			if(sum<Record || Record==0){
				Writere(mission);
				gotoYX(20,20);
				printf("恭喜你打破本关记录！     \n");
				gotoYX(21,20);
				printf("是否继续？y or n？ ");
				fflush(stdin);
				while(scanf("%s",order)){
					if(!strcmp(order,"n")){
						system("cls");
						return;
					}
					else if(!strcmp(order,"y")){
						break;
					}
					else{
						printf("请输入 y or n ！");
					}
				}
			}
			time=1;
        	Laststep[time]=0;
        	ismove[time]=0;
			mission++;
			count++;
			if(mission>45){
				system("cls");
				printf("\n\t恭喜通关!!!\n\n\n\t");
				system("pause");
				return;
			}
			readMissionMap(mission);
			Readre(mission);
			drawMap();
			drawInfo();
		}
	}
}

unsigned char getKey()/*获得有效按键，返回按键值*/
{
    unsigned char key;

    while(1){
        key = getch();
        if(key ==RESTART || key == ESC || key == SAVE || key == BACK){
            return key;
		}
        if(key == UP ||key == DOWN || key == LEFT || key == RIGHT){
            return key;
		}
    }
}

void moveMan(Point manM1_p,int map[][N])/*人移动*/
{
	if(map[manM1_p.y][manM1_p.x]==N_AIM||map[manM1_p.y][manM1_p.x]==N_BOX_OK){
		map[manM1_p.y][manM1_p.x]=N_MAN_IN;
	}
	else{
		map[manM1_p.y][manM1_p.x]=N_MAN;
	}

	if(map[manPosition.y][manPosition.x]==N_MAN_IN){
		map[manPosition.y][manPosition.x]=N_AIM;
	}
	else{
		map[manPosition.y][manPosition.x]=N_AISLE;
	}

    drawMapYX(manPosition);
    drawMapYX(manM1_p);
    manPosition=manM1_p;
}

void moveBox(Point box_p,Point boxM1_p,int map[][N])/*移箱子*/
{
    map[boxM1_p.y][boxM1_p.x]=map[boxM1_p.y][boxM1_p.x]==N_AISLE?N_BOX:N_BOX_OK;
    map[box_p.y][box_p.x]=map[box_p.y][box_p.x]==N_BOX?N_AISLE:N_AIM;
    drawMapYX(boxM1_p);
    drawMapYX(box_p);
}

int win()/*判断当无※时，赢*/
{
    int i,j;
    for(i=0;i<Y;i++){
        for(j=0;j<X;j++){
            if(map[i][j]==N_AIM||map[i][j]==N_MAN_IN){
                return 0;
			}
		}
	}
    return 1;
}
void backlast()
{
	Point manM1_p;/*小人将要移动1步后的坐标*/
	Point manM2_p;/*小人现在坐标*/
	Point manM3_p;/*箱子的坐标*/
	time--;
	drawStep();
	if(time!=0){
		switch(Laststep[time]){
	        case 1:/*上*/
	            manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y-1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x;
				manM3_p.y=manPosition.y+1;
	            break;
	        case 2:/*下*/
	            manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y+1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x;
				manM3_p.y=manPosition.y-1;
	            break;
	        case 3:/*左*/
	            manM1_p.x=manPosition.x-1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x+1;
				manM3_p.y=manPosition.y;
	            break;
	        case 4:/*右*/
	            manM1_p.x=manPosition.x+1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x-1;
				manM3_p.y=manPosition.y;
	            break;
	        default:
				break;
	    }
	    	switch(map[manM1_p.y][manM1_p.x]){
	            case N_AISLE:	//0:走廊
	            case N_AIM: 	//2:目标
	                 moveMan(manM1_p,map);
				     break;
				default:
					break;
			}
			if(ismove[time]==1){
				switch(map[manM3_p.y][manM3_p.x]){
	            case N_BOX:		//3:未在目标位置的箱子
	            case N_BOX_OK:	//5:已在目标位置的箱子
	                if(map[manM2_p.y][manM2_p.x]==N_AISLE
							|| map[manM2_p.y][manM2_p.x]==N_AIM){
	                   	moveBox(manM3_p,manM2_p,map);
	                   	push--;
	                   	drawStep();
	                }
	                break;
	            default:
					break;
				}
			}
			else{
				step--;
	    		drawStep();
			}
	}
}
void Readre(int m)
{
	int x;
    char filename[N];
    FILE *fp;
    sprintf(filename,"record\\record%d.txt",m);
    fp=fopen(filename,"r");
    fscanf(fp,"%d",&x);
	gotoYX(20,20);
	color(11);
	Record=x;
    printf("本关最少总步数记录：%d     ",Record);
    fclose(fp);
}
void Writere(int m)
{
    char filename[N];
    FILE *fp;
    sprintf(filename,"record\\record%d.txt",m);
    fp=fopen(filename,"w");
    fprintf(fp,"%d",sum);
    fclose(fp);
}
