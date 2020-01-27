#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>
#include <sstream>
using namespace std;
using namespace olc;

#define N_BRICKS_LINE	3

#define SPEED_INCREASE	15

typedef struct 
{
	float bar_x0;
	float bar_y0;
	float bar_w;
	float bar_h;
	float bar_center_x;
	float bar_center_y;
	float bar_vx;	
	Pixel bar_color;
}BAR;

typedef struct 
{
	float ball_x;
	float ball_y;
	float ball_vx;
	float ball_vy;
	float ball_rad;
}BALL;

BAR BounceBar;
BALL WreckingBall;
vector<BAR> Bricks;


class ATARI_BO : public olc::PixelGameEngine
{
public:
	int32_t Lifes = 3;
	void Reset()
	{	
		CreateBounceBar();
		CreateBall();
	}

	void GameOver()
	{
		if(Lifes == 0)
		{
			DrawString(ScreenHeight() / 4, ScreenHeight() / 3, "You Lost!!");
			if(GetKey(SPACE).bPressed)
			{
				Lifes = 3;
				Reset();
				CreateBriks();
			}
		}	
		if(Bricks.size() == 0)
		{
			DrawString(ScreenHeight() / 4, ScreenHeight() / 3, "You Win!!");
			if(GetKey(SPACE).bPressed)
			{
				Lifes = 3;
				Reset();
				CreateBriks();
			}			
		}	
	}

	void CreateBall()
	{
		WreckingBall.ball_x = BounceBar.bar_center_x + 8;
		WreckingBall.ball_y = BounceBar.bar_center_y - 15;
		WreckingBall.ball_rad = 1;
		WreckingBall.ball_vx = -30;
		WreckingBall.ball_vy = 30;
		FillCircle(WreckingBall.ball_x, WreckingBall.ball_y, WreckingBall.ball_rad, WHITE);
	}

	void CreateBounceBar()
	{
		BounceBar.bar_center_x = ScreenWidth() / 2;
		BounceBar.bar_w = (25 * ScreenWidth()) / 100;
		BounceBar.bar_h = 4;
		BounceBar.bar_center_y = ScreenHeight() - (BounceBar.bar_h / 2) - 1;
		BounceBar.bar_x0 = BounceBar.bar_center_x - (BounceBar.bar_w / 2);
		BounceBar.bar_y0 = BounceBar.bar_center_y - (BounceBar.bar_h / 2);
		FillRect(BounceBar.bar_x0, BounceBar.bar_y0, BounceBar.bar_w, BounceBar.bar_h, WHITE);
		Draw(BounceBar.bar_center_x, BounceBar.bar_center_y, RED);
	}

	void CreateBriks()
	{
		for(int i = 0; i < N_BRICKS_LINE; i++)
		{
			int TotalWidth = 0;
			bool First = true;
			while(TotalWidth < ScreenWidth())
			{
				BAR Brick;
				Brick.bar_w = (float)(rand()%40) + 10;
				Brick.bar_h = 4;
				if(First)
					Brick.bar_x0 = 0;
				else
					Brick.bar_x0 = TotalWidth;
				TotalWidth += (int)Brick.bar_w;
				Brick.bar_y0 = 0 + (Brick.bar_h * i);			
				if(First)
					First = false;
				Brick.bar_center_x = Brick.bar_x0 + (Brick.bar_w / 2);
				Brick.bar_center_y = Brick.bar_y0 + (Brick.bar_h / 2);
				Brick.bar_color = Pixel(rand()%255, rand()%255, rand()%128);
				Bricks.push_back(Brick);
			}
		}
		DrawBricks();
	}

	void DrawBricks()
	{
		for(int i = 0; i < Bricks.size(); i++)
		{
			FillRect(Bricks[i].bar_x0, Bricks[i].bar_y0, Bricks[i].bar_w, Bricks[i].bar_h, Bricks[i].bar_color);			
			// Draw(Bricks[i].bar_center_x, Bricks[i].bar_center_y, WHITE);
		}
	}

