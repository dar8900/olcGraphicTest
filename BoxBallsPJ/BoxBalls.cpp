#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>

#define WALLS_H		     3
// #define N_BALLS		 	 5
// #define ACC_GRAVITY		 98
#define MAX_VELOCITY	 200
#define FLOOR_MASS		 10000000
#define MAX_TEMP 	     1000
#define RAND_RAD	     10
#define RAND_BALL		 6

using namespace std;
using namespace olc;

// Override base class with your custom functionality

typedef struct 
{
	float x_pos;
	float y_pos;
	float radius;
	float mass;
	float v_x, v_y;
	float x_mom, y_mom;
	float gravityField;
	int Id;
	Pixel color;
}BALL;

enum
{
	W_TOP = 0,
	W_RIGHT,
	W_BOTTOM,
	W_LEFT,
	NO_WALL,
	MAX_WALLS
};

string WallsStr[MAX_WALLS] = 
{
	"Top",
	"Right",
	"Bottom",
	"Left",
	"No wall"
};

vector<BALL> Balls;

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Balls In Box";
	}
public:
	
	BALL *PickedBall = nullptr;
	int N_BALLS = rand()%RAND_BALL;
	float ACC_GRAVITY = 98;
	float BallsRadius = rand()% RAND_RAD;
	float Temperature = 0;
	bool TempChanged = false;
	int Ids = 1;

	void CreateBall(float Px, float Py, float Rad)
	{
		BALL NewBall;
		NewBall.radius = Rad;
		NewBall.mass = (NewBall.radius * 10);
		NewBall.x_pos = Px;
		NewBall.y_pos = Py;
		if(((NewBall.x_pos + NewBall.radius) >= ScreenWidth()) || (NewBall.x_pos - NewBall.radius) <=  0)
			NewBall.x_pos = ScreenWidth() / 2;
		if(((NewBall.y_pos + NewBall.radius) >= ScreenHeight() - WALLS_H) || (NewBall.y_pos - NewBall.radius) <=  0)
			NewBall.y_pos = ScreenHeight() / 2;			
		NewBall.color = Pixel(255, rand() % 255, rand() % 255);
		NewBall.v_x = Temperature * 2;
		NewBall.v_y = Temperature * 2;
		NewBall.Id = Ids++;
		cout << NewBall.Id << endl;
		Balls.push_back(NewBall);		
	}

	void DrawWalls()
	{
		// Top Wall
		FillRect(0, 0, ScreenWidth(), WALLS_H, WHITE);
		// Bottom Wall
		FillRect(0, ScreenHeight() - WALLS_H, ScreenWidth(), WALLS_H, WHITE);
		// Left Wall
		FillRect(0, 0, WALLS_H, ScreenHeight(), WHITE);
		// Right Wall
		FillRect(ScreenWidth() - WALLS_H, 0, WALLS_H, ScreenHeight(), WHITE);
	}

	float CalcBallsQuadDistance(float x1_pos, float y1_pos, float x2_pos, float y2_pos, bool Sqrt)
	{
		if(Sqrt)
			return sqrt(((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
		else
			return (((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
	}

	void DynamicCollision(BALL *Actual, BALL *Target)
	{
		BALL  *b1 = Actual;
		BALL  *b2 = Target;

		// Distance between balls
		float fDistance = sqrtf((b1->x_pos - b2->x_pos)*(b1->x_pos - b2->x_pos) + (b1->y_pos - b2->y_pos)*(b1->y_pos - b2->y_pos));

		// Normal
		float nx = (b2->x_pos - b1->x_pos) / fDistance;
		float ny = (b2->y_pos - b1->y_pos) / fDistance;

		// Tangent
		float tx = -ny;
		float ty = nx;

		// Dot Product Tangent
		float dpTan1 = b1->v_x * tx + b1->v_y * ty;
		float dpTan2 = b2->v_x * tx + b2->v_y * ty;

		// Dot Product Normal
		float dpNorm1 = b1->v_x * nx + b1->v_y * ny;
		float dpNorm2 = b2->v_x * nx + b2->v_y * ny;

		// Conservation of momentum in 1D
		float m1 = (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
		float m2 = (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

		// Update ball velocities
		b1->v_x = tx * dpTan1 + nx * m1;
		b1->v_y = ty * dpTan1 + ny * m1;
		b2->v_x = tx * dpTan2 + nx * m2;
		b2->v_y = ty * dpTan2 + ny * m2;
	}


	void BallsOverlap(BALL *Actual, BALL *Target)
	{
		float qDistance = CalcBallsQuadDistance(Actual->x_pos, Actual->y_pos, Target->x_pos, Target->y_pos, false);
		float qRadiusSum = ((Actual->radius + Target->radius) * (Actual->radius + Target->radius));
		if(qDistance < qRadiusSum)
		{
			DynamicCollision(Actual, Target);
			qDistance = sqrt(qDistance);
			float Ovelap = 0.5 * (qDistance - Actual->radius - Target->radius);
			Actual->x_pos -= Ovelap * (Actual->x_pos - Target->x_pos) / qDistance;
			Actual->y_pos -= Ovelap * (Actual->y_pos - Target->x_pos) / qDistance;
			Target->x_pos += Ovelap * (Actual->x_pos - Target->x_pos) / qDistance;
			Target->y_pos += Ovelap * (Actual->y_pos - Target->y_pos) / qDistance;	
			if(Target->y_pos + Target->radius > ScreenHeight() - WALLS_H)
				Target->y_pos = ScreenHeight() - WALLS_H - Target->radius;		
		}
	}

	int WallTouched(BALL *Actual)
	{
		if(Actual->x_pos + Actual->radius >= ScreenWidth() - WALLS_H)
			return W_RIGHT;
		else if(Actual->x_pos - Actual->radius <= 0 + WALLS_H)
			return W_LEFT;
		else if(Actual->y_pos - Actual->radius <= 0 + WALLS_H)
			return W_TOP;
		else if(Actual->y_pos + Actual->radius >= ScreenHeight() - WALLS_H)
			return W_BOTTOM;	
		else 
			return NO_WALL;	
	}

	bool IsPointInCircle(float px, float py, float r, int mx, int my)
	{
		float fmx = (float)mx;
		float fmy = (float)my;
		float distance = (((px - fmx) * (px - fmx)) + ((py - fmy) * (py - fmy)));
		if(distance <= (r *r))
			return true;
		else 
			return false;
	}

	void GrabBall(BALL *BallSelected)
	{
		if(GetMouse(0).bPressed)
		{
			if(IsPointInCircle(BallSelected->x_pos, BallSelected->y_pos, BallSelected->radius, GetMouseX(), GetMouseY()))
				PickedBall = BallSelected;
		}
		if(GetMouse(0).bHeld)
		{
			if(PickedBall != nullptr)
			{
				PickedBall->x_pos = GetMouseX();
				PickedBall->y_pos = GetMouseY();
				// PickedBall->v_x += 10;
			}
		}
		if(GetMouse(0).bReleased)
		{
			// if(PickedBall != nullptr)
			// {
			// 	if(PickedBall->v_y < 0)
			// 		PickedBall->v_y -= 10;
			// 	else
			// 		PickedBall->v_y += 10;
			// }
			PickedBall = nullptr;
		}	
	}

	void ChangeTemp()
	{
		if(GetKey(UP).bPressed || GetKey(UP).bHeld)
		{
			if(Temperature < MAX_TEMP)
				Temperature++;
			else
				Temperature = MAX_TEMP;
			TempChanged = true;
		}
		if(GetKey(DOWN).bPressed || GetKey(DOWN).bHeld)
		{
			if(Temperature > 0)
				Temperature--;
			else
				Temperature = 0;
			TempChanged = true;
		}
		if(TempChanged)
			cout << "Temp: " << Temperature << endl;
	}

	void Gravity(BALL *Actual, float fElapsedTime)
	{
		float FloorDist = (ScreenHeight() - WALLS_H) - Actual->y_pos;
		float ElasticSlow = Actual->mass * 0.08;
		if(FloorDist <= Actual->radius)
		{

			// cout << "Prima: " << Actual->v_y << endl;
			if(Actual->v_y - ElasticSlow >= 0)
				Actual->v_y = -Actual->v_y + ElasticSlow;
			else
				Actual->v_y = 0;
			// cout << "Dopo: " << Actual->v_y << endl;

		}
		else
			Actual->v_y += ACC_GRAVITY * fElapsedTime;
		Actual->y_mom = Actual->mass * Actual->v_y;
		Actual->y_pos += 0.5 * Actual->v_y * fElapsedTime;
	}

	void BallDynamic(BALL *Actual, vector<BALL> &OtherBalls, float fElapsedTime)
	{
		vector<pair<float, float>> RelativeDistance;
		if(Temperature >= 0)
		{
			// if(Actual->v_x == 0)
			// 	Actual->v_x = Temperature * 2;
			// if(Actual->v_y == 0)
			// 	Actual->v_y = Temperature * 2;
			if(TempChanged)
			{
				if(Actual->v_x < 0)
					Actual->v_x = -Temperature * 2;
				else
					Actual->v_x = Temperature * 2;
				if(Actual->v_y < 0)
					Actual->v_y = -Temperature * 2;
				else
					Actual->v_y = Temperature * 2;
			}
			int WichWall = WallTouched(Actual);

			switch(WichWall)
			{
				case W_TOP:
				case W_BOTTOM:
					Actual->v_y = -Actual->v_y;
					break;
				case W_LEFT:
				case W_RIGHT:
					Actual->v_x = -Actual->v_x;
					break;
				default:
					break;
			}
			for(int ob = 0; ob < OtherBalls.size(); ob++)
			{
				if(Actual->Id == OtherBalls[ob].Id)
					continue;
				RelativeDistance.push_back({OtherBalls[ob].x_pos - Actual->x_pos, OtherBalls[ob].y_pos - Actual->y_pos});
				
			}
			for(int ob = 0; ob < OtherBalls.size(); ob++)
			{
				if(Actual->Id == OtherBalls[ob].Id)
					continue;
				Actual->v_x += (-OtherBalls[ob].mass * fElapsedTime / (RelativeDistance[ob].first * RelativeDistance[ob].first));
				Actual->v_y += (-OtherBalls[ob].mass * fElapsedTime / (RelativeDistance[ob].second * RelativeDistance[ob].second));
				cout << Actual->v_x << endl;
			}

			Actual->x_pos += Actual->v_x * fElapsedTime;
			Actual->y_pos += Actual->v_y * fElapsedTime;
		}
		else
		{
			Actual->v_x = 0;
			Actual->v_y = 0;
		}

	}

	void CheckWallsCompenetration(BALL *Actual)
	{
		float WallDistance[MAX_WALLS - 1];
		WallDistance[W_TOP]    = Actual->y_pos - Actual->radius;
		WallDistance[W_RIGHT]  = ScreenWidth() - Actual->x_pos + Actual->radius;
		WallDistance[W_BOTTOM] = ScreenHeight() - Actual->y_pos + Actual->radius;
		WallDistance[W_LEFT]   = Actual->x_pos - Actual->radius;
		if(WallDistance[W_TOP] < Actual->radius)
			Actual->y_pos = Actual->y_pos + Actual->radius;
		if(WallDistance[W_RIGHT] < Actual->radius)
			Actual->x_pos = Actual->x_pos - Actual->radius;
		if(WallDistance[W_BOTTOM] < Actual->radius)
			Actual->y_pos = Actual->y_pos - Actual->radius;
		if(WallDistance[W_LEFT] < Actual->radius)
			Actual->x_pos = Actual->x_pos + Actual->radius;								
	}

	void CheckRestart()
	{
		if(GetKey(SPACE).bPressed)
		{
			Ids = 1;
			N_BALLS = rand()%RAND_BALL;
			Balls.clear();
			for(int i = 0; i < N_BALLS; i++)
			{
				CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand()%RAND_RAD);
			}
		}
	}
	void AddBall()
	{
		if(GetKey(CTRL).bPressed)
		{
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand()%RAND_RAD);
		}
	}

	void TurnGravity()
	{
		if(GetMouse(1).bPressed)
		{
			if(ACC_GRAVITY == 0)
				ACC_GRAVITY = 98;
			else
				ACC_GRAVITY = 0;
		}
	}
	void AirFriction(BALL *Actual)
	{
		if(Actual->v_y != 0)
		{
			if(Actual->v_y < 0)
				Actual->v_y++;
			else
				Actual->v_y--;
		}
		if(Actual->v_x != 0)
		{
			if(Actual->v_x < 0)
				Actual->v_x++;
			else
				Actual->v_x--;
		}		
	}

	bool OnUserCreate() override
	{

		for(int i = 0; i < N_BALLS; i++)
		{
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand()%RAND_RAD);
		}
		for(int i = 0; i < Balls.size(); i++)
			FillCircle(Balls[i].x_pos , Balls[i].y_pos, Balls[i].radius, Balls[i].color);
		DrawWalls();			
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		
		for(int ball = 0; ball <  Balls.size(); ball++)
		{
			GrabBall(&Balls[ball]);
			BallDynamic(&Balls[ball], Balls, fElapsedTime);
			for(int tar = 0; tar < Balls.size(); tar++)
			{
				if(Balls[tar].Id == Balls[ball].Id)
					continue;
				BallsOverlap(&Balls[ball], &Balls[tar]);
			}		
			// CheckWallsCompenetration(&Balls[ball]);
			FillCircle(Balls[ball].x_pos , Balls[ball].y_pos, Balls[ball].radius, Balls[ball].color);
		}
		if(TempChanged)
			TempChanged = false;
		ChangeTemp();
		DrawWalls();
		AddBall();
		CheckRestart();
		return true;
	}
};
int main()
{
	srand(time(NULL));
	Example FallingBalls;
	if (FallingBalls.Construct(200, 200, 2, 2))
		FallingBalls.Start();
	return 0;
}