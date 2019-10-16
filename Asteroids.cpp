#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>
#include <sstream> 

#define VELOCITY 	2.0f

using namespace std;
using namespace olc;

struct LINE
{
	float x1, y1, x2, y2;
	float px, py;
};

class Asteroids : public PixelGameEngine
{
private:
	float Angle;



public:
	Asteroids()
	{
		sAppName = "Asteroids";
	}
public:

	float RotAngle;
	LINE line;
	float CalcBallsQuadDistance(float x1_pos, float y1_pos, float x2_pos, float y2_pos, bool Sqrt)
	{
		if(Sqrt)
			return sqrtf(((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
		else
			return (((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
	}



	bool OnUserCreate() override
	{
		Angle = 0;
		line.x1 = ScreenWidth() / 4;
		line.y1 = ScreenHeight() / 4;
		line.x2 =   3 * ScreenWidth() / 4;
		line.y2 = ScreenHeight() / 4;
		line.px = ScreenWidth() / 4;
		line.py = ScreenHeight()/4;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		float RadAngle = Angle * M_PI / 180;	
		LINE NewLine;
		NewLine.x1 = line.x1 * cos(RadAngle) - line.y1 * sin(RadAngle);
		NewLine.y1 = line.x1 * sin(RadAngle) + line.y1 * cos(RadAngle);
		NewLine.x2 = line.x2 * cos(RadAngle) - line.y2 * sin(RadAngle);
		NewLine.y2 = line.x2 * sin(RadAngle) + line.y2 * cos(RadAngle);	
		NewLine.x1 *= 0.8;
		NewLine.y1 *= 0.8;
		NewLine.x2 *= 0.8;
		NewLine.y2 *= 0.8;			
		NewLine.x1 += line.px;
		NewLine.y1 += line.py;
		NewLine.x2 += line.px;
		NewLine.y2 += line.py;	
		if(Angle <= 60)
			Angle += fElapsedTime * 30.0;
		else
			Angle = 0.0f;
		DrawLine(NewLine.x1, NewLine.y1, NewLine.x2, NewLine.y2, WHITE);
		return true;
	}
};
int main()
{
	Asteroids Asteroid;
	if (Asteroid.Construct(150, 150, 4, 4))
		Asteroid.Start();
	return 0;
}