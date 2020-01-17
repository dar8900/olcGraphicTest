#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <ctime>

#define FLOOR_H		     5
// #define N_BALLS		 	 5
// #define ACC_GRAVITY		 98
#define MAX_VELOCITY	 1000
#define FLOOR_MASS		 10000000

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
	Pixel color;
}BALL;



vector<BALL> Balls;

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "FallingBalls";
	}
public:
	
	BALL *PickedBall = nullptr;
	int N_BALLS = rand() % 30;
	float ACC_GRAVITY = 98;

	void CreateBall(float Px, float Py, float Rad)
	{
		BALL NewBall;
		NewBall.radius = Rad;
		NewBall.mass = (NewBall.radius * 10);
		NewBall.x_pos = Px;
		NewBall.y_pos = Py;
		if(((NewBall.x_pos + NewBall.radius) >= ScreenWidth()) || (NewBall.x_pos - NewBall.radius) <=  0)
			NewBall.x_pos = ScreenWidth() / 2;
		if(((NewBall.y_pos + NewBall.radius) >= ScreenHeight() - FLOOR_H) || (NewBall.y_pos - NewBall.radius) <=  0)
			NewBall.y_pos = ScreenHeight() / 2;			
		NewBall.color = Pixel(255, rand() % 255, rand() % 255);
		NewBall.v_x = 0;
		NewBall.v_y = 0;
		Balls.push_back(NewBall);		
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
			if(Target->y_pos + Target->radius > ScreenHeight() - FLOOR_H)
				Target->y_pos = ScreenHeight() - FLOOR_H - Target->radius;		
		}
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
				PickedBall->v_y = 0;
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

	void Gravity(BALL *Actual, float fElapsedTime)
	{
		float FloorDist = (ScreenHeight() - FLOOR_H) - Actual->y_pos;
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

	void CheckFloorCompenetration(BALL *Actual)
	{
		float FloorDist = (ScreenHeight() - FLOOR_H) - Actual->y_pos;
		if(Actual->v_y == 0)
		{
			if(FloorDist <= Actual->radius)
				Actual->y_pos = ScreenHeight() - FLOOR_H - Actual->radius;
		}
	}

	void CheckRestart()
	{
		if(GetKey(SPACE).bPressed)
		{
			N_BALLS = rand() % 30;
			Balls.clear();
			for(int i = 0; i < N_BALLS; i++)
			{
				CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand() % 10);
			}
		}
	}
	void AddBall()
	{
		if(GetKey(CTRL).bPressed)
		{
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand() % 10);
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
			CreateBall(rand()%ScreenWidth(), rand()%ScreenHeight(), rand() % 10);
		}
		for(int i = 0; i < Balls.size(); i++)
			FillCircle(Balls[i].x_pos , Balls[i].y_pos, Balls[i].radius, Balls[i].color);			
		FillRect(0, ScreenHeight() - 5, ScreenWidth(), 5, WHITE);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		
		for(int ball = 0; ball <  Balls.size(); ball++)
		{
			Gravity(&Balls[ball], fElapsedTime);
			GrabBall(&Balls[ball]);
			for(int tar = 0; tar < Balls.size(); tar++)
			{
				if(tar == ball)
					continue;
				BallsOverlap(&Balls[ball], &Balls[tar]);
			}
			CheckFloorCompenetration(&Balls[ball]);
			AirFriction(&Balls[ball]);
			// TouchFloor(&Balls[ball]);
			FillCircle(Balls[ball].x_pos , Balls[ball].y_pos, Balls[ball].radius, Balls[ball].color);
		}
		FillRect(0, ScreenHeight() - 5, ScreenWidth(), 5, WHITE);
		TurnGravity();
		AddBall();
		CheckRestart();
		return true;
	}
};
int main()
{
	srand(time(NULL));
	Example FallingBalls;
	if (FallingBalls.Construct(200, 200, 4, 4))
		FallingBalls.Start();
	return 0;
}