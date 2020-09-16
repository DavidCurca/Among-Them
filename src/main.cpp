#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <math.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 550
#define TICK_INTERVAL  30

using namespace std;

bool quit = false;
SDL_Window *window;
SDL_Renderer *renderer = NULL;
SDL_Surface* screenSurface = NULL;
TTF_Font *font;
static Uint32 next_time;
int wantedColor, totolNumber = 2, xMouse = -50, yMouse = -50;
int numKills, gameMode = 4, hideMap[100][100], numTime = 5, numHigh;
SDL_Rect ImgRect;
Uint32 start = 0, timeStart;

void load_highscore(int &x){
  // ifstream fin("among.txt");
  // fin >> x;
  // fin.close();
}

void write_highscore(int x){
  // ofstream fout("among.txt");
  // fout << x;
  // fout.close();
}

bool check_collision( SDL_Rect A, SDL_Rect B )
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;
    if( bottomA <= topB )
    {
        return false;
    }
    
    if( topA >= bottomB )
    {
        return false;
    }
    
    if( rightA <= leftB )
    {
        return false;
    }
    
    if( leftA >= rightB )
    {
        return false;
    }
    return true;
}

class Person{
public:
  int x = 0, y = 0, color, directionX = 0, directionY = 0;
  void draw();
  void create();
  void setDirection();
  void setColor(int x){
    color = x;
  }
  void setXY(int nx, int ny){
    x = nx;
    y = ny;
  }
  void move(){
    SDL_Rect player = {x, y, 32, 64};
    SDL_Rect left = {211, 0, 7, WINDOW_HEIGHT};
    SDL_Rect right = {993, 0, 7, WINDOW_HEIGHT};
    SDL_Rect top = {210, 0, WINDOW_WIDTH, 7};
    SDL_Rect bottom = {210, 543, WINDOW_WIDTH, 7};
    x += directionX;
    y += directionY;
    while(check_collision(player, left) || check_collision(player, right) || check_collision(player, top) || check_collision(player, bottom)){
      setDirection();
      player.x += directionX;
      player.y += directionY;
    }
  }
};
Person persons[202];

Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

void draw_cutsomline(int mode, int x, int y, int w, int h){
	if(mode == 1){
		// horizontal
  	SDL_Rect fillRect = { x, y, w, h };
  	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );

  	fillRect = { x, y, w, 1 };
  	SDL_SetRenderDrawColor( renderer, 128, 128, 128, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );

  	fillRect = { x, y+h, w, 1 };
  	SDL_SetRenderDrawColor( renderer, 77, 77, 77, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );

	}else{
		// vertical
		SDL_Rect fillRect = { x, y, w, h };
  	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );

		fillRect = { x, y, 1, h };
  	SDL_SetRenderDrawColor( renderer, 77, 77, 77, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );

  	fillRect = { x+w, y, 1, h };
  	SDL_SetRenderDrawColor( renderer, 128, 128, 128, 255 );
  	SDL_RenderFillRect( renderer, &fillRect );
	}
}

void draw_customrect(int x, int y, int w, int h, int l){
  SDL_Rect fillRect = { x, y, w, h };
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  SDL_RenderFillRect( renderer, &fillRect );

  fillRect = { x+l, y+l, w-(l*2), h-(l*2) };
  SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
  SDL_RenderFillRect( renderer, &fillRect );
}

void draw_centertext(int y, int textSize, const char* textStr){
  SDL_Color color = {0, 0, 0, 255};
  font = TTF_OpenFont("arial.ttf", textSize);
  SDL_Surface *textSurface = TTF_RenderText_Solid(font, textStr, color);
  SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_Rect textRect;
  textRect.x = WINDOW_WIDTH/2-(textRect.w/2); textRect.y = y;
  SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);
  textRect.x = WINDOW_WIDTH/2-(textRect.w/2); textRect.y = y;
  textSurface = nullptr;
  SDL_RenderCopy(renderer, text, NULL, &textRect);
  TTF_CloseFont(font);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(text);
}

void draw_text(int x, int y, const char* textStr, int colorInt, int textSize, int mode){
  font = TTF_OpenFont("arial.ttf", textSize);
  SDL_Color color = {0, 0, 0, 255};
  if(colorInt == 1){
    color = {255, 255, 255, 255};
  }
  if(mode == 1){
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  }
  SDL_Surface *textSurface = TTF_RenderText_Solid(font, textStr, color);
  SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_Rect textRect;
  textRect.x = x; textRect.y = y;
  SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);
  textSurface = nullptr;
  SDL_RenderCopy(renderer, text, NULL, &textRect);
  TTF_CloseFont(font);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(text);
}

