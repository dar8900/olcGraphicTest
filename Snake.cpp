#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>

#define INIT_LENGHT	 8
#define V_INCREMENT	 0.5f

using namespace std;
using namespace olc;



typedef struct 
{
	float headX;
	float headY;
}SNAKE;

vector<SNAKE> snakeGuy;

float AbsVelocity = 1;

// Override base class with your custom functionality
class SnakeProject : public olc::PixelGameEngine
{
public:
	SnakeProject()
	{
		sAppName = "Ssnake";
	}
private:
	float snake_vx, snake_vy;
	int Cycle;
public:
	

	void DrawSnakeBody()
	{
		for(int i = 0; i < snakeGuy.size(); i++)
		{
			if(i == 0)
				FillCircle(snakeGuy[i].headX, snakeGuy[i].headY, 1, RED);
			else
				Draw(snakeGuy[i].headX, snakeGuy[i].headY, WHITE);
		}
	}

	void UpdateVelocity()
	{
		if(snake_vy == 0)
		{
			if(snake_vx > 0)
				snake_vx = AbsVelocity;
			else if(snake_vx < 0)
				snake_vx = -AbsVelocity;
		}
		if(snake_vx == 0)
		{
			if(snake_vy > 0)
				snake_vy = AbsVelocity;
			else if(snake_vy < 0)
				snake_vy = -AbsVelocity;	
		}
	}

	void Restart()
	{
		snakeGuy.clear();
		float h_x = rand()%ScreenWidth();
		float h_y = rand()%ScreenHeight();
		snake_vx = -AbsVelocity;
		snake_vy = 0;
		for(int i = 0; i < INIT_LENGHT; i++)
		{
			snakeGuy.push_back({h_x + (float)i, h_y});
		}
	}

	bool OnUserCreate() override
	{
		snakeGuy.clear();
		float h_x = rand()%ScreenWidth();
		float h_y = rand()%ScreenHeight();
		snake_vx = -AbsVelocity;
		snake_vy = 0;
		for(int i = 0; i < INIT_LENGHT; i++)
		{
			snakeGuy.push_back({h_x + (float)i, h_y});
		}
		DrawSnakeBody();
		return true;
	}



	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);

		if(GetKey(UP).bPressed && snake_vy == 0)
		{
			snake_vx = 0;
			snake_vy = -AbsVelocity;
			// Init = false;
		}
		if(GetKey(DOWN).bPressed && snake_vy == 0)
		{
			snake_vx = 0;
			snake_vy = AbsVelocity;	
			// Init = false;	
		}
		if(GetKey(LEFT).bPressed && snake_vx == 0)
		{
			snake_vx = -AbsVelocity;
			snake_vy = 0;
			// Init = false;
		}
		if(GetKey(RIGHT).bPressed && snake_vx == 0)
		{
			snake_vx = AbsVelocity;
			snake_vy = 0;			
			// Init = false;
		}
		if(GetKey(SPACE).bPressed)
		{
			if(snakeGuy[snakeGuy.size()].headX - snakeGuy[snakeGuy.size() - 1].headX < 0)
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX + 1, snakeGuy[snakeGuy.size()].headY});
			else
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX - 1, snakeGuy[snakeGuy.size()].headY});
			if(snakeGuy[snakeGuy.size()].headY - snakeGuy[snakeGuy.size() - 1].headY < 0)
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX, snakeGuy[snakeGuy.size()].headY + 1});
			else
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX, snakeGuy[snakeGuy.size()].headY - 1});			
		}
		if(GetKey(A).bPressed || GetKey(A).bHeld)
		{
			AbsVelocity += V_INCREMENT;
		}
		if(GetKey(S).bPressed || GetKey(S).bHeld)
		{
			if(AbsVelocity > 0.0f)
				AbsVelocity -= V_INCREMENT;
			else
				AbsVelocity = 0.0f;
		}
		UpdateVelocity();

		// PrevTime += fElapsedTime;
		// cout << PrevTime << endl;
		// if(PrevTime >= 1)
		// {
		// PrevTime = 0;


		for(int i = snakeGuy.size() - 1; i > 0; i--)
		{
			snakeGuy[i].headX = snakeGuy[i - 1].headX;
			snakeGuy[i].headY = snakeGuy[i - 1].headY;
		}	
		if(snake_vx != 0)
		{
			if(snake_vx > 0)
				snakeGuy[0].headX += fabs(snake_vx) *  fElapsedTime;
			else
				snakeGuy[0].headX -= fabs(snake_vx) * fElapsedTime;
		}	
		else
		{
			if(snake_vy > 0)
				snakeGuy[0].headY += fabs(snake_vy) *  fElapsedTime;
			else
				snakeGuy[0].headY -= fabs(snake_vy) * fElapsedTime;
		}

		if(snakeGuy[0].headX < 0)
			snakeGuy[0].headX = ScreenWidth();
		if(snakeGuy[0].headX > ScreenWidth())
			snakeGuy[0].headX = 0;

		if(snakeGuy[0].headY < 0)
			snakeGuy[0].headY = ScreenHeight();
		if(snakeGuy[0].headY > ScreenHeight())
			snakeGuy[0].headY = 0;

		for(int i = 1; i < snakeGuy.size(); i++)
		{
			if(snakeGuy[0].headX == snakeGuy[i].headX && 
				snakeGuy[0].headY == snakeGuy[i].headY)
			{
				Restart();
				break;
			}
		}

		DrawSnakeBody();
		// }
		return true;
	}
};
int main()
{
	srand(time(NULL));
	SnakeProject Ssnake;
	if (Ssnake.Construct(100, 100, 4, 4))
		Ssnake.Start();
	return 0;
}