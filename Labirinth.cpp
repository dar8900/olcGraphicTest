#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>
#include <sstream> 

using namespace std;
using namespace olc;


#define VELOCITY	60.0f


typedef struct 
{
	float px, py;
	float vx, vy;
}MAP_POINT;

class Labirinth : public PixelGameEngine
{
private:
	vector<MAP_POINT> Map;
	MAP_POINT MapPt;
	int Direction;
	bool Manual;
	
public:
	Labirinth()
	{
		sAppName = "Labirinth";
	}
public:
	bool OnUserCreate() override
	{
		MapPt.px = ScreenWidth() / 2;
		MapPt.py = ScreenHeight() / 2;
		Manual = false;
		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		if(GetMouse(0).bPressed)
			Manual = !Manual;
		if(!Manual)
		{
			Direction = rand()%10;
			switch(Direction)
			{
				case 0:
					MapPt.vx = 0.0f;
					MapPt.vy = -VELOCITY;			
					break;
				case 1:
					MapPt.vx = 0.0f;
					MapPt.vy = VELOCITY;				
					break;
				case 2:
					MapPt.vx = -VELOCITY;
					MapPt.vy = 0.0f;				
					break;
				case 3:	
					MapPt.vx = VELOCITY;
					MapPt.vy = 0.0f;				
					break;
				case 4:
					MapPt.vx = 0.0f;
					MapPt.vy = -VELOCITY;			
					break;
				case 5:
					MapPt.vx = 0.0f;
					MapPt.vy = VELOCITY;				
					break;
				case 6:
					MapPt.vx = -VELOCITY;
					MapPt.vy = 0.0f;				
					break;
				case 7:	
					MapPt.vx = VELOCITY;
					MapPt.vy = 0.0f;				
					break;				
				default:			
					break;
			}
		}
		else
		{
			if((GetKey(UP).bPressed || GetKey(UP).bHeld) && MapPt.vy == 0.0f)
			{
				MapPt.vx = 0.0f;
				MapPt.vy = -VELOCITY;
			}
			if((GetKey(DOWN).bPressed || GetKey(DOWN).bHeld) && MapPt.vy == 0.0f)
			{
				MapPt.vx = 0.0f;
				MapPt.vy = VELOCITY;			
			}
			if((GetKey(LEFT).bPressed || GetKey(LEFT).bHeld) && MapPt.vx == 0.0f)
			{
				MapPt.vx = -VELOCITY;
				MapPt.vy = 0.0f;			
			}
			if((GetKey(RIGHT).bPressed || GetKey(RIGHT).bHeld) && MapPt.vx == 0.0f)
			{
				MapPt.vx = VELOCITY;
				MapPt.vy = 0.0f;			
			}
		}
		if(MapPt.vx != 0.0f || MapPt.vy != 0.0f)
		{
			MapPt.px += MapPt.vx * fElapsedTime;
			MapPt.py += MapPt.vy * fElapsedTime;
			if(MapPt.px < 0)
				MapPt.px = ScreenWidth();
			else if(MapPt.px > ScreenWidth())
				MapPt.px = 0;
			if(MapPt.py < 0)
				MapPt.px = ScreenHeight();
			else if(MapPt.py > ScreenHeight())
				MapPt.px = 0;			
			Map.push_back({MapPt.px, MapPt.py});
			if(Manual)
			{			
				MapPt.vx = 0.0f;
				MapPt.vy = 0.0f;		
			}	
		}
		for(auto pixel:Map)
		{
			Draw(pixel.px, pixel.py, Pixel(0, rand()%255, rand()%255));
		}
		
		return true;
	}
};
int main()
{
	srand(time(NULL));
	Labirinth labi;
	if (labi.Construct(100, 100, 4, 4))
		labi.Start();
	return 0;
}