void draw_image(int x, int y, int w, int h, const char* path){
  ImgRect.x = x; 
  ImgRect.y = y; 
  ImgRect.w = w;
  ImgRect.h = h;
  SDL_Texture* texture = NULL;
  SDL_Surface* temp = IMG_Load(path);
  texture = SDL_CreateTextureFromSurface(renderer, temp);
  SDL_RenderCopy(renderer, texture, NULL, &ImgRect);
  SDL_FreeSurface(temp);
  SDL_DestroyTexture(texture);
}

void Person::draw(){
  draw_image(x+7, y+41, 18, 18, "res/bottom.png");
  if(color == 1){
    draw_image(x+7, y+13, 18, 40, "res/red.png");
  }else if(color == 2){
    draw_image(x+7, y+13, 18, 40, "res/blue.png");
  }else if(color == 3){
    draw_image(x+7, y+13, 18, 40, "res/yellow.png");
  }else if(color == 4){
    draw_image(x+7, y+13, 18, 40, "res/green.png");
  }
  draw_image(x, y, 32, 32, "res/face.png");
}

void Person::setDirection(){
  directionY = (rand() % 2);
  directionX = (rand() % 2);
  if(directionY == 0){
    directionY = -1;
  }else{
    directionY = 1;
  }
  if(directionX == 0){
    directionX = -1;
  }else{
    directionX = 1;
  }
}

void Person::create(){
  x = 210 + ((rand() % 740) + 1), y = (rand() % 480) + 1;
  color = (rand() % 4) + 1;
  setDirection();
}

void draw_sidebar(){

  draw_cutsomline(2, 210, 0, 7, WINDOW_HEIGHT);
  draw_customrect(20, 60, 170, 200, 10);
  draw_customrect(40, 80, 130, 130, 7);
  draw_text(32, 215, "WANTED", 0, 30, 1);

  SDL_Rect fillRect = { 20, 270, 80, 100 };
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  SDL_RenderFillRect( renderer, &fillRect );
  draw_text(35, 275, "TIME", 1, 20, 2);
  char stime[] = "00";
  stime[0] = (char)(numTime/10%10)+'0';
  stime[1] = (char)(numTime%10)+'0';
  draw_text(22, 292, stime, 1, 70, 2);

  fillRect = { 110, 270, 80, 100 };
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  SDL_RenderFillRect( renderer, &fillRect );
  draw_text(116, 275, "SCORE", 1, 20, 2);
  char skills[] = "00";
  skills[0] = (char)(numKills/10%10)+'0';
  skills[1] = (char)(numKills%10)+'0';
  draw_text(112, 292, skills, 1, 70, 2);

  draw_customrect(20, 395, 170, 40, 4);
  fillRect = { 130, 390, 60, 50 };
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
  SDL_RenderFillRect( renderer, &fillRect );
  draw_text(25, 405, "HIGHSCORE", 2, 17, 2);
  char shigh[] = "00";
  shigh[0] = (char)(numHigh/10%10)+'0';
  shigh[1] = (char)(numHigh%10)+'0';
  draw_text(144, 398, shigh, 1, 30, 2);

  int xcenter = 20+85;
  draw_customrect(xcenter/2, 445, 100, 40, 4);
  draw_text(xcenter/2+7, 455, "GO BACK", 2, 17, 1);

  draw_image(89, 162, 32, 32, "res/bottom.png");
  if(wantedColor == 1){
    draw_image(89, 135, 32, 50, "res/red.png");
  }else if(wantedColor == 2){
    draw_image(89, 135, 32, 50, "res/blue.png");
  }else if(wantedColor == 3){
    draw_image(89, 135, 32, 50, "res/yellow.png");
  }else if(wantedColor == 4){
    draw_image(89, 135, 32, 50, "res/green.png");
  }
  draw_image(80, 95, 50, 50, "res/face.png");
}

bool check_valid(int n){
  SDL_Rect rect = {persons[n].x, persons[n].y, 32, 64};
  for(int i = 1; i <= totolNumber; i++){
    if(i == n)
      continue;
    SDL_Rect other = {persons[i].x, persons[i].y, 32, 64};
    if(check_collision(rect, other)){
      return false;
    }
  }
  return true;
}