	void UpdateBounceBarPos(float fElapsedTime)
	{
		if(GetKey(LEFT).bHeld || GetKey(A).bHeld)
		{
			BounceBar.bar_vx = -80;
		}
		if(GetKey(LEFT).bReleased || GetKey(A).bReleased)
		{
			BounceBar.bar_vx = 0;
		}
		if(GetKey(RIGHT).bHeld || GetKey(D).bHeld)
		{
			BounceBar.bar_vx = 80;
		}
		if(GetKey(RIGHT).bReleased || GetKey(D).bReleased)
		{
			BounceBar.bar_vx = 0;
		}	
		BounceBar.bar_center_x += BounceBar.bar_vx * fElapsedTime;
		if(BounceBar.bar_center_x - (BounceBar.bar_w / 2) < 0)
			BounceBar.bar_center_x = 0 + (BounceBar.bar_w / 2);
		if(BounceBar.bar_center_x + (BounceBar.bar_w / 2) > ScreenWidth())
			BounceBar.bar_center_x = ScreenWidth() - (BounceBar.bar_w / 2);
		BounceBar.bar_x0 = BounceBar.bar_center_x - (BounceBar.bar_w / 2);
		// BounceBar.bar_y0 = BounceBar.bar_center_y - (BounceBar.bar_h / 2);
		FillRect(BounceBar.bar_x0, BounceBar.bar_y0, BounceBar.bar_w, BounceBar.bar_h, WHITE);
		// Draw(BounceBar.bar_center_x, BounceBar.bar_center_y, RED);		
	}

	void UpdateBallCollisions(float fElapsedTime)
	{
		// Se colpisce la barra
		if(WreckingBall.ball_y + WreckingBall.ball_rad > BounceBar.bar_center_y - (BounceBar.bar_h / 2))
		{
			if((WreckingBall.ball_x + WreckingBall.ball_rad < BounceBar.bar_center_x + (BounceBar.bar_w / 2)) && 
				(WreckingBall.ball_x - WreckingBall.ball_rad > BounceBar.bar_center_x - (BounceBar.bar_w / 2)))
			{
				WreckingBall.ball_vy = -WreckingBall.ball_vy;
				if(WreckingBall.ball_vy > 0)
					WreckingBall.ball_vy += SPEED_INCREASE;
				else
					WreckingBall.ball_vy -= SPEED_INCREASE;				
			}
		} 

		// Interazione con i mattoncini alti
		for(int i = 0; i < Bricks.size(); i++)
		{
			if(WreckingBall.ball_y - WreckingBall.ball_rad < Bricks[i].bar_center_y + (Bricks[i].bar_h / 2) )
			{
				if((WreckingBall.ball_x + WreckingBall.ball_rad < Bricks[i].bar_center_x + (Bricks[i].bar_w / 2)) && 
					(WreckingBall.ball_x - WreckingBall.ball_rad > Bricks[i].bar_center_x - (Bricks[i].bar_w / 2)))
				{
					WreckingBall.ball_vy = -WreckingBall.ball_vy;
					if(WreckingBall.ball_vy > 0)
						WreckingBall.ball_vy += SPEED_INCREASE;
					else
						WreckingBall.ball_vy -= SPEED_INCREASE;
					Bricks.erase(Bricks.begin() + i);
				}
			}
		}

		// Se colpisce il soffitto
		if(WreckingBall.ball_y - WreckingBall.ball_rad < 0)
		{
			WreckingBall.ball_vy = -WreckingBall.ball_vy;
			if(WreckingBall.ball_vy > 0)
				WreckingBall.ball_vy += SPEED_INCREASE;
			else
				WreckingBall.ball_vy -= SPEED_INCREASE;
		}		

		// Se colpisce le pareti laterali
		if(WreckingBall.ball_x - WreckingBall.ball_rad < 0 || WreckingBall.ball_x + WreckingBall.ball_rad > ScreenWidth())
		{
			WreckingBall.ball_vx = -WreckingBall.ball_vx;
			if(WreckingBall.ball_vx > 0)
				WreckingBall.ball_vx += SPEED_INCREASE;
			else
				WreckingBall.ball_vx -= SPEED_INCREASE;			
		}

		//Se la barra non la prende
		if(WreckingBall.ball_y + WreckingBall.ball_rad > ScreenHeight())
		{
			Lifes--;
			Reset();
		} 
		WreckingBall.ball_x += WreckingBall.ball_vx * fElapsedTime;
		WreckingBall.ball_y += WreckingBall.ball_vy * fElapsedTime;	
		if((int)WreckingBall.ball_vy == 0)
			WreckingBall.ball_vy = -2;
		FillCircle(WreckingBall.ball_x, WreckingBall.ball_y, WreckingBall.ball_rad, WHITE);
	}

	ATARI_BO()
	{
		sAppName = "BreakOut";
	}
public:
	bool OnUserCreate() override
	{
		CreateBounceBar();
		CreateBall();
		CreateBriks();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		if(Lifes > 0 && Bricks.size() > 0)
		{
			UpdateBounceBarPos(fElapsedTime);
			UpdateBallCollisions(fElapsedTime);
			DrawBricks();
		}
		GameOver();
		return true;
	}
};
int main()
{
	ATARI_BO BO;
	srand(time(NULL));
	if (BO.Construct(200, 150, 2, 2))
		BO.Start();
	return 0;
}