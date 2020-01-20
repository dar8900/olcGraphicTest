#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <sstream> 
#include <ctime>
// Override base class with your custom functionality

using namespace std;
using namespace olc;

#define MAX_OBJ_W	30
#define MAX_OBJ_H	20

#define MAX_VELOCITY  20
#define GRAVITY	      25

typedef enum 
{
	RECTANGULAR = 0,
	CIRCLE,
	MAX_OBJ_TYPE
}GAME_OBJ_TYPE;

typedef struct 
{
	float c_pos_x;
	float c_pos_y;
	float c_v_x;
	float c_v_y;
	float c_w;
	float c_h;
}CHARACTER;


typedef struct 
{
	float obj_x;
	float obj_y;
	float obj_w;
	float obj_h;
	float obj_center_x;
	float obj_center_y;
	uint8_t objType;
}GAME_OBJ;


vector<GAME_OBJ> GameObjs;

CHARACTER EtGuy;

class SIMOLED : public olc::PixelGameEngine
{
public:
	SIMOLED()
	{
		sAppName = "SimOledDisplay";
	}
private:
	template<class value> void DrawValueDisplay(int32_t x, int32_t y, value Val)
	{
		stringstream ss;
		ss << Val;
		DrawString(x, y, ss.str());
	}
public:
	bool OverObject = false;

	void DrawCharacter()
	{
		Draw(EtGuy.c_pos_x, EtGuy.c_pos_y, WHITE);     // blocco centrale
		Draw(EtGuy.c_pos_x + 1, EtGuy.c_pos_y, WHITE); // braccio dx 1
		Draw(EtGuy.c_pos_x + 2, EtGuy.c_pos_y, WHITE); // braccio dx 2
		Draw(EtGuy.c_pos_x - 1, EtGuy.c_pos_y, WHITE); // braccio sx 1
		Draw(EtGuy.c_pos_x - 2, EtGuy.c_pos_y, WHITE); // braccio sx 2
		Draw(EtGuy.c_pos_x, EtGuy.c_pos_y - 1, WHITE); // testa 1
		Draw(EtGuy.c_pos_x, EtGuy.c_pos_y - 2, WHITE); // testa 2		
		// testa 3 direzione ROSSA
		if(EtGuy.c_v_x > 0)
			Draw(EtGuy.c_pos_x + 1, EtGuy.c_pos_y - 2, RED); 
		else if(EtGuy.c_v_x < 0)
			Draw(EtGuy.c_pos_x - 1, EtGuy.c_pos_y - 2, RED); 
		else
			Draw(EtGuy.c_pos_x, EtGuy.c_pos_y - 3, RED); 
		Draw(EtGuy.c_pos_x, EtGuy.c_pos_y, WHITE);     // blocco gambe 1
		Draw(EtGuy.c_pos_x, EtGuy.c_pos_y + 1, WHITE);     // blocco gambe 1
		Draw(EtGuy.c_pos_x - 1, EtGuy.c_pos_y + 1, WHITE); // blocco gamba sx 1
		Draw(EtGuy.c_pos_x - 1, EtGuy.c_pos_y + 2, WHITE); // blocco gamba sx 2
		Draw(EtGuy.c_pos_x + 1, EtGuy.c_pos_y + 1, WHITE); // blocco gamba dx 1
		Draw(EtGuy.c_pos_x + 1, EtGuy.c_pos_y + 2, WHITE); // blocco gamba dx 2		
	}

