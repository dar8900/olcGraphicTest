#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <sstream> 
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace olc;

#define FRAMES	 2000

#define A_PARAM  10
#define B_PARAM  10

#define DELAY(ms) (usleep(ms * 1000))

typedef enum 
{
	SPIRACAL = 0,
	ELLIPTICAL,
	CIRCLE,
	MAX_ROTATIONS
}ROTATIONS;


typedef struct 
{
	float x;
	float y;
	float v_ang;
}SPIRAL_COORD;

vector<Pixel> PlanetsColor;
vector<uint8_t> PlanetsRadius;
vector<vector<SPIRAL_COORD>> Planets;

class SPIRAL : public olc::PixelGameEngine
{
public:
	SPIRAL()
	{
		sAppName = "Spiral";
	}
private:
	template<class value> void DrawValueDisplay(float x, float y, value Val)
	{
		stringstream ss;
		ss << Val;
		int32_t _x = (int32_t)x;
		int32_t _y = (int32_t)y;
		DrawString(_x, _y, ss.str());
	}	
public:

	uint32_t Frame = 0;
	uint8_t Rotation = SPIRACAL;
	void FillCooord()
	{
		if(GetKey(SPACE).bPressed)
		{
			SPIRAL_COORD Coord;
			vector<SPIRAL_COORD> Planet;
			uint8_t Radius = (rand()%2 + 1);
			float Angle = 0.0f;
			float a_trasf = (((float)(rand()%100)) / 90);
			float b_trasf = (((float)(rand()%100)) / 100);
			Coord.v_ang = Radius * 0.9;
			for(int i = 0; i < FRAMES; i++)
			{
				Angle = (i * M_PI) / 180;
				switch(Rotation)
				{
					case SPIRACAL:
						Coord.x = (A_PARAM * a_trasf + (B_PARAM * b_trasf * Angle * Coord.v_ang)) * cos(Angle * Coord.v_ang) + (ScreenWidth() / 2);
						Coord.y = (A_PARAM * a_trasf + (B_PARAM * b_trasf * Angle * Coord.v_ang)) * sin(Angle * Coord.v_ang) + (ScreenHeight() / 2);
						break;
					case ELLIPTICAL:
						Coord.x = (A_PARAM * a_trasf) * cos(Angle * Coord.v_ang) + (ScreenWidth() / 2);
						Coord.y = (B_PARAM * b_trasf) * sin(Angle * Coord.v_ang) + (ScreenHeight() / 2);					
						break;
					case CIRCLE:
						Coord.x = (A_PARAM * a_trasf) * cos(Angle * Coord.v_ang) + (ScreenWidth() / 2);
						Coord.y = (A_PARAM * a_trasf) * sin(Angle * Coord.v_ang) + (ScreenHeight() / 2);					
						break;
					default:
						break;
				}
				Planet.push_back(Coord);
			}
			Planets.push_back(Planet);
			PlanetsColor.push_back(Pixel(rand() % 255, rand() % 255, 255));
			PlanetsRadius.push_back(Radius);
		}
		if(GetKey(S).bPressed)
		{
			if(Rotation < MAX_ROTATIONS - 1)
				Rotation++;
			else
				Rotation = 0;
		}
	}

	void CheckReset()
	{
		if(GetKey(R).bPressed)
		{
			Planets.clear();
			PlanetsRadius.clear();
			PlanetsColor.clear();
			Frame = 0;
		}
	}

	void DrawPlanets()
	{
		vector<SPIRAL_COORD> Planet;
		for(int p = 0; p < Planets.size(); p++)
		{
			if(Frame >= FRAMES)
				Frame = 0;
			Planet = Planets[p];
			FillCircle(Planet[Frame].x, Planet[Frame].y, PlanetsRadius[p], PlanetsColor[p]);
		}
		Frame++;
	}

	bool OnUserCreate() override
	{
		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		string Rot = "Rotation: ";
		Clear(BLACK);
		FillCooord();
		DrawPlanets();
		switch(Rotation)
		{
			case SPIRACAL:
				Rot += "spiral";
				break;
			case ELLIPTICAL:
				Rot += "ellipse";
				break;
			case CIRCLE:
				Rot += "circle";
				break;
			default:
				break;
		}
		DrawString(0, 0, Rot);
		CheckReset();
		return true;
	}
};
int main()
{
	SPIRAL spiral;
	srand(time(NULL));
	if (spiral.Construct(200, 200, 2, 2))
		spiral.Start();
	return 0;
}