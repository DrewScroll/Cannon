// EjercicioCañon.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include <math.h>
#include <vector>
#include <iostream>

#ifndef Rad2Deg
#define Rad2Deg( r ) ((r) * (180.0f / M_PI))
#endif

#ifndef Deg2Rad
#define Deg2Rad( d ) ((d) * (M_PI / 180.0f))
#endif

#define M_PI 3.1416

using namespace std;

struct Vector3
{
	struct
	{
		float x, y, z, w;
	};
	Vector3() :x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
	Vector3(const float &xp, const float &yp, const float &zp) :x(xp), y(yp), z(zp), w(1.0f) {  }
	float Length();
	Vector3 Normalize();

	Vector3& operator += (const Vector3&);
	Vector3& operator -= (const Vector3&);
	Vector3& operator *= (float);
	Vector3& operator /= (float f);

	Vector3 operator + (const Vector3&) const;
	Vector3 operator - (const Vector3&) const;
	Vector3 operator * (float) const;
	Vector3 operator / (float) const;
	
	Vector3	   operator * (const Vector3&);

	Vector3 Cross3(Vector3&V);

	friend Vector3 operator * (float, const struct Vector3&);

	bool operator == (const Vector3&) const;
	bool operator != (const Vector3&) const;
};

struct Objective
{
	Objective() :Padre(nullptr), Madre(nullptr) {}
	Vector3 Pos;
	Objective* Padre;
	Objective* Madre;
	bool isDestroyed = false;
};

//Vf = V1 + (A*T) (Vf = Velocidad Final, V1 = Velocidad inicial, A = Acceleration, T = Tiempo)
//Peso es fuerza porque m*a (m = masa, a = acceleracion)
//plano = aX +bY +cZ + d
//rayo contra rayo, contra plano, caja, triangulo, y esfera
//plano contra plano, plano contra caja, plano contra caja, plano contra esfera
//caja contra caja, caja contra esfera
//esfera contra esfera

float Time( float velocity, float grav, float ang)
{
	float totalTime;
	totalTime = ((2 * velocity*sin(Deg2Rad(ang))) / grav);
	return totalTime;
}

Vector3 NewVelocity(float velocity, float angVert, float angHori)
{
	Vector3 Res;
	
	Res.x = cos(Deg2Rad(angHori))*cos(Deg2Rad(angVert));
	Res.y = sin(Deg2Rad(angVert));
	Res.z = sin(Deg2Rad(angHori))*cos(Deg2Rad(angVert));
	Res = Res * velocity;
	return Res;
}

void AssignPos(vector <Objective>& Targets)
{
	Targets[0].Pos = { 204,0,0 };
	Targets[1].Pos = { 209,0,0 };

	Objective* p1;
	Objective* p2;

	float Mag = sqrt(25 + (2.5*2.5));
	float j = 2;
	bool bMother = false;
	bool bPassed = false;

	p1 = &Targets[0];
	p2 = &Targets[1];

	for (int i = 2; j < 20; ++i)
	{
		do
		{
			if (i != 2)
			{
				if (!bPassed)
				{
					p1 = Targets[i - 1].Padre;
					p2 = &Targets[i - 1];
					bPassed = true;
				}
				else
				{
					p1 = &Targets[i - 1];
					p2 = Targets[i - 1].Madre;
					bMother = true;
				}
			}

			Vector3 Dir = p2->Pos - p1->Pos;
			Vector3 Up(0, 1, 0);
			Vector3 Perpen = Dir.Cross3(Up).Normalize();
			Vector3 PuntoMedio = ((Dir *0.5f) + p1->Pos);

			Targets[j].Pos = PuntoMedio + (Perpen*Mag);
			Targets[j].Padre = p1;
			Targets[j].Madre = p2;
			++j;

			if (i == 2)
			{
				Targets[j].Pos = PuntoMedio - (Perpen*Mag);
				Targets[j].Padre = p1;
				Targets[j].Madre = p2;
				++j;
			}
		} while (!bMother && i > 2);
		bMother = false;
	}
}

float CheckNegative(float fi, float ff)
{
	float r = ff - fi;
	if (r < 0)
	{
		r = r * -1;
	}
	return r;
}

