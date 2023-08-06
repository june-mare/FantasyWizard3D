#include "Sprite.h"

bool WhichDiffDirection(side& vec, float x, float y)
{
	side priv = vec;
	bool verOrHori = abs(x) < abs(y) 
		? true   //�c����
		: false; //������
	
	//����
	{
		if (x > 0 && !verOrHori)
			vec = side::right;
		else if (x < 0 && !verOrHori)
			vec = side::left;
	}
	//�c��
	{
		if (y > 0 && verOrHori)
			vec = side::up;
		else if (y < 0 && verOrHori)
			vec = side::down;
	}

	return priv != vec;
}


bool WhichDiffDirection(side& vec, glm::vec3 coordinate)
{
	side priv = vec;
	bool verOrHori = abs(coordinate.x) < abs(coordinate.y)
		? true   //�c����
		: false; //������

	//����
	{
		if (coordinate.x > 0 && !verOrHori)
			vec = side::right;
		else if (coordinate.x < 0 && !verOrHori)
			vec = side::left;
	}
	//�c��
	{
		if (coordinate.y > 0 && verOrHori)
			vec = side::up;
		else if (coordinate.y < 0 && verOrHori)
			vec = side::down;
	}

	return priv != vec;
}