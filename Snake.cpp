#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>

using namespace std;
using namespace olc;

typedef struct 
{
	float headX;
	float headY;
	float tailX;
	float tailY;
	float lenght;
	float v_x, v_y;
	vector<pair<float,float>> curvPoint;
}SNAKE;

SNAKE snakeGuy;

bool Init = true;

// Override base class with your custom functionality
class SnakeProject : public olc::PixelGameEngine
{
public:
	SnakeProject()
	{
		sAppName = "Ssnake";
	}
public:
	
	void DrawSnakeBody(SNAKE *SnakeBody, bool Init)
	{
		if(Init)
		{
			DrawLine(SnakeBody->headX, SnakeBody->headY, snakeGuy.tailX, snakeGuy.tailY, WHITE);
			Draw(SnakeBody->headX, SnakeBody->headY, RED);		
		}
		else
		{
			int curvPointSize = SnakeBody->curvPoint.size();
			DrawLine(SnakeBody->headX, SnakeBody->headY, SnakeBody->curvPoint[curvPointSize].first, SnakeBody->curvPoint[curvPointSize].second, WHITE);			
			Draw(SnakeBody->headX, SnakeBody->headY, RED);	
			for(int i = curvPointSize; i > 1; i--)
			{
				DrawLine(SnakeBody->curvPoint[i].first, SnakeBody->curvPoint[i].second, SnakeBody->curvPoint[i - 1].first, SnakeBody->curvPoint[i - 1].second, WHITE);			
			}
			DrawLine(SnakeBody->curvPoint[0].first, SnakeBody->curvPoint[0].second, SnakeBody->tailX, SnakeBody->tailY, WHITE);			

		}
	}

	bool OnUserCreate() override
	{
		snakeGuy.headX = (float)(rand()%ScreenWidth());
		snakeGuy.headY = (float)(rand()%ScreenHeight());
		snakeGuy.lenght = 4;
		snakeGuy.tailX = snakeGuy.headX + snakeGuy.lenght;
		snakeGuy.tailY = snakeGuy.headY;
		snakeGuy.v_x = -20;
		snakeGuy.v_y = 0;
		DrawSnakeBody(&snakeGuy, Init);
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);

		if(GetKey(UP).bPressed && snakeGuy.v_y == 0)
		{
			snakeGuy.v_x = 0;
			snakeGuy.v_y = -20;
			snakeGuy.curvPoint.push_back({snakeGuy.headX, snakeGuy.headY});
			Init = false;
		}
		if(GetKey(DOWN).bPressed && snakeGuy.v_y == 0)
		{
			snakeGuy.v_x = 0;
			snakeGuy.v_y = 20;	
			snakeGuy.curvPoint.push_back({snakeGuy.headX, snakeGuy.headY});	
			Init = false;	
		}
		if(GetKey(LEFT).bPressed && snakeGuy.v_x == 0)
		{
			snakeGuy.v_x = -20;
			snakeGuy.v_y = 0;
			snakeGuy.curvPoint.push_back({snakeGuy.headX, snakeGuy.headY});
			Init = false;
		}
		if(GetKey(RIGHT).bPressed && snakeGuy.v_x == 0)
		{
			snakeGuy.v_x = 20;
			snakeGuy.v_y = 0;			
			snakeGuy.curvPoint.push_back({snakeGuy.headX, snakeGuy.headY});
			Init = false;
		}
		// if(GetKey(SPACE).bPressed)
		// {
		// 	if(snakeGuy.v_x < 0)
		// 		snakeGuy.lenght++;
		// 	else
		// 		snakeGuy.lenght--;
		// }

		cout << "headX: " << snakeGuy.headX << endl;
		cout << "headY: " << snakeGuy.headY << endl;
		snakeGuy.headX += snakeGuy.v_x * fElapsedTime;
		snakeGuy.headY += snakeGuy.v_y * fElapsedTime;
		snakeGuy.tailX += snakeGuy.v_x * fElapsedTime;
		snakeGuy.tailY += snakeGuy.v_y * fElapsedTime;
		if(snakeGuy.headX < 0)
			snakeGuy.headX = ScreenWidth();
		if(snakeGuy.tailX < 0)
			snakeGuy.tailX = ScreenWidth();
		if(snakeGuy.headX > ScreenWidth())
			snakeGuy.headX = 0;
		if(snakeGuy.tailX > ScreenWidth())
			snakeGuy.tailX = 0;		
		if(snakeGuy.headY < 0)
			snakeGuy.headY = ScreenHeight();
		if(snakeGuy.headY > ScreenHeight())
			snakeGuy.headY = 0;	
		if(snakeGuy.tailY < 0)
			snakeGuy.tailY = ScreenHeight();
		if(snakeGuy.tailY > ScreenHeight())
			snakeGuy.tailY = 0;					
		DrawSnakeBody(&snakeGuy, Init);
		return true;
	}
};
int main()
{
	srand(time(NULL));
	SnakeProject Ssnake;
	if (Ssnake.Construct(200, 200, 4, 4))
		Ssnake.Start();
	return 0;
}