#include "Goomba.h"

CGoomba::CGoomba(float x, float y):CGameObject(x, y)
{
	this->ax = 0;
	this->ay = 0;
	die_start = -1;
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (state == GOOMBA_STATE_DIE)
	{
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{ 
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	// Handle rising logic
	if (state == GOOMBA_STATE_RISE && y <= originalY - GOOMBA_BBOX_HEIGHT)
	{
		y = originalY - GOOMBA_BBOX_HEIGHT;
		SetState(GOOMBA_STATE_WALKING); // Transition to walking state
	}

	if ((state == GOOMBA_STATE_DIE) && (GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CGoomba*>(e->obj)) return;

	if (e->ny != 0)
	{
		if (state != GOOMBA_STATE_RISE) // Allow rising
			vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}


void CGoomba::Render()
{
	//Keep the goomba in suspend aniamtion when game is pause or freeze
	CGame* game = CGame::GetInstance();
	if (game->IsPaused() || game->IsTimeFrozen()) {
		CSprites* s = CSprites::GetInstance();
		s->Get(currentAniId)->Draw(x, y);
		return;
	}

	//Render Normally
	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == GOOMBA_STATE_DIE) 
	{
		aniId = ID_ANI_GOOMBA_DIE;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x,y, &currentAniId); //&currentAniId to get current sprite id 
	RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			die_start = GetTickCount64();
			y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
			vx = 0;
			vy = 0;
			ay = 0; 
			break;

		case GOOMBA_STATE_WALKING: 
			vx = -GOOMBA_WALKING_SPEED;
			ay = GOOMBA_GRAVITY;
			break;

		case GOOMBA_STATE_RISE:
			originalY = y;
			ay = 0;
			vx = 0;
			vy = -GOOMBA_RISE_SPEED; 
			break;
	}
}
