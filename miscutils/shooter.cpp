#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

//variaveis globais
COORD coord = {0, 0}; // sets coordinates to 0,0
bool endGame=false;
bool temTiro=false;
int auxTiro=1;
int ciclos=0;
int tiroAux=0;
bool tiroUnico=false;

void gotoxy (int x, int y)
{
        coord.X = x; coord.Y = y; // X and Y coordinates
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void textcolor (int iColor)
{
    HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    BOOL b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
    bufferInfo.wAttributes &= 0x00F0;
    SetConsoleTextAttribute (hl, bufferInfo.wAttributes |= iColor);
/*
0 – preto       - black
1 – Azul        - blue
2 – Verde       - green
3 – Cian        - cyan
4 – Vermelho    - red
5 – Roxo        - purple
6 – Amarelo     - yellow
7 – Branco      - white
8 – Cinza       - gray
9 – Azul claro  - light blue
*/
}
struct nave
{
    // Estruturas das naves
    int posX=25, posY=23, numTiros;

}nave,x1,x2,x3;

void desenhaBordas()
{

    //desenha bordas do game
    textcolor(9);
    int i=0;
    for(i=2;i<50;i++)
    {
        gotoxy(i,2);
        printf("%c",176);
        gotoxy(i,25);
        printf("%c",176);
    }

    for(i=2;i<=25;i++)
    {
        gotoxy(2,i);
        printf("%c",176);
        gotoxy(50,i);
        printf("%c",176);
    }
    textcolor(7);
}

void desenhaNave()
{
    textcolor(2); // changes the color
    //desenha a nossa nave - draws our spaceship
    gotoxy(nave.posX-1,nave.posY);
    printf("%c",219);
    gotoxy(nave.posX+1,nave.posY);
    printf("%c",219);
    gotoxy(nave.posX,nave.posY);
    printf("%c",206);
    gotoxy(nave.posX,nave.posY-1);
    printf("%c",30);

    textcolor(7); // sets the color to white

}
void desenhaInimigos()
{
    //desenha as 3 naves inimigas de uma vez só
    textcolor(4);
    gotoxy(x1.posX-1,x1.posY);
    printf("%c",219);
    gotoxy(x1.posX+1,x1.posY);
    printf("%c",219);
    gotoxy(x1.posX,x1.posY);
    printf("%c",206);
    gotoxy(x1.posX,x1.posY+1);
    printf("%c",31);

    gotoxy(x2.posX-1,x2.posY);
    printf("%c",219);
    gotoxy(x2.posX+1,x2.posY);
    printf("%c",219);
    gotoxy(x2.posX,x2.posY);
    printf("%c",206);
    gotoxy(x2.posX,x2.posY+1);
    printf("%c",31);

    gotoxy(x3.posX-1,x3.posY);
    printf("%c",219);
    gotoxy(x3.posX+1,x3.posY);
    printf("%c",219);
    gotoxy(x3.posX,x3.posY);
    printf("%c",206);
    gotoxy(x3.posX,x3.posY+1);
    printf("%c",31);

    textcolor(7);

}

void direcaoInimigos()
{
    // calcula aleatoriamente a direção dos inimigos
    // calculates a random movement for the enemies
    srand(time(0)); // to get real random numbers
    int dir=rand()%4;

    dir=rand()%4;
    switch(dir)
    {
        case 0: if(x2.posX<48)  {  x2.posX++; } break;
        case 1: if(x2.posX>4)   {  x2.posX--; } break;
        case 2: if(x2.posY<24)  {  x2.posY++; } break;
        case 3: if(x2.posY>4)   {  x2.posY--; } break;

    }

    dir=rand()%4;
    switch(dir)
    {
        case 0: if(x2.posX<48) {  x2.posX++; } break;
        case 1: if(x2.posX>4)  {  x2.posX--; } break;
        case 2: if(x2.posY<24) {  x2.posY++; } break;
        case 3: if(x2.posY>4)  {  x2.posY--; } break;

    }
    dir=rand()%4;
    switch(dir)
    {
        case 0: if(x3.posX<48) {  x3.posX++; } break;
        case 1: if(x3.posX>4)  {  x3.posX--; } break;
        case 2: if(x3.posY<24) {  x3.posY++; } break;
        case 3: if(x3.posY>4)  {  x3.posY--; } break;

    }
}

void controle()
{
    char x;
    if(kbhit())
    {
        x=getch();
    }

    switch(x)
    {
        case 'a': if(nave.posX>4)  { nave.posX--; } break;
        case 's': if(nave.posY<24) { nave.posY++; } break;
        case 'd': if(nave.posX<48) { nave.posX++; } break;
        case 'w': if(nave.posY>4)  { nave.posY--; } break;
        case 32:  nave.numTiros++; temTiro=true; tiroUnico=true;   break; //spacebar
        case 27: { gotoxy(2,26); exit(0); endGame=true; } break;        //esc
    }
    fflush(stdin);
}

void printPainel()
{
    // imprime informações do painel lateral - Print information
    gotoxy(52,10);
    printf("ESC - exit");
    gotoxy(52,11);
    printf("W,A,S,D-move");
    gotoxy(52,12);
    printf("Space- shoot");
    gotoxy(52,13);
    printf("Shoots %d", nave.numTiros);
    gotoxy(52,14);
    printf("Ciclos %d", ciclos);

}

void tiro()
{
    // função que cuida do tiro da nave do jogador
    // function that creates the shoot of the player
    if(temTiro==true)
    {
        if(tiroUnico==true) { tiroAux=nave.posX; tiroUnico=false; }
        auxTiro++;
        gotoxy(tiroAux,nave.posY-auxTiro);
        textcolor(6);
        printf("%c",30);
        textcolor(7);
    }
    if(auxTiro>18)
    {
        temTiro=false;
        auxTiro=1;
        tiroUnico=false;
    }
}

void posicoesIniciais()
{
    // inicializa as naves com posições iniciais
    // initialize the positions
    nave.posX=25;
    nave.posY=23;
    x1.posX=25;
    x1.posY=4;
    x2.posX=30;
    x2.posY=7;
    x3.posX=6;
    x3.posY=4;
}

int main()
{
    system("mode 65,28");
    system("title C language Spaceship Shooter Console Mode");
    posicoesIniciais();
    while(endGame==false)
    {

        if(ciclos%2==0) 
        { 
                direcaoInimigos(); 
        }
        desenhaBordas();
        desenhaNave();      //draws our spaceship
        desenhaInimigos();  //draws the enemies

        tiro();
        controle();   // checks if key is pressed
        printPainel();
        Sleep(100);  // pauses excecution for 100 miliseconds

        system("cls"); // clears the screen

        ciclos++;
    }

    return 0;
}
