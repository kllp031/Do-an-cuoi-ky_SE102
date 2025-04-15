#pragma once
#include "GameObject.h"

class CEnemy : public CGameObject
{
public:
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) = 0;
	virtual void Render() = 0;
	virtual int IsCollidable() { return 1; };
};