void updatePeople(int n){
  for(int i = 1; i <= n; i++){
    persons[i].create();
    while((persons[i].color == wantedColor)){
      persons[i].create();
    }
  }
  while(!check_valid(1)){
    persons[1].create();
  }
  persons[1].setColor(wantedColor);
}


void black_screen(){

  int mapX = -10, mapY = -10;
  int xoffset = 210, yoffset = 0;
  for(int i = 1; i <= 62; i++){
    xoffset = 210;
    for(int j = 1; j <= 62; j++){
      SDL_Rect mouseRect = { xMouse-2, yMouse-2, 5, 5 };
      SDL_Rect fillRect = { xoffset, yoffset, 21, 21 };
      if(check_collision(fillRect, mouseRect)){
        mapX = i; mapY = j;
      }
      xoffset += 21;
    }
    yoffset += 21;
  }
  for(int i = 1; i <= 55; i++){
    for(int j = 1; j <= 55; j++){
      hideMap[i][j] = 0;
    }
  }

  yoffset = 0;
  for(int i = 1; i <= 62; i++){
    xoffset = 210;
    for(int j = 1; j <= 62; j++){
      if(i == mapX && j == mapY+3){
      }else if(i == mapX && j == mapY-3){
      }else if(i == mapX && j == mapY+2){
      }else if(i == mapX && j == mapY-2){
      }else if(i == mapX && j == mapY+1){
      }else if(i == mapX && j == mapY-1){
      }else if(i == mapX && j == mapY){
      }else if(i == mapX-1 && j == mapY+1){
      }else if(i == mapX+1 && j == mapY+1){
      }else if(i == mapX-1 && j == mapY-1){
      }else if(i == mapX+1 && j == mapY-1){
      }else if(i == mapX-1 && j == mapY+2){
      }else if(i == mapX+1 && j == mapY+2){
      }else if(i == mapX-1 && j == mapY-2){
      }else if(i == mapX+1 && j == mapY-2){
      }else if(i == mapX-2 && j == mapY-1){
      }else if(i == mapX-2 && j == mapY+1){
      }else if(i == mapX+2 && j == mapY-1){
      }else if(i == mapX+2 && j == mapY+1){
      }else if(i == mapX+3 && j == mapY+1){
      }else if(i == mapX+3 && j == mapY-1){
      }else if(i == mapX-3 && j == mapY+1){
      }else if(i == mapX-3 && j == mapY-1){
      }else if(i == mapX-2 && j == mapY-2){
      }else if(i == mapX-2 && j == mapY+2){
      }else if(i == mapX-1 && j == mapY-3){
      }else if(i == mapX-1 && j == mapY+3){
      }else if(i == mapX+2 && j == mapY-2){
      }else if(i == mapX+2 && j == mapY+2){
      }else if(i == mapX+1 && j == mapY-3){
      }else if(i == mapX+1 && j == mapY+3){
      }else if(i == mapX+3 && j == mapY){
      }else if(i == mapX-3 && j == mapY){
      }else if(i == mapX+2 && j == mapY){
      }else if(i == mapX-2 && j == mapY){
      }else if(i == mapX+1 && j == mapY){
      }else if(i == mapX-1 && j == mapY){
      }else{
        SDL_Rect fillRect = { xoffset, yoffset, 21, 21 };
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderFillRect( renderer, &fillRect );
      }
      xoffset += 21;
    }
    yoffset += 21;
  }
}