	void CreateGameObj()
	{
		GAME_OBJ Obj;
		Obj.objType = CIRCLE;//(uint8_t)(rand()%MAX_OBJ_TYPE);
		if(Obj.objType == RECTANGULAR)
		{	
			Obj.obj_x = (float)(rand()%ScreenWidth());
			Obj.obj_y = (float)(rand()%(ScreenHeight() - (int)EtGuy.c_h - 2));
			Obj.obj_w = (float)(rand()%MAX_OBJ_W);
			Obj.obj_h = (float)(rand()%MAX_OBJ_H);
			if(Obj.obj_w < EtGuy.c_w)
				Obj.obj_w = EtGuy.c_w + 1;

			if(Obj.obj_x < EtGuy.c_pos_x + EtGuy.c_w)
				Obj.obj_x = EtGuy.c_pos_x + EtGuy.c_w + 1;
			// if(Obj.obj_x + Obj.obj_w < EtGuy.c_pos_x - EtGuy.c_w)
			// 	Obj.obj_x = EtGuy.c_pos_x - EtGuy.c_w - 1 - Obj.obj_w;		
			if(Obj.obj_x + Obj.obj_w > ScreenWidth())
				Obj.obj_w = ScreenWidth() - (Obj.obj_x + Obj.obj_w);
			Obj.obj_h = (ScreenHeight() - 1) - (Obj.obj_y);
			Obj.obj_center_x = (Obj.obj_x + (Obj.obj_w / 2));
			Obj.obj_center_y = (Obj.obj_y + (Obj.obj_h / 2));			

		}
		else
		{
			Obj.obj_x = 66;//(float)(rand()%ScreenWidth());
			Obj.obj_w = (float)(rand()%(MAX_OBJ_W - 10));
			Obj.obj_h = Obj.obj_w;
			Obj.obj_y = ScreenHeight() - (Obj.obj_w / 2) - 2;
			Obj.obj_center_x = Obj.obj_x;
			Obj.obj_center_y = Obj.obj_y;			
		}
		GameObjs.push_back(Obj);
	}

	void DrawGameObj()
	{
		for(int i = 0; i < GameObjs.size(); i++)
		{
			if(GameObjs[i].objType == RECTANGULAR)
			{
				FillRect((int32_t) GameObjs[i].obj_x, (int32_t)(GameObjs[i].obj_y), 
					(int32_t)GameObjs[i].obj_w, (int32_t)GameObjs[i].obj_h);
				Draw((int )GameObjs[i].obj_center_x, (int)GameObjs[i].obj_center_y, RED);
			}
			else
			{
				FillCircle((int32_t)GameObjs[i].obj_center_x, (int32_t)GameObjs[i].obj_center_y,
				 (int32_t)GameObjs[i].obj_w);
				Draw((int )GameObjs[i].obj_center_x, (int)GameObjs[i].obj_center_y, RED);
			}
		}
	}

	void UpdateCharacterPos(float fElapsedTime)
	{
		if(GetKey(RIGHT).bPressed)
		{
			if(EtGuy.c_v_x < 0)
				EtGuy.c_v_x = 0;
			else
				EtGuy.c_v_x += MAX_VELOCITY;
			if(EtGuy.c_v_x > MAX_VELOCITY)
				EtGuy.c_v_x = MAX_VELOCITY;
		}
		if(GetKey(LEFT).bPressed)
		{
			if(EtGuy.c_v_x > 0)
				EtGuy.c_v_x = 0;
			else
				EtGuy.c_v_x -= MAX_VELOCITY;
			if(EtGuy.c_v_x < -MAX_VELOCITY)
				EtGuy.c_v_x = -MAX_VELOCITY;			
		}
		if(GetKey(SPACE).bPressed)
		{
			EtGuy.c_v_y = -20;
		}

		EtGuy.c_pos_x += (EtGuy.c_v_x * fElapsedTime);
		if(EtGuy.c_pos_x > ScreenWidth() - 1)
			EtGuy.c_pos_x = 1;
		if(EtGuy.c_pos_x < 1)
			EtGuy.c_pos_x = ScreenWidth() - 1;

		if((EtGuy.c_v_y != 0 || EtGuy.c_pos_y < ScreenHeight() - 3) && !OverObject )
		{
			EtGuy.c_v_y += GRAVITY * fElapsedTime; 
			EtGuy.c_pos_y += (EtGuy.c_v_y * fElapsedTime);
			if(EtGuy.c_pos_y > ScreenHeight() - 3)
			{
				EtGuy.c_pos_y = ScreenHeight() - 3;
				EtGuy.c_v_y = 0;
			}
			if(EtGuy.c_pos_y - EtGuy.c_h < 0)
				EtGuy.c_v_y = 0;
		}
	}

