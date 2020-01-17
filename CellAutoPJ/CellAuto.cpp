#define OLC_PGE_APPLICATION
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream> 
#include "olcPixelGameEngine.h"

#define N_INIT_CELLS	10
#define N_INIT_FRUITS	 1
#define INIT_RADIUS	 	 4

#define N_COLORS	    23

#define MAIN_TICK 						 0.05f
#define MAIN_TICK_TO_SEC(Sec)            (Sec / MAIN_TICK)
#define CREATE_TICK						 10.0f

using namespace std;
using namespace olc;

struct COLOR_TAG
{
	Pixel CellColor;
	string ColorName;
};

struct CELL
{
	int px;
	int py;
	int radius;
	int vx;
	int vy;
	int weight;
	int age;
	COLOR_TAG color;
};

struct FRUIT
{
	int fx;
	int fy;
	bool fcached;
	Pixel fcolor;
	int fvalNutr;
};



const COLOR_TAG CellColors[N_COLORS]
{
	{WHITE,                        "BIANCO"},
	{GREY, 					       "GRIGIO"},
	{DARK_GREY,      	     "GRIGIO SCURO"},
	{VERY_DARK_GREY,   "GRIGIO MOLTO SCURO"},
	{RED, 					        "ROSSO"},
	{DARK_RED, 			      "ROSSO SCURO"},
	{VERY_DARK_RED,		"ROSSO MOLTO SCURO"},
	{YELLOW, 					   "GIALLO"},
	{DARK_YELLOW,			 "GIALLO SCURO"},
	{VERY_DARK_YELLOW, "GIALLO MOLTO SCURO"},
	{GREEN,						    "VERDE"},
	{DARK_GREEN, 		      "VERDE SCURO"},
	{VERY_DARK_GREEN,   "VERDE MOLTO SCURO"},
	{CYAN, 						    "CYANO"},
    {DARK_CYAN,				  "CIANO SCURO"},
    {VERY_DARK_CYAN,    "CIANO MOLTO SCURO"},
	{BLUE,						      "BLU"},
	{DARK_BLUE,				    "BLU SCURO"},
	{VERY_DARK_BLUE,	  "BLU MOLTO SCURO"},
	{MAGENTA, 					  "MAGENTA"},
	{DARK_MAGENTA, 			"MAGENTA SCURO"},
	{VERY_DARK_MAGENTA,"MAGENTA MOLTO SCURO"},
};

class CellAuto : public olc::PixelGameEngine
{
private:
	vector<CELL>Cells;
	float MainTime = 0.0f, Timer1 = 0.0f, RecreateFruit = 0.0f;
	vector<FRUIT> Fruit;
	bool Stop = false;
public:
	CellAuto()
	{
		sAppName = "CellAuto";
	}
public:
	void WrapCoordinates(int ix, int iy, int *ox, int *oy, int radius)
	{
		*ox = ix;
		*oy = iy;
		if (ix - radius < 0.0f)	*ox = ix + (float)ScreenWidth() - radius;
		if (ix + radius >= (float)ScreenWidth())	*ox = ix - (float)ScreenWidth() + radius;
		if (iy - radius < 0.0f)	*oy = iy + (float)ScreenHeight() - radius;
		if (iy + radius >= (float)ScreenHeight()) *oy = iy - (float)ScreenHeight() + radius;
	}

