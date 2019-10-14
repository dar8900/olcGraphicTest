#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>

#define WALLS_H		     3
#define N_BALLS		 	 2//(rand()%10)
#define RADIUS           (rand()%30)
// #define ACC_GRAVITY		 98
#define MAX_VELOCITY	 50
#define FLOOR_MASS		 10000000
#define MAX_TEMP 	     1000
#define BALLS_DENSITY	 1

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
vector<pair<float, float>> RelativeVelocity;
bool EnableGravity = false;
bool BallGrabbed = false;


class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Gravity Balls";
	}
public:
	
	BALL *PickedBall = nullptr;
	int Ids = 1;
	float BigRad;

	void CreateBall(float Px, float Py, float Rad)
	{
		BALL NewBall;
		NewBall.radius = Rad;
		NewBall.mass = (4/3 * 3.14 * pow(NewBall.radius, 3) * BALLS_DENSITY);
		NewBall.x_pos = Px;
		NewBall.y_pos = Py;
		if(((NewBall.x_pos + NewBall.radius) >= ScreenWidth()) || (NewBall.x_pos - NewBall.radius) <=  0)
			NewBall.x_pos = ScreenWidth() / 2;
		if(((NewBall.y_pos + NewBall.radius) >= ScreenHeight() - WALLS_H) || (NewBall.y_pos - NewBall.radius) <=  0)
			NewBall.y_pos = ScreenHeight() / 2;			
		NewBall.color = Pixel(255, rand() % 255, rand() % 255);
		NewBall.v_x = 5;
		NewBall.v_y = 5; 
		NewBall.Id = Ids++;
		// cout << NewBall.Id << endl;
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
			return sqrtf(((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
		else
			return (((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
	}

	void DynamicCollision(BALL *Actual, BALL *Target)
	{
		BALL  *b1 = Actual;
		BALL  *b2 = Target;

		// Distance between balls
		float fDistance = CalcBallsQuadDistance(b1->x_pos, b1->y_pos, b2->x_pos, b2->y_pos, true);

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
			// DynamicCollision(Actual, Target);
			qDistance = sqrt(qDistance);
			float Ovelap = 0.5 * (qDistance - Actual->radius - Target->radius);
			Actual->x_pos -= Ovelap * (Actual->x_pos - Target->x_pos) / qDistance;
			Actual->y_pos -= Ovelap * (Actual->y_pos - Target->x_pos) / qDistance;
			Target->x_pos += Ovelap * (Actual->x_pos - Target->x_pos) / qDistance;
			Target->y_pos += Ovelap * (Actual->y_pos - Target->y_pos) / qDistance;	
			if(Target->x_pos - Target->radius <= 0) 
				Target->x_pos = Target->x_pos + Target->radius;
			else if(Target->x_pos + Target->radius >= ScreenWidth() - WALLS_H) 
				Target->x_pos = Target->x_pos - Target->radius;
			if(Target->y_pos - Target->radius <= 0) 
				Target->y_pos = Target->y_pos + Target->radius;
			else if(Target->y_pos + Target->radius >= ScreenHeight() - WALLS_H) 
				Target->y_pos = Target->y_pos - Target->radius;			
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
			BallGrabbed = true;
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
			PickedBall = nullptr;
			BallGrabbed = false;
		}	
	}

	void BallsGravity(BALL *Actual, BALL *OtherBall, float fElapsedTime)
	{
		float dist = CalcBallsQuadDistance(Actual->x_pos, Actual->y_pos, OtherBall->x_pos, OtherBall->y_pos, true);
		float norm_x = (Actual->x_pos - OtherBall->x_pos) / dist;
		float norm_y = (Actual->y_pos - OtherBall->y_pos) / dist;

		if(norm_x != 0 || Actual->v_x < MAX_VELOCITY || dist + BigRad > 0)
		{
				Actual->v_x += (OtherBall->mass / (dist * dist)) * fElapsedTime;
		}
		else if(Actual->v_x >= MAX_VELOCITY)
		{
				Actual->v_x = MAX_VELOCITY;
		}
		// else
		// 	Actual->v_x = 0;

		if(norm_y != 0 || Actual->v_y < MAX_VELOCITY || dist + BigRad > 0)
		{
				Actual->v_y += (OtherBall->mass / (dist * dist)) * fElapsedTime;
		}
		else if( Actual->v_y >= MAX_VELOCITY)
		{
				Actual->v_y = MAX_VELOCITY;
		}
		// else
		// 	Actual->v_y = 0;

		// cout << endl;
		// cout << "vx: " << Actual->v_x << endl;
		// cout << "vy: " << Actual->v_y << endl;

		RelativeVelocity.push_back({Actual->v_x, Actual->v_y});
	}

	void NewGravityPos(BALL *Actual, BALL *OtherBall, float fElapsedTime)
	{
		float dist = CalcBallsQuadDistance(Actual->x_pos, Actual->y_pos, OtherBall->x_pos, OtherBall->y_pos, true);
		float norm_x = (Actual->x_pos - OtherBall->x_pos) / dist;
		float norm_y = (Actual->y_pos - OtherBall->y_pos) / dist;
		float sumX = 0, sumY = 0;
		for(int i = 0; i < RelativeVelocity.size(); i++)
		{		
			if(i == Actual->Id)
				continue;
			sumX += RelativeVelocity[i].first;
			sumY += RelativeVelocity[i].second;
		}
		if(dist > BigRad)
		{
			if(norm_x > 0)
				Actual->x_pos -= sumX * fElapsedTime;
			else
				Actual->x_pos += sumX * fElapsedTime;
			if(norm_y > 0)
				Actual->y_pos -= sumY * fElapsedTime;
			else
				Actual->y_pos += sumY * fElapsedTime;
		}
		else 
		{
			Actual->v_x = 0;
			Actual->v_y = 0;
		}

		dist = fabs(dist);
		cout << endl;
		cout << "Id: " << Actual->Id << endl;
		cout << "Dist: " << dist << endl;
		// cout << "X: " << norm_x << endl;
		// cout << "Y: " << norm_y << endl;
	
	}

	void BallDynamic(BALL *Actual, float fElapsedTime)
	{
		// Wall touched
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
		// if(WichWall != NO_WALL)
		// {
		// 	// Aggiorno le posizioni
		// 	Actual->x_pos += Actual->v_x * fElapsedTime;
		// 	Actual->y_pos += Actual->v_y * fElapsedTime;
		// }
		if(Actual->x_pos - Actual->radius <= 0) 
			Actual->x_pos = Actual->x_pos + Actual->radius;
		else if(Actual->x_pos + Actual->radius >= ScreenWidth() - WALLS_H) 
			Actual->x_pos = Actual->x_pos - Actual->radius;
		if(Actual->y_pos - Actual->radius <= 0) 
			Actual->y_pos = Actual->y_pos + Actual->radius;
		else if(Actual->y_pos + Actual->radius >= ScreenHeight() - WALLS_H) 
			Actual->y_pos = Actual->y_pos - Actual->radius;		
	}



	void CheckRestart()
	{
		if(GetKey(SPACE).bPressed)
		{
			Ids = 1;
			EnableGravity = false;
			Balls.clear();
			RelativeVelocity.clear();
			for(int i = 0; i < N_BALLS; i++)
			{
				CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), RADIUS);
				Balls[i].v_x = 0;
				Balls[i].v_y = 0;
			}
		}
	}
	void AddBall()
	{
		if(GetKey(CTRL).bPressed)
		{
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), RADIUS);
			for(int i = 0; i < Balls.size(); i++)
			{
				Balls[i].v_x = 0;
				Balls[i].v_y = 0;
			}
			RelativeVelocity.clear();
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
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), RADIUS);
		}
		for(int i = 0; i < Balls.size(); i++)
			FillCircle(Balls[i].x_pos , Balls[i].y_pos, Balls[i].radius, Balls[i].color);
		DrawWalls();			
		return true;
	}

	

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		if(GetMouse(1).bPressed)
			EnableGravity = !EnableGravity;
		for(int ball = 0; ball <  Balls.size(); ball++)
		{
			GrabBall(&Balls[ball]);
			BigRad = (Balls[ball].radius > BigRad)? Balls[ball].radius : BigRad;
			for(int tar = 0; tar < Balls.size(); tar++)
			{
				if(Balls[tar].Id == Balls[ball].Id)
					continue;
				if(EnableGravity)
					BallsGravity(&Balls[ball], &Balls[tar], fElapsedTime);
				BallsOverlap(&Balls[ball], &Balls[tar]);
			}	
			for(int newB = 0; newB < Balls.size(); newB++)
			{
				if(EnableGravity)
					NewGravityPos(&Balls[ball], &Balls[newB], fElapsedTime);
			}
			// BallDynamic(&Balls[ball], fElapsedTime);	
			if(BallGrabbed)
			{
				Balls[ball].v_x = 0;
				Balls[ball].v_y = 0;
				RelativeVelocity.clear();
			}

			// CheckWallsCompenetration(&Balls[ball]);
			FillCircle(Balls[ball].x_pos , Balls[ball].y_pos, Balls[ball].radius, Balls[ball].color);
		}
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
	if (FallingBalls.Construct(400, 400, 2, 2))
		FallingBalls.Start();
	return 0;
}