int main(int argc, char* argv[]){
  load_highscore(numHigh);
  HWND windowHandle = GetConsoleWindow();
  ShowWindow(windowHandle,SW_HIDE);
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
  IMG_Init(IMG_INIT_PNG);
  srand(time(NULL));
  wantedColor = (rand() % 4) + 1;
  updatePeople(totolNumber);

  next_time = SDL_GetTicks() + TICK_INTERVAL;
  start = SDL_GetTicks();
  timeStart = SDL_GetTicks();
	window = SDL_CreateWindow("Among Them", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Surface* icon = IMG_Load("res/icon.png");
  SDL_SetWindowIcon(window, icon);
  screenSurface = SDL_GetWindowSurface( window );
  SDL_ShowCursor(SDL_DISABLE);

  while( !quit ){

		// Clear Window
		SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0 );
	  SDL_RenderClear( renderer );
    if(numTime <= 0){
      gameMode = 5;
      numTime = 1;
    }

		SDL_Event event;
	  while( SDL_PollEvent( &event ) != 0 ){
        if( event.type == SDL_QUIT )
        {
            quit = true;

        }
        if( event.type == SDL_MOUSEMOTION)
        {
            SDL_GetMouseState(&xMouse,&yMouse);
        }
        if( event.type == SDL_MOUSEBUTTONDOWN){
          int xcenter = 20+85;
          SDL_Rect rect = {persons[1].x, persons[1].y, 32, 64};
          SDL_Rect back_rect = {xcenter/2, 445, 100, 40};
          SDL_Rect aim = {xMouse, yMouse, 40, 40};
          if(gameMode != 0 && gameMode != 5){
            if(check_collision(back_rect, aim)){
              gameMode = 0;
            }else if(check_collision(rect, aim)){
              numKills++;
              if(numKills > numHigh){
                write_highscore(numKills);
                load_highscore(numHigh);
              }
              numTime += 5;
              totolNumber += 8;
              wantedColor = (rand() % 4) + 1;
              if(totolNumber == 74){
                totolNumber = 2;
                gameMode++;
              }
              updatePeople(totolNumber);
            }else{
              numTime -= 5;
            }
          }else if(gameMode == 0){
            SDL_Rect mouseRect = { xMouse-2, yMouse-2, 5, 5 };
            SDL_Rect play_button = {WINDOW_WIDTH/2-100, 290, 200, 55};
            if(check_collision(play_button, mouseRect)){
                gameMode = 1;
                numTime = 5;
                numKills = 0;
            }
          }else if(gameMode == 5){
            SDL_Rect mouseRect = { xMouse-2, yMouse-2, 5, 5 };
            SDL_Rect play_button = {WINDOW_WIDTH/2-100, 260, 200, 55};
            SDL_Rect back_button = {WINDOW_WIDTH/2-100, 330, 200, 55};
            if(check_collision(play_button, mouseRect)){
              gameMode = 1;
              numTime = 5;
              numKills = 0;
            }else if(check_collision(back_button, mouseRect)){
              gameMode = 0;
              numTime = 5;
              numKills = 0;
            }
          }
        }
    }

    if(gameMode == 0){
      Person left;
      left.create();
      left.setXY(380, 120);
      left.setColor(1);
      left.draw();

      Person right;
      right.create();
      right.setXY(590, 120);
      right.setColor(2);
      right.draw();
      SDL_ShowCursor(SDL_ENABLE);
      draw_centertext(50, 60, "Among");
      draw_centertext(120, 60, "Them");
      draw_customrect(WINDOW_WIDTH/2-100, 290, 200, 55, 4);

      draw_centertext(305, 20, "Play");

      SDL_RenderPresent( renderer );
    }else if(gameMode == 5){
      SDL_ShowCursor(SDL_ENABLE);
      draw_centertext(50, 60, "Game Over!");
      char stext[] = "Your Score Was: 00";
      stext[16] = (char)(numKills/10%10)+'0';
      stext[17] = (char)(numKills%10)+'0';
      draw_centertext(130, 25, stext);
      draw_customrect(WINDOW_WIDTH/2-100, 260, 200, 55, 4);
      draw_customrect(WINDOW_WIDTH/2-100, 330, 200, 55, 4);
      draw_centertext(275, 20, "Play Again");
      draw_centertext(345, 20, "Back To Menu");
      SDL_ShowCursor(SDL_ENABLE);

      SDL_RenderPresent( renderer );
    }else{
      SDL_ShowCursor(SDL_DISABLE);
      draw_sidebar();
      for(int i = 1; i <= totolNumber; i++){
        persons[i].draw();
      }
      draw_image(xMouse-20, yMouse-20, 40, 40, "res/aim.png");
      if(gameMode == 3){
        black_screen();
      }

      SDL_RenderPresent( renderer );
      SDL_RenderClear( renderer );

      if(SDL_GetTicks() - start >= 5){
        start = SDL_GetTicks();
        if(gameMode == 2 || gameMode == 3){
          for(int i = 1; i <= totolNumber; i++){
            persons[i].move();
          }
        }
      }

      if(SDL_GetTicks() - timeStart >= 600){
        timeStart = SDL_GetTicks();
        numTime--;
      }
    }
    SDL_Delay(time_left());
      next_time += TICK_INTERVAL;
  }

  SDL_DestroyWindow(window);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

	return 0;
}