int main()
{
	float iWBall = 20;
	float iVBall = 45;
	float iSec = 0;
	Vector3 PositionI(0, 0, 0);
	Vector3 PositionF(0, 0, 0);
	Vector3 Velocity(0, 0, 0);
	Vector3 Wind(0, 0, -9);
	Vector3 Gravity(0, -9.8, 0);
	int iRadius = 3;
	vector <Objective> Targets;
	Targets.resize(20);
	int destroyed = 0;
	float vd = 0, hd = 0, vdf = 0, hdf = 0;

	AssignPos(Targets);
	 
	while (destroyed < 20)
	{
		cout << "Current Targets on field:" << endl;
		for (int i = 0; i < 20; ++i)
		{
			if (Targets[i].isDestroyed == false)
			{
				cout << Targets[i].Pos.x << ", " << Targets[i].Pos.y << ", " << Targets[i].Pos.z << endl;
			}
		}
		cout << "Assign vertical degree" << endl;
		cin >> vdf;
		cout << "Assign horizontal degree" << endl;
		cin >> hdf;
		iSec = (CheckNegative(vd, vdf)) + (CheckNegative(hd, hdf));
		vd = vdf;
		hd = hdf;
		Velocity = NewVelocity(iVBall, vd, hd);
		do
		{
			PositionI = PositionF;
			PositionF = PositionF + (Velocity + (Wind / 20));
			Velocity = Velocity + Gravity;

		} while (PositionF.y > 0);

		Vector3 H = PositionF - PositionI;
		float ang = atan(H.y / sqrt((H.x*H.x) + (H.z*H.z)));
		float h = PositionI.y / sin(ang);
		float mag = h / H.Length();
		H = H * mag;
		Vector3 Respuesta = PositionI - H;

		cout << "Your ball landed at:" << endl;
		cout << Respuesta.x << ", " << Respuesta.y << ", " << Respuesta.z << endl;

		for (int i = 0; i < 20; ++i)
		{
			Vector3 Dir = Targets[i].Pos - Respuesta;
			if (Dir.Length() < iRadius && Targets[i].isDestroyed == false)
			{
				Targets[i].isDestroyed = true;
				++destroyed;

				cout << "Target # " << destroyed << " destroyed" << endl;
			}
		}
		iSec += Time(iVBall, Gravity.Length(), vdf);
	}
	cout << "Time taken: " << iSec << endl;
	cout << "Thank you for participating." << endl;
    return 0;
}

float Vector3::Length()
{
	return sqrt((this->x*this->x) + (this->y*this->y) + (this->z*this->z));
}

Vector3 Vector3::Normalize()
{
	Vector3 Result;
	float res = this->Length();
	Result.x = this->x / res;
	Result.y = this->y / res;
	Result.z = this->z / res;
	return Result;
}

Vector3 & Vector3::operator+=(const Vector3 &v)
{
	Vector3 Res;
	Res.x = this->x + v.x;
	Res.y = this->y + v.y;
	Res.z = this->z + v.z;
	return Res;
}

Vector3 & Vector3::operator-=(const Vector3 &v)
{
	Vector3 Res;
	Res.x = this->x - v.x;
	Res.y = this->y - v.y;
	Res.z = this->z - v.z;
	return Res;
}

Vector3 & Vector3::operator*=(float f)
{
	Vector3 Res;
	Res.x = this->x*f;
	Res.y = this->y*f;
	Res.z = this->z*f;
	return Res;
}

Vector3 & Vector3::operator/=(float f)
{
	Vector3 Res;
	Res.x = this->x / f;
	Res.y = this->y / f;
	Res.z = this->z / f;
	return Res;
}

Vector3 Vector3::operator+(const Vector3 &v) const
{
	Vector3 Res;
	Res.x = this->x + v.x;
	Res.y = this->y + v.y;
	Res.z = this->z + v.z;
	return Res;
}

Vector3 Vector3::operator-(const Vector3 &v) const
{
	Vector3 Res;
	Res.x = this->x - v.x;
	Res.y = this->y - v.y;
	Res.z = this->z - v.z;
	return Res;
}

Vector3 Vector3::operator*(float f) const
{
	Vector3 Res;
	Res.x = this->x*f;
	Res.y = this->y*f;
	Res.z = this->z*f;
	return Res;
}

Vector3 Vector3::operator/(float f) const
{
	Vector3 Res;
	Res.x = this->x / f;
	Res.y = this->y / f;
	Res.z = this->z / f;
	return Res;
}

Vector3 Vector3::operator*(const Vector3 &v)
{
	Vector3 Res;
	Res.x = this->x*v.x;
	Res.y = this->y*v.y;
	Res.z = this->z*v.z;
	Res.w = this->w*v.w;
	return Res;
}

Vector3 Vector3::Cross3(Vector3 & V)
{
	Vector3 R;
	R.x = this->y*V.z - this->z*V.y;
	R.y = this->z*V.x - this->x*V.z;
	R.z = this->x*V.y - this->y*V.x;
	R.w = 0;
	return R;
}

bool Vector3::operator==(const Vector3 &v) const
{
	if (this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w)
		return true;
	else
		return false;
}

bool Vector3::operator!=(const Vector3 &v) const
{
	if (this->x != v.x || this->y != v.y || this->z != v.z || this->w != v.w)
		return true;
	else
		return false;
}

Vector3 operator*(float f, const Vector3 &v)
{
	Vector3 Res;
	Res.x = v.x*f;
	Res.y = v.y*f;
	Res.z = v.z*f;
	return Res;
}