	float CalcBallsQuadDistance(float x1_pos, float y1_pos, float x2_pos, float y2_pos, bool Sqrt)
	{
		if(Sqrt)
			return sqrtf(((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
		else
			return (((x1_pos - x2_pos) * (x1_pos - x2_pos)) + ((y1_pos - y2_pos) * (y1_pos - y2_pos)));
	}

	bool TakeTick(float fElapsedTime, float Delay, float *Time)
	{
		*Time += fElapsedTime;
		if(*Time >= Delay)
		{
			*Time = 0.0;
			return true;
		}	
		return false;	
	}

	void CreateCell(int posx, int posy, COLOR_TAG Color, int Weight, int rad)
	{
		WrapCoordinates(posx, posy, &posx, &posy, rad);
		Cells.push_back({posx, posy, rad, 1, 1, Weight, 1, Color.CellColor, Color.ColorName});
	}

	void CreateFruit(int HowMany)
	{
		for(int i = 0; i < HowMany; i++)
		{
			FRUIT f;
			f.fx = rand()%ScreenWidth();
			f.fy = rand()%ScreenHeight();
			f.fcached = false;
			f.fvalNutr = rand()%30;
			f.fcolor = Pixel(rand()%255, rand()%255, rand()%255);
			Fruit.push_back(f);		
		}
	}

	void CheckCollision()
	{
		for(auto &c : Cells)
		{
			for(auto &f : Fruit)
			{
				if(!f.fcached)
				{
					float Dist = CalcBallsQuadDistance(c.px, c.py, f.fx, f.fy, true);
					if(Dist <= 100 && Dist > c.radius)
					{
						if(c.px - f.fx < 0)
							c.px++;
						else if(c.px - f.fx > 0)
							c.px--;

						if(c.py - f.fy < 0)
							c.py++;
						else if(c.py - f.fy > 0)
							c.py--;
						if(c.radius <= INIT_RADIUS)
							c.age++;
						else if(c.radius > INIT_RADIUS)
							c.age += 5;
					}
					else if(Dist > 100)
					{

					}
					else
					{
						f.fcached = true;
						c.weight += f.fvalNutr;
						// c.radius++;
						c.age -= 5;
					}
				}
			}
		}
	}

	void Restart()
	{
		if(Cells.size() == 0)
		{
			for(int i = 0; i < N_INIT_CELLS; i++)
			{
				CreateCell(rand() % ScreenWidth() - 1, rand() % ScreenHeight() - 1, CellColors[rand()%N_COLORS], 1, INIT_RADIUS);
			}
			for(auto c : Cells)
				FillCircle(c.px, c.py, c.radius, c.color.CellColor);
			Fruit.clear();
			CreateFruit(N_INIT_FRUITS);			
		}
	}

	bool OnUserCreate() override
	{
		for(int i = 0; i < N_INIT_CELLS; i++)
		{
			CreateCell(rand() % ScreenWidth() - 1, rand() % ScreenHeight() - 1, CellColors[rand()%N_COLORS], 1, INIT_RADIUS);
		}
		for(auto c : Cells)
			FillCircle(c.px, c.py, c.radius, c.color.CellColor);
		CreateFruit(N_INIT_FRUITS);
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{	
		if(GetKey(SPACE).bPressed)
		{
			Stop = !Stop;
			cout << endl;
			if(Stop)
				cout << "STOP" << endl;
			else
				cout << "GO" << endl;
		}
		if(!Stop)
		{
			if(TakeTick(fElapsedTime, MAIN_TICK, &MainTime))
			{		
				Clear(BLACK);
				for(int i = 0; i < Cells.size(); i++)
				{
					for(int j = 0; j < Cells.size(); j++)
					{
						if (i == j)
							continue;
						float DistCell = CalcBallsQuadDistance(Cells[i].px, Cells[i].py, Cells[j].px, Cells[j].py, false);
						if((DistCell <= (Cells[i].radius + Cells[j].radius) * (Cells[i].radius + Cells[j].radius)) &&
							Cells[i].color.CellColor != Cells[j].color.CellColor)
						{
							Cells[j].px += Cells[i].radius * 1;
							Cells[j].py += Cells[i].radius * 1;
							Cells[i].px -= Cells[j].radius * 1;
							Cells[i].py -= Cells[j].radius * 1;
							WrapCoordinates(Cells[j].px, Cells[j].py, &Cells[j].px, &Cells[j].py, Cells[j].radius);
						}
						else if((DistCell <= (Cells[i].radius + Cells[j].radius) * (Cells[i].radius + Cells[j].radius)) && 
							Cells[i].color.CellColor == Cells[j].color.CellColor)
						{
							cout << endl;
							cout << "Cella ingrandita" << endl;
							cout << "Colore: " << Cells[j].color.ColorName << endl;
							cout << "Cella attuale rad: " << Cells[i].radius <<  endl;
							cout << "Cella target rad: " << Cells[j].radius << endl;
							int rad = Cells[i].radius;
							if(rad < Cells[j].radius)
								rad = Cells[j].radius;
							int posx = Cells[j].px + Cells[j].radius;
							int posy = Cells[j].py + Cells[j].radius;
							COLOR_TAG col = Cells[j].color;
							int w = Cells[j].weight;
							Cells.erase(Cells.begin() + i);
							Cells.erase(Cells.begin() + j);							
							CreateCell(posx, posy, col, w, rad * 2);
							cout << "Cella creata rad: " << Cells[Cells.size() - 1].radius << endl;
						}
					}
					
				}

				CheckCollision();

				for(auto c : Cells)
				{
					WrapCoordinates(c.px , c.py , &c.px, &c.py, c.radius);
					FillCircle(c.px, c.py, c.radius, c.color.CellColor);
				}


				for(auto f : Fruit)
				{
					if(!f.fcached)
					{
						DrawCircle(f.fx, f.fy, 1, f.fcolor);
					}
					else
					{
						CreateFruit(1);
					}
				}
			}
			for(int i = 0; i < Fruit.size(); i++)
			{
				if(Fruit[i].fcached)
					Fruit.erase(Fruit.begin() + i);
				else
				{
					if(TakeTick(fElapsedTime, 5.0, &RecreateFruit))
					{
						Fruit.erase(Fruit.begin() + i);
						CreateFruit(1);
						// break;
					}
				}
			}
			for(int i = 0; i < Cells.size(); i++)
			{
				if(Cells[i].age > MAIN_TICK_TO_SEC(30))
					Cells.erase(Cells.begin() + i);
				if(Cells[i].weight >= Cells[i].radius * 2)
				{
					// cout << "Cella duplicata" << endl;
					CreateCell(Cells[i].px + 4, Cells[i].py + 4, Cells[i].color, Cells[i].weight / 4, Cells[i].radius);
					CreateCell(Cells[i].px - 4, Cells[i].py - 4, Cells[i].color, Cells[i].weight / 4, Cells[i].radius);
					Cells.erase(Cells.begin() + i);
				}
			}	
			if(TakeTick(fElapsedTime, CREATE_TICK, &Timer1))
			{
				CreateCell(rand() % ScreenWidth() - 1, rand() % ScreenHeight() - 1, CellColors[rand()%N_COLORS], 1, INIT_RADIUS);
			}
			Restart();
		}
		return true;
	}
};
int main()
{
	srand(clock());
	CellAuto Cell;
	if (Cell.Construct(300, 300, 2, 2))
		Cell.Start();
	return 0;
}
