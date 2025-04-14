#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    //DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
    switch (KeyCode)
    {
    case DIK_DOWN:
        mario->SetState(MARIO_STATE_SIT);
        break;
    case DIK_S:
        mario->SetState(MARIO_STATE_JUMP);
        break;
    case DIK_1:
        mario->SetLevel(MARIO_LEVEL_SMALL);
        break;
    case DIK_2:
        mario->SetLevel(MARIO_LEVEL_BIG);
        break;
    case DIK_R: // reset
        //Reload();
        break;
	case DIK_P:
		// Pause game
		CGame::GetInstance()->TogglePause();
		break;
    }
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
    CGame* game = CGame::GetInstance();

    //DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
    switch (KeyCode)
    {
    case DIK_RIGHT:
        //The order of these codditions is important
        if (mario->GetVx() > 0.05f)  // Only decelerate if moving at a meaningful speed
            mario->SetState(MARIO_STATE_DECELERATE_RIGHT);
        else
            mario->SetState(MARIO_STATE_IDLE);  // Stop immediately if moving very slowly
        // Check if down key is pressed when right key is released
        if (game->IsKeyDown(DIK_DOWN)) {
            mario->SetState(MARIO_STATE_SIT);
            DebugOut(L"Sitting\n");
        }
        break;

    case DIK_LEFT:
        if (mario->GetVx() < -0.05f)  // Only decelerate if moving at a meaningful speed
            mario->SetState(MARIO_STATE_DECELERATE_LEFT);
        else
            mario->SetState(MARIO_STATE_IDLE);  // Stop immediately if moving very slowly
        if (game->IsKeyDown(DIK_DOWN))
            mario->SetState(MARIO_STATE_SIT);
        break;

    case DIK_S:
        mario->SetState(MARIO_STATE_RELEASE_JUMP);
        // Check if down key is presed when jump key is released
        if (game->IsKeyDown(DIK_DOWN))
            mario->SetState(MARIO_STATE_SIT);
        break;

    case DIK_DOWN:
        mario->SetState(MARIO_STATE_SIT_RELEASE);
        break;
    }
}

void CSampleKeyHandler::KeyState(BYTE* states)
{
    CGame* game = CGame::GetInstance();
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    // Handle movement even when sitting
    if (game->IsKeyDown(DIK_RIGHT))
    {
        if (game->IsKeyDown(DIK_A))
            mario->SetState(MARIO_STATE_RUNNING_RIGHT);
        else
            mario->SetState(MARIO_STATE_WALKING_RIGHT);
    }
    else if (game->IsKeyDown(DIK_LEFT))
    {
        if (game->IsKeyDown(DIK_A))
            mario->SetState(MARIO_STATE_RUNNING_LEFT);
        else
            mario->SetState(MARIO_STATE_WALKING_LEFT);
    }
    else
    {
        // Check velocity thresholds to determine when to stop
        float vx = mario->GetVx();

        // If Mario is in a deceleration state, let it continue
        if (mario->GetState() == MARIO_STATE_DECELERATE_RIGHT ||
            mario->GetState() == MARIO_STATE_DECELERATE_LEFT)
        {
            // If nearly stopped, set to IDLE
            if (abs(vx) < 0.01f)
                mario->SetState(MARIO_STATE_IDLE);
        }
        // If not already in a special state and not moving, go IDLE
        else if (abs(vx) < 0.01f)
            mario->SetState(MARIO_STATE_IDLE);
    }
}
