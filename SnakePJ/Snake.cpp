#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>
#include <sstream> 

#define INIT_LENGHT	 8
#define V_INCREMENT	 1.0f
#define H_HIGH_SCORE 10

using namespace std;
using namespace olc;



typedef struct 
{
	float headX;
	float headY;
}SNAKE;



vector<SNAKE> snakeGuy;

float AbsVelocity = 10;


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
	bool FruitCreated, FruitCached;
	float FruitCoord[2];
	Pixel FruitColor;
	int Score;
	float Time;
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

	void DrawScore(int Score)
	{
		stringstream s, v, l;
		s << Score;
		v << (int)AbsVelocity;
		l << snakeGuy.size();
		string score = "Score: " + s.str() + "  Vel: " + v.str() + "  Len: " + l.str(); 
		DrawString(0, 0, score, WHITE, 1);
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
		float h_y = rand()%(ScreenHeight() - H_HIGH_SCORE);
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
		float h_y = rand()%(ScreenHeight() - H_HIGH_SCORE);
		snake_vx = -AbsVelocity;
		snake_vy = 0;
		FruitCreated = false;
		FruitCached = false;
		FruitColor = WHITE;
		Score = 0;
		Time = 0.0f;
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

		// Aggiorno la velocità
		UpdateVelocity();

		// Aggiorno la posizione del corpo
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

		if(snakeGuy[0].headY < H_HIGH_SCORE)
			snakeGuy[0].headY = ScreenHeight();
		if(snakeGuy[0].headY > ScreenHeight())
			snakeGuy[0].headY = H_HIGH_SCORE;

		// Controllo sovrapposizione corpo
		for(int i = 1; i < snakeGuy.size(); i++)
		{
			if((int)(snakeGuy[0].headX * 1000)== (int)(snakeGuy[i].headX * 1000) && 
				(int)(snakeGuy[0].headY * 1000) == (int)(snakeGuy[i].headY * 1000))
			{
				Restart();
				break;
			}
		}
		Time += fElapsedTime;
		if(Time > 2.0f)
		{
			Time = 0.0f;
			cout << endl;
			for(int i = 0; i < snakeGuy.size(); i++)
			{
				cout << "X: " << (int)snakeGuy[i].headX * 1000 << endl;
				cout << "Y: " << (int)snakeGuy[i].headY * 1000<< endl;
			}
		}
		

		// Creazione e gestione raccolta frutta
		if(!FruitCreated)
		{
			FruitCreated = true;
			FruitCoord[0] = (float)(rand()%ScreenWidth());
			FruitCoord[1] = (float)(rand()%ScreenHeight());
			if(FruitCoord[1] <= 10)
				FruitCoord[1] = 12;
			FruitColor = Pixel(rand() % 255, rand() % 255, rand()% 255);
		}
		if(!FruitCached)
			FillCircle(FruitCoord[0], FruitCoord[1], 1, FruitColor);

		if((int)snakeGuy[0].headX == (int)FruitCoord[0] && (int)snakeGuy[0].headY == (int)FruitCoord[1])
		{
			FruitCached = true;
			FruitCreated = false;
		}
		if(FruitCached)
		{
			FruitCached = false;
			Score++;
			if(snakeGuy[snakeGuy.size()].headX - snakeGuy[snakeGuy.size() - 1].headX < 0)
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX + 1, snakeGuy[snakeGuy.size()].headY});
			else
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX - 1, snakeGuy[snakeGuy.size()].headY});
			if(snakeGuy[snakeGuy.size()].headY - snakeGuy[snakeGuy.size() - 1].headY < 0)
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX, snakeGuy[snakeGuy.size()].headY + 1});
			else
				snakeGuy.push_back({snakeGuy[snakeGuy.size()].headX, snakeGuy[snakeGuy.size()].headY - 1});		
			AbsVelocity += V_INCREMENT;		
		}

		// Disegno corpo
		DrawSnakeBody();

		// Disegna punteggio
		DrawScore(Score);
		
		return true;
	}
};
int main()
{
	srand(time(NULL));
	SnakeProject Ssnake;
	if (Ssnake.Construct(150, 150, 8, 8))
		Ssnake.Start();
	return 0;
}