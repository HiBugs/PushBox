#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>

/****************��������****************/
#define  N 30

#define  TEXT_COLOR 13

#define  AISLE      "  " //����
#define  N_AISLE    0
#define  AISLE_COLOR 0xf

#define  WALL       "��" //ǽ��
#define  WALL_COLOR 14
#define  N_WALL     1

#define  AIM        "��" //δ�����ӵ�Ŀ��
#define  N_AIM      2
#define  AIM_COLOR  10

#define  BOX        "��" //δ��Ŀ��λ�õ�����
#define  N_BOX      3
#define  BOX_COLOR  9

#define  BOX_OK     "��" //����Ŀ��λ�õ�����[���ѷ����ӵ�Ŀ��]
#define  N_BOX_OK   5
#define  BOX_OK_COLOR BOX_COLOR

#define  MAN        "��" //��
#define  N_MAN      4
#define  MAN_COLOR  11

#define  MAN_IN     "* "//��Ŀ���е���
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
/****************��������****************/

/****************�ⲿ��������****************/
struct Point{
    int x,y;
}manPosition;	/*С�˵�����*/

int mission=1;	/*����*/
int step;		/*����*/
int push;		/*�ƵĴ���*/
int sum;		/*�ܲ���*/
int Y,X;		/*��ͼ��С*/
int map[N][N];	/*��ͼ*/
int Laststep[1000]={0};		/*��¼���߲��� 1���ϣ�2���£�3���� 4����*/
int ismove[1000]={0};       /*��¼�ߵĲ���ʱ�����Ƿ��ƶ� 1���ƶ���0��û�ƶ�*/
int count;                  /*��¼�����Ĺ���*/
int time=0;                 /*���߲������±�*/
HANDLE handle;
int Record;                 /*ÿһ�ص����ټ�¼*/
char order[10];
/****************�ⲿ��������****************/

