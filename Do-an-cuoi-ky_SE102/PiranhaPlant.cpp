﻿#include "PiranhaPlant.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(float x, float y) : CEnemy(x, y)
{
	this->x = x;
	this->y = y;
	originalY = y;
	SetState(PIRANHA_STATE_HIDE);
	isShooting = false;				// Initialize shooting state
}

void CPiranhaPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - PIRANHA_BBOX_WIDTH / 2;
	top = y - PIRANHA_BBOX_HEIGHT / 2;
	right = left + PIRANHA_BBOX_WIDTH;
	bottom = top + PIRANHA_BBOX_HEIGHT;
}

void CPiranhaPlant::SetState(int state)
{
	stateStartTime = GetTickCount64();
	this->state = state;

	switch (state) {
	case PIRANHA_STATE_HIDE:
		isShooting = false; // Reset shooting state
		vy = 0;
		break;
	case PIRANHA_STATE_RISE:
		isShooting = false; // Reset shooting state
		vy = -PIRANHA_MOVE_SPEED;
		break;
	case PIRANHA_STATE_SNIP:
		vy = 0;
		break;
	case PIRANHA_STATE_DIVE:
		isShooting = false; // Reset shooting state
		vy = PIRANHA_MOVE_SPEED;
		break;
	default:
		break;
	}
}

int CPiranhaPlant::GetSnippingDirection()
{
	// Get the current scene and player
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// Check the position of the Piranha Plant relative to Mario
	if (marioX < x) {
		if (marioY < y)
			return 0;		// UP_LEFT
		else
			return 1;		// DOWN_LEFT
	}
	else {
		if (marioY < y)
			return 2;		// UP_RIGHT
		else
			return 3;		// DOWN_RIGHT
	}
}

void CPiranhaPlant::Render()
{
	if (state == PIRANHA_STATE_HIDE)
		return;

	int aniId = -1;

	if (state == PIRANHA_STATE_RISE || state == PIRANHA_STATE_DIVE) {
		int direction = GetSnippingDirection();
		if (direction == 0 || direction == 1)
			aniId = PIRANHA_ANI_LEFT_RISE_DIVE;
		else
			aniId = PIRANHA_ANI_RIGHT_RISE_DIVE;
	}
	else if (state == PIRANHA_STATE_SNIP) {
		int direction = GetSnippingDirection();
		if (direction == 0)
			aniId = PIRANHA_ANI_UP_LEFT;
		else if (direction == 1)
			aniId = PIRANHA_ANI_DOWN_LEFT;
		else if (direction == 2)
			aniId = PIRANHA_ANI_UP_RIGHT;
		else
			aniId = PIRANHA_ANI_DOWN_RIGHT;
	}

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

void CPiranhaPlant::Shoot(int direction)
{
	CFireBullet* fireBullet = new CFireBullet(x, y);
	CGame* game = CGame::GetInstance();	
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	currentScene->Add(fireBullet);

	switch (direction) {
	case 0:	// UP_LEFT
		fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_HIGH);
		break;
	case 1:	// DOWN_LEFT
		fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_LOW);
		break;
	case 2:	// UP_RIGHT
		fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_HIGH);
		break;
	case 3:	// DOWN_RIGHT
		fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_LOW);
		break;
	default:
		break;
	}

	isShooting = true; // Set shooting state to true
}

bool CPiranhaPlant::IsTargetInRange() {
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float backBufferWidth = game->GetBackBufferWidth();
	float backBufferHeight = game->GetBackBufferHeight();
	
	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	float relativeX = fabs(marioX - x);
	float relativeY = fabs(marioY - y);

	return (relativeX < backBufferWidth / 2.0f
		&& relativeY < backBufferHeight / 2.0f);
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();

	bool isInRange = IsTargetInRange();

	if (!isInRange) {		// when mario is not in range (not in the camera)
		if (state == PIRANHA_STATE_SNIP)
			state = PIRANHA_STATE_DIVE;
		else if (state == PIRANHA_STATE_HIDE)
			stateStartTime = now;		// stay hide, if not implement this, it will rise when the timeout is over
		DebugOut(L"[INFO] Piranha plant is not in range\n");
	}

	// Mario is in the range of snipping
	switch (state) {
	case PIRANHA_STATE_HIDE:
		if (now - stateStartTime > PIRANHA_HIDE_TIMEOUT) {
			SetState(PIRANHA_STATE_RISE);
		}
		break;

	case PIRANHA_STATE_RISE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY - PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_SNIP);
		}
		break;

	case PIRANHA_STATE_SNIP:
		if (!isShooting) {
			int direction = GetSnippingDirection();
			Shoot(direction);
		}

		if (now - stateStartTime > PIRANHA_SNIP_TIMEOUT) {		
			SetState(PIRANHA_STATE_DIVE);
		}
		break;

	case PIRANHA_STATE_DIVE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY + PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_HIDE);
		}
		break;

	default:
		break;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}