	float CalcDistance(GAME_OBJ Obj, uint8_t Type)
	{
		float Distance = 0.0f;
		Distance = sqrt(((EtGuy.c_pos_x - Obj.obj_center_x) * (EtGuy.c_pos_x - Obj.obj_center_x)) + 
			((EtGuy.c_pos_y - Obj.obj_center_y) * (EtGuy.c_pos_y - Obj.obj_center_y)));
		return Distance;
	}

	void ObjectCollision()
	{
		int distX1 = 0, distX2 = 0, distY1 = 0;
		for(int i = 0; i < GameObjs.size(); i++)
		{
			if(GameObjs[i].objType == RECTANGULAR)
			{
				distX1 = (int)(GameObjs[i].obj_center_x - (GameObjs[i].obj_w / 2) - (EtGuy.c_pos_x + EtGuy.c_w));
				distX2 = -(int)(GameObjs[i].obj_center_x + (GameObjs[i].obj_w / 2) - (EtGuy.c_pos_x - EtGuy.c_w - 1));
				distY1 = (int)(GameObjs[i].obj_center_y - (GameObjs[i].obj_h / 2)) - (EtGuy.c_pos_y + EtGuy.c_h - 2);
			}
			else
			{
				distX1 = (int)CalcDistance(GameObjs[i], GameObjs[i].objType);
			}
			if((distX1 < 0 && distX2 < 0) && GameObjs[i].objType == RECTANGULAR)
			{
				if(EtGuy.c_v_y == 0)
					EtGuy.c_v_x = 0;
				if(distY1 < 0)
					EtGuy.c_v_x = 0;
				else if(distY1 == 0 && (int)EtGuy.c_v_y < 0)
				{
					EtGuy.c_v_x = 0;
				}
				else if(distY1 > 0 && (int)EtGuy.c_v_y < 0 )
				{
					EtGuy.c_v_x  = MAX_VELOCITY;
				}
				else if(distY1 == 0 && (int)EtGuy.c_v_y > 0)
				{
					// EtGuy.c_v_y = 0;
					OverObject = true;
				}	
			}
			else if(distX1 < (EtGuy.c_w + GameObjs[i].obj_w) ||
				(distX1 < (EtGuy.c_h + GameObjs[i].obj_w)))
			{
				if((int)EtGuy.c_v_y == 0)
					EtGuy.c_v_x = 0;
				else
					OverObject = true;
			}
			else
				OverObject = false;
			DrawValueDisplay(0, 0, distX1);
			DrawValueDisplay(0, 10, EtGuy.c_w + GameObjs[i].obj_w);
			DrawValueDisplay(0, 20, OverObject);

		}
	}

	void CheckReset(bool Restart)
	{
		if(GetKey(N).bPressed || Restart)
		{
			GameObjs.clear();
			EtGuy.c_pos_y = ScreenHeight() - 3;
			EtGuy.c_pos_x = ScreenWidth() / 8;	
			EtGuy.c_v_x = 0;
			EtGuy.c_v_y = 0;
			EtGuy.c_w = 3;
			EtGuy.c_h = 4;	
			CreateGameObj();	
		}
	}

	bool OnUserCreate() override
	{
		CheckReset(true);
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(BLACK);
		DrawCharacter();
		UpdateCharacterPos(fElapsedTime);
		DrawGameObj();
		ObjectCollision();
		CheckReset(false);
		return true;
	}
};
int main()
{
	SIMOLED Character;
	srand(time(NULL));
	if (Character.Construct(128, 64, 2, 2))
		Character.Start();
	return 0;
}