/****************��������****************/
void gameInit();
void color(unsigned short c);
void gotoYX(int y,int x);
void textOut(short y,short x,unsigned short c,const char s[]);
void drawInfo();/*��ʾ��Ϣ*/
void drawData(int k);
void drawMapYX(Point p);/*��ָ���ص���ʾmap����Ԫ�ص���Ϣ*/
void drawStep();/*��ʾ����*/
void preMenu();
void menu();
void menuSelect();
void playGame(int map[][N]);/*����Ϸ*/
unsigned char getKey();/*�����Ч���������ذ���ֵ*/
void moveMan(Point manM1_p,int map[][N]);/*���ƶ�*/
void moveBox(Point box_p,Point boxM1_p,int map[][N]);/*������*/
int win();			/*�жϵ��ޡ�ʱ��Ӯ*/
void readMissionMap(int);	/*��m�ؿ��ĵ�ͼ*/
void drawMap();	/*��ʾ��ͼ*/
void save();	/*���浱ǰ״̬*/
int read();		/*��ȡ�浵*/
void help();    /*����*/
void backlast();    /*������һ��*/
void Writere(int m);	//д��¼
void Readre(int m);		//����¼
/****************��������****************/





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

    handle=GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��׼������þ��

	//�����й�ָ���Ŀ���̨��Ļ�������Ĺ��Ŀɼ��Ժʹ�С��Ϣ
    GetConsoleCursorInfo(handle, &cursorInfo);

    cursorInfo.bVisible=false;
    SetConsoleCursorInfo(handle, &cursorInfo);//���ÿ���̨�����Ϣ
    SetConsoleOutputCP(936);//���ô���ҳ.936�����������
}
void color(unsigned short c)
{
    SetConsoleTextAttribute(handle,c);//����WriteConsole�Ⱥ������ַ�����
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
void save()	/*���浱ǰ״̬*/
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
	printf("\n���� %d ����ɹ�!\n",mission);
	system("pause");
	fclose(in);
}
int read()		/*��ȡ�浵*/
{
	int i,j;
	FILE *out;
    if((out=fopen("save game","r"))==NULL){
	    printf("\n���ܴ��ļ�\n");
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
void readMissionMap(int m)/*��m�ؿ��ĵ�ͼ*/
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

void drawMap()/*��ʾ��ͼ*/
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

void drawMapYX(Point p)/*��ָ���ص���ʾmap����Ԫ�ص���Ϣ*/
{
    gotoYX(p.y, p.x);
    drawData(map[p.y][p.x]);
}



void drawInfo()/*��ʾ��Ϣ*/
{
    char s[N];

    sprintf(s,">>>>>  ������%d  �ƣ�%d  �ܣ�%d  ",step,push,step+push);

    textOut(1, X, 0xf, "\t��ʾ��");
    textOut(3, X, TEXT_COLOR, ">> �ϣ�w,�£�s,��a,�ң�d");
    textOut(5, X, TEXT_COLOR, ">> ������һ����f");
    textOut(7, X, TEXT_COLOR, ">> ���¿�ʼ��r");
    textOut(9, X, TEXT_COLOR, ">> ���棺x");
    textOut(11, X, TEXT_COLOR, ">> �������˵���ESC");
    textOut(13, X, TEXT_COLOR, s);
    sprintf(s, "��%d��", mission);
    textOut(Y+1, (X-4)/2, 0xf, s);
}

void drawStep()/*��ʾ����*/
{
    char s[40];
    sprintf(s,">>>>>  ������%d  �ƣ�%d  �ܣ�%d  ",step,push,step+push);
    textOut(13,X,TEXT_COLOR,s);
}

void help()
{
	char c;

	while(1){
	    system("cls");
        color(0xf);
        printf("\t\t\t�q�����������������������r\n");
        printf("\t\t\t��    ��Ϸ����˵��    ��\n");
        printf("\t\t\t�t�����������������������s\n\n");

        color(MAN_COLOR);
        printf("   %s         ",MAN);
        color(0xf);
        printf("���ˣ���������ˣ���ͦ��İɣ��ٺ٣�\n\n");
        color(MAN_IN_COLOR);
        printf("   %s         ",MAN_IN);
        color(0xf);
        printf("��Ҳ�����ˣ�ֻ����ȵ�");
		color(AIM_COLOR);
		printf(AIM);
		color(0xf);
		printf("�ˣ�\n");
		printf("\n");
        color(WALL_COLOR);
        printf("   %s         ",WALL);
        color(0xf);
        printf("�����ǽ�����ģ����Ƕ��������̣�Ҫ�������ȥײײ����\n");
        printf("              ��ᶮ��......\n");
        printf("\n");
        color(BOX_COLOR);
        printf("   %s         ",BOX);
        color(0xf);
        printf("��������ӣ�����Ҫ��ȥ�Ƶ��Ǹ�����Ҫ��ͼһ����������\n");
        printf("              �ӣ�����Ҫ���������Ӱ�ǽײ����û�õ�...\n");
        printf("              ��������߲���ȥ�ĵز���Ҳ��Ҫ��ɥ�����԰�f������һ��Ӵ��\n");
        printf("\n");
        color(AIM_COLOR);
        printf("   %s         ",AIM);
        color(0xf);
                      printf("������������ǵ����չ����ˣ�����Ҫ�ǻز����ң��ߺߣ�\n");
        printf("              ���Լ������Ű�ɡ����⣬ֻ�����Ƕ��ӵ����Լ���С���ӣ�\n");
        printf("              ������ҵ�����Ŷ��\n");
        printf("\n");
        color(BOX_OK_COLOR);
        printf("   %s         ",BOX_OK);
        color(0xf);
        printf("���ӵ��Һ�ͻ��������\n");
        printf("              a d s w   ��������Ʒ���Ϳ����ˣ�ɶҲ��˵�ˣ����ư��Ǿͣ�\n");
        color(11);
        printf("\t\t\t\t\t\tby zjb @2015/03/21\n");
        color(14);
        printf("\t\t\t\t>>�����밴 B<<\n");
        c=getche();/*����������ӿ���̨ȡ�ַ������Իس�Ϊ����*/
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
        textOut(0,x,10,"        �q�r      �� ��      ��       ��         ����������������");
        textOut(1,x,10,"        ����    ��   ��      �������� ��������                 ��  \n");
        textOut(2,x,10,"    �q�������r �� ��������   ��  ��   ��  ��         ��       ��  \n");
        textOut(3,x,10,"    �t������ �� ��   ��         ��   ����������        ��   ��   \n");
        textOut(4,x,10,"        ����    �� ������    ������������������          ����    \n");
        textOut(5,x,10,"    �q�������r  ��   ��         ��   ����������  ������������������\n");
        textOut(6,x,10,"    �t�������s  �� ������      ����  ����������  ������������������\n");
        textOut(7,x,10,"    �q������    ��   ��      �� �� ������������            ��   \n");
        textOut(8,x,10,"    �t�����s    ����������  ��  �� ������������  ������������        \n\n");
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
	textOut(0,x,10,"        �q�r      �� ��      ��       ��         ���������������� \n");
	textOut(1,x,10,"        ����    ��   ��      �������� ��������                 ��  \n");
	textOut(2,x,10,"    �q�������r �� ��������   ��  ��   ��  ��         ��       ��  \n");
	textOut(3,x,10,"    �t������ �� ��   ��         ��   ����������        ��   ��   \n");
	textOut(4,x,10,"        ����    �� ������    ������������������          ����    \n");
	textOut(5,x,10,"    �q�������r  ��   ��         ��   ����������  ������������������\n");
	textOut(6,x,10,"    �t�������s  �� ������      ����  ����������  ������������������\n");
	textOut(7,x,10,"    �q������    ��   ��      �� �� ������������            ��   \n");
	textOut(8,x,10,"    �t�����s    ����������  ��  �� ������������  ������������        \n\n");
}

void menuSelect()
{
    char x;

    color(0xf);

    printf("                              >>1  ��ʼ��Ϸ\n\n");
	printf("                              >>2  ѡ��ؿ�\n\n");
	printf("                              >>3  ���뵵��\n\n");
	printf("                              >>4  ��Ϸ����\n\n");
	printf("                              >>5  �˳���Ϸ\n\n");
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
                printf("\t\t\t��ѡ��ؿ�(1-45):");
                while(scanf("%d",&mission)){
                	if(mission>45){
                		printf("������1-45! ");
                	}
                	else{
                		readMissionMap(mission);
						return;
                	}
                }
                
            case '3':
                if(read()==1){
                    printf("**�򿪵�%d��\n",mission);
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
            default:/*����������*/
                printf("��ѡ��1-4\n");
				break;
        }
    }
}

void playGame(int map[][N])/*����Ϸ*/
{
	unsigned char key;
    Point manM1_p;/*С�˽�Ҫ�ƶ�1���������*/
	Point manM2_p;/*С�˽�Ҫ�ƶ�2���������*/

	readMissionMap(mission);
	Readre(mission);
	drawMap();
	drawInfo();

	time=1;     //�±��1��ʼ����ʼ��
	count=0;    //��ʼ��

	while(1){
		Readre(mission);
		gotoYX(17,3);
    	color(12);
    	printf("��ϲ�㣬�Ѿ�����%d���ˣ�",count);
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
            case UP:/*��*/
                manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y-1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y-2;
				Laststep[time]=2;
                break;
            case DOWN:/*��*/
                manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y+1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y+2;
				Laststep[time]=1;
                break;
            case LEFT:/*��*/
                manM1_p.x=manPosition.x-1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x-2;
				manM2_p.y=manPosition.y;
				Laststep[time]=4;
                break;
            case RIGHT:/*��*/
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
					textOut(15, X, 10, "���ܷ�������");
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
            case N_AISLE:	//0:����
            case N_AIM:		//2:Ŀ��
                 moveMan(manM1_p,map);
                 ismove[time]=0;
				 time++;
                 switch(key){
                 	case UP:
                 		textOut(15, X, 10, "��                 ");
                 		Laststep[time]=2;
                 		break;
				  	case DOWN:
				  		textOut(15, X, 10, "��                 ");
                 		Laststep[time]=1;
                 		break;
					case LEFT:
						textOut(15, X, 10, "��                 ");
                 		Laststep[time]=4;
                 		break;
                 	case RIGHT:
                 		textOut(15, X, 10, "��                 ");
                 		Laststep[time]=3;
                 		break;
                 	default:
						break;
                 }
                 step++;
                 drawStep();
                 break;
            case N_BOX:		//3:δ��Ŀ��λ�õ�����
            case N_BOX_OK:	//5:����Ŀ��λ�õ�����
                if(map[manM2_p.y][manM2_p.x]==N_AISLE
						|| map[manM2_p.y][manM2_p.x]==N_AIM){
                    moveBox(manM1_p,manM2_p,map);
                    ismove[time]=1;
                    moveMan(manM1_p,map);
                    time++;
                    switch(key){
	                 	case UP:
	                 		textOut(15, X, 10, "��                 ");
	                 		Laststep[time]=2;
	                 		break;
					  	case DOWN:
					  		textOut(15, X, 10, "��                 ");
	                 		Laststep[time]=1;
	                 		break;
						case LEFT:
							textOut(15, X, 10, "��                 ");
	                 		Laststep[time]=4;
	                 		break;
	                 	case RIGHT:
	                 		textOut(15, X, 10, "��                 ");
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
				printf("��ϲ����Ʊ��ؼ�¼��     \n");
				gotoYX(21,20);
				printf("�Ƿ������y or n�� ");
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
						printf("������ y or n ��");
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
				printf("\n\t��ϲͨ��!!!\n\n\n\t");
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

unsigned char getKey()/*�����Ч���������ذ���ֵ*/
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

void moveMan(Point manM1_p,int map[][N])/*���ƶ�*/
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

void moveBox(Point box_p,Point boxM1_p,int map[][N])/*������*/
{
    map[boxM1_p.y][boxM1_p.x]=map[boxM1_p.y][boxM1_p.x]==N_AISLE?N_BOX:N_BOX_OK;
    map[box_p.y][box_p.x]=map[box_p.y][box_p.x]==N_BOX?N_AISLE:N_AIM;
    drawMapYX(boxM1_p);
    drawMapYX(box_p);
}

int win()/*�жϵ��ޡ�ʱ��Ӯ*/
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
	Point manM1_p;/*С�˽�Ҫ�ƶ�1���������*/
	Point manM2_p;/*С����������*/
	Point manM3_p;/*���ӵ�����*/
	time--;
	drawStep();
	if(time!=0){
		switch(Laststep[time]){
	        case 1:/*��*/
	            manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y-1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x;
				manM3_p.y=manPosition.y+1;
	            break;
	        case 2:/*��*/
	            manM1_p.x=manPosition.x;
				manM1_p.y=manPosition.y+1;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x;
				manM3_p.y=manPosition.y-1;
	            break;
	        case 3:/*��*/
	            manM1_p.x=manPosition.x-1;
				manM1_p.y=manPosition.y;
				manM2_p.x=manPosition.x;
				manM2_p.y=manPosition.y;
				manM3_p.x=manPosition.x+1;
				manM3_p.y=manPosition.y;
	            break;
	        case 4:/*��*/
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
	            case N_AISLE:	//0:����
	            case N_AIM: 	//2:Ŀ��
	                 moveMan(manM1_p,map);
				     break;
				default:
					break;
			}
			if(ismove[time]==1){
				switch(map[manM3_p.y][manM3_p.x]){
	            case N_BOX:		//3:δ��Ŀ��λ�õ�����
	            case N_BOX_OK:	//5:����Ŀ��λ�õ�����
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
    printf("���������ܲ�����¼��%d     ",Record);
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
