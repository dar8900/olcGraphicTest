#define OLC_PGE_APPLICATION
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream> 
#include "olcPixelGameEngine.h"

#define BULLET_DIST 	60


using namespace std;
using namespace olc;

struct TRIG
{
	float x[3];
	float y[3];
	float size;
};

struct GAMES_OBJ
{
	float px, py;
	float vx, vy;
	float pAngle;
};

class Rotates : public PixelGameEngine
{
public:
	Rotates()
	{
		sAppName = "Rotate";
	}
	float CalcBallsQuadDistance(float x1_pos, float y1_pos, float x2_pos, float y2_pos, bool Sqrt)
	{
		if(Sqrt)
			return sqrtf(((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
		else
			return (((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
	}	
	void WrapCoordinates(float ix, float iy, float *ox, float *oy)
	{
		*ox = ix;
		*oy = iy;
		if (ix < 0.0f)	*ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())	*ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	*oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) *oy = iy - (float)ScreenHeight();
	}

	void DrawShip()
	{
		TRIG ShipModify;

		// Rotate
		for(int i = 0; i < 3; i++)
		{
			ShipModify.x[i] = Ship.x[i] * cosf(Guy.pAngle) - Ship.y[i] * sinf(Guy.pAngle);
			ShipModify.y[i] = Ship.x[i] * sinf(Guy.pAngle) + Ship.y[i] * cosf(Guy.pAngle);
		}

		// Resize

		for(int i = 0; i < 3; i++)
		{
			ShipModify.x[i] = ShipModify.x[i] * Ship.size;
			ShipModify.y[i] = ShipModify.y[i] * Ship.size;
		}	

		// Traslate
		for(int i = 0; i < 3; i++)
		{
			ShipModify.x[i] = ShipModify.x[i] + Guy.px;
			ShipModify.y[i] = ShipModify.y[i] + Guy.py;
		}

		for(int i = 0; i < 3; i++)
		{
			int j = i + 1;
			DrawLine(ShipModify.x[i % 3], ShipModify.y[i % 3], ShipModify.x[j % 3], ShipModify.y[j % 3]);
		}		
	}

	void DrawTarget()
	{
		for(int j = 0; j < TargetsTrig.size(); j++)
		{
			TRIG TrigModify;

			// Rotate
			for(int i = 0; i < 3; i++)
			{
				TrigModify.x[i] = TargetsTrig[j].x[i] * cosf(Targets[j].pAngle) - TargetsTrig[j].y[i] * sinf(Targets[j].pAngle);
				TrigModify.y[i] = TargetsTrig[j].x[i] * sinf(Targets[j].pAngle) + TargetsTrig[j].y[i] * cosf(Targets[j].pAngle);
			}

			// Resize

			for(int i = 0; i < 3; i++)
			{
				TrigModify.x[i] = TrigModify.x[i] * TargetsTrig[j].size;
				TrigModify.y[i] = TrigModify.y[i] * TargetsTrig[j].size;
			}	

			// Traslate
			for(int i = 0; i < 3; i++)
			{
				TrigModify.x[i] = TrigModify.x[i] + Targets[j].px;
				TrigModify.y[i] = TrigModify.y[i] + Targets[j].py;
			}

			for(int i = 0; i < 3; i++)
			{
				int j = i + 1;
				DrawLine(TrigModify.x[i % 3], TrigModify.y[i % 3], TrigModify.x[j % 3], TrigModify.y[j % 3]);
			}		
		}
	}

private:
	TRIG Ship;
	vector<TRIG> TargetsTrig;
	GAMES_OBJ Guy;
	vector<GAMES_OBJ> Bullets;
	vector<GAMES_OBJ> Targets;
	int n_target = 4;
	bool RestartGame = false;
protected:
	
	// Called by olcConsoleGameEngine
	bool OnUserCreate() override
	{
		Ship.x[0] = 0.0f;
		Ship.x[1] = -2.5f;
		Ship.x[2] = 2.5f;
		Ship.y[0] = -5.0f;
		Ship.y[1] = 2.5f;
		Ship.y[2] = 2.5f;
		Ship.size = 1.0f;
		Guy.px = ScreenWidth() / 2;
		Guy.py = ScreenHeight() / 2;
		Guy.vx = 0.0f;
		Guy.vy = 0.0f;
		Guy.pAngle = 0.0f;
		for(int i = 0; i < n_target; i++)
		{
			TRIG LoadParam;
			LoadParam.x[1] = 3.5; 
			LoadParam.x[2] = 7.6; 
			LoadParam.x[0] = (LoadParam.x[1] + LoadParam.x[2]) / 2;
			LoadParam.y[1] = 3.0; 
			LoadParam.y[2] = 3.0; 
			LoadParam.y[0] = 5.0f; 
			TargetsTrig.push_back({LoadParam.x[0], LoadParam.x[1], LoadParam.x[2], 
						 LoadParam.y[0], LoadParam.y[1], LoadParam.y[2], (float)(rand()% 6)});
			Targets.push_back({(float)(rand()% ScreenWidth()), (float)(rand()% ScreenHeight()), -0.1f, 0.1f, 0.0f});
		}
		return true;
	}

	void Restart()
	{
		Ship.x[0] = 0.0f;
		Ship.x[1] = -2.5f;
		Ship.x[2] = 2.5f;
		Ship.y[0] = -5.0f;
		Ship.y[1] = 2.5f;
		Ship.y[2] = 2.5f;
		Ship.size = 1.0f;
		Guy.px = ScreenWidth() / 2;
		Guy.py = ScreenHeight() / 2;
		Guy.vx = 0.0f;
		Guy.vy = 0.0f;
		Guy.pAngle = 0.0f;
		TargetsTrig.clear();
		Targets.clear();
		for(int i = 0; i < n_target; i++)
		{
			TRIG LoadParam;
			LoadParam.x[1] = 3.5; 
			LoadParam.x[2] = 7.6; 
			LoadParam.x[0] = (LoadParam.x[1] + LoadParam.x[2]) / 2;
			LoadParam.y[1] = 3.0; 
			LoadParam.y[2] = 3.0; 
			LoadParam.y[0] = 5.0f; 
			TargetsTrig.push_back({LoadParam.x[0], LoadParam.x[1], LoadParam.x[2], 
						 LoadParam.y[0], LoadParam.y[1], LoadParam.y[2], (float)(rand()% 6)});
			Targets.push_back({(float)(rand()% ScreenWidth()), (float)(rand()% ScreenHeight()), -0.1f, 0.1f, 0.0f});
		}
	}

	// Called by olcConsoleGameEngine
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		if(GetKey(LEFT).bHeld)
		{
			Guy.pAngle -= 5.0f * fElapsedTime;
		}
		if(GetKey(RIGHT).bHeld)
		{
			Guy.pAngle += 5.0f * fElapsedTime;
		}	
		if(GetKey(UP).bHeld)
		{
			Guy.vx += sin(Guy.pAngle) * 10.0f * fElapsedTime;
			Guy.vy += -cos(Guy.pAngle) * 10.0f * fElapsedTime;			
		}
		Guy.px += Guy.vx * fElapsedTime;
		Guy.py += Guy.vy * fElapsedTime;
		WrapCoordinates(Guy.px, Guy.py, &Guy.px, &Guy.py);
		if(GetKey(SPACE).bPressed)
		{
			// Recoil
			Guy.vx -= sin(Guy.pAngle) * 5.0f * fElapsedTime;
			Guy.vy -= -cos(Guy.pAngle) * 5.0f * fElapsedTime;				
			Bullets.push_back({Guy.px, Guy.py, 50.0f * sinf(Guy.pAngle), -50.0f * cosf(Guy.pAngle), 100.0f });
		}


		for(auto &tar : Targets)
		{
			if(tar.px < Guy.px)
				tar.px += tar.vx * fElapsedTime;
			else
				tar.px -= tar.vx * fElapsedTime;
			if(tar.py < Guy.py)
				tar.py += tar.vy * fElapsedTime;
			else
				tar.py -= tar.vy * fElapsedTime;
			tar.pAngle -= 0.2f * fElapsedTime;
			WrapCoordinates(tar.px, tar.py, &tar.px, &tar.py);
			if(CalcBallsQuadDistance(tar.px, tar.py, Guy.px, Guy.py, true) < 5)
			{
				RestartGame = true;
			}
		}

		if(RestartGame)
		{
			RestartGame = false;
			Restart();
		}
		else
		{
			for(auto &b:Bullets)
			{
				b.px += b.vx * fElapsedTime;
				b.py += b.vy * fElapsedTime;
				WrapCoordinates(b.px, b.py, &b.px, &b.py);
				b.pAngle -= 1.0f * fElapsedTime;
				for(auto &t : Targets)
				{
					float TarBulDist = CalcBallsQuadDistance(b.px, b.py, t.px, t.py, true);
					if(TarBulDist <= 8)
					{
						t.vx -= sin(t.pAngle) * 20.0f * fElapsedTime;
						t.vy -= -cos(t.pAngle) * 20.0f * fElapsedTime;
					}
				}
			}
			for(int i = 0; i < Bullets.size(); i++)
			{
				float Dist = CalcBallsQuadDistance(Bullets[i].px, Bullets[i].py, Guy.px, Guy.py, false);
				if(Dist > (BULLET_DIST * BULLET_DIST))
					Bullets.erase(Bullets.begin() + i);
			}

			// Draw bullets
			for(auto b:Bullets)
			{
				Draw(b.px, b.py);
			}

			DrawShip();
			DrawTarget();
		}
		return true;
	}
};



int main()
{
	Rotates Rot;
	if (Rot.Construct(160, 100, 4, 4))
		Rot.Start();
	return 0;
}