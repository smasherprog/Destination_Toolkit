#include "stdafx.h"
#include "UI_Camera.h"
#include "../MY_UI/cWidgetUtilities.h"
#include "cCamera.h"
#include "Global_GameData.h"
#include "../Common_Game_Objects_Lib/cBaseNetwork.h"
#include "cDemoControl.h"
#include "../Utilities/NetLib/cUDP_Client.h"

UI_Camera* UI_Camera::self=0;

UI_Camera::~UI_Camera(){
	self =0;
	MY_UI::GetApplicationLayer()->SetCamera(0);
}
UI_Camera::UI_Camera(cDemoControl* demo) : cWidgetBase(0), DemoControl(*demo) {
	assert(self ==0);
	SetControlBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	SetClientBounds(MY_UI::Utilities::Rect(0, 0, 2000, 2000));
	OnRightDrag.Add(std::bind(&UI_Camera::DraggingCamera, this));
	SetDraggable(true);
	SetName("UI_Camera");
	MY_UI::GetApplicationLayer()->SetCamera(this);
}

	void UI_Camera::DraggingCamera(){
		Camera.Pitch(Delta_Mousey);
		Camera.Heading(Delta_Mousex);
	}
	void UI_Camera::KeyDown(){// no need to call the lower version, this  has no need for that functionality
		uint8_t movestate = 0;
		if(MyCharacter.get()){
			movestate = MyCharacter->MovementState;
		}
		uint16_t header=-1;

		switch(Current_Key){
		case(KEY_Q): 
			Camera.Strafing =-1.0f;
			break;
		case(KEY_E): 
			Camera.Strafing =1.0f;
			break;
		case(KEY_W): 
		case(KEY_UP): 
			Camera.Walking = 1.0f;
			if(MyCharacter.get()){
				header = Network::GS_RUN_FORWARD;
				MyCharacter->RunFoward();
				if(MyCharacterMesh->mAnimator.SetAnimation("Run"))
					MyCharacterMesh->AnimationTime=0.0f;// important to reset this
			}
			break;
		case(KEY_S): 
		case(KEY_DOWN):
			Camera.Walking = -1.0f;
			if(MyCharacter.get()){
				MyCharacter->WalkBackward();
				header = Network::GS_WALK_BACKWARD;	
				MyCharacterMesh->mAnimator.PlayAnimationBackward();
				if(MyCharacterMesh->mAnimator.SetAnimation("Walk"))
					MyCharacterMesh->AnimationTime=0.0f;// important to reset this

			}
			break;
		case(KEY_A): 
		case(KEY_LEFT): 
			if(Mouse_RButton_Down)	
				Camera.Strafing =-1.0f;
			else {
				Camera.Rotating = -1.0f;
			}
			if(!Mouse_RButton_Down && MyCharacter.get()){
				MyCharacter->RotateLeft();
				header = Network::GS_ROTATE_LEFT;
			}
			break;
		case(KEY_D): 
		case(KEY_RIGHT): 
			if(Mouse_RButton_Down) {
				Camera.Strafing = 1.0f;
			}else {
				Camera.Rotating =1.0f;
			}
			if(!Mouse_RButton_Down && MyCharacter.get()){
				MyCharacter->RotateRight();
				header = Network::GS_ROTATE_RIGHT;
			}
			break;
		case(KEY_SPACE): 
			if(MyCharacter.get()){
				if(MyCharacterMesh->mAnimator.SetAnimation("Jump"))
					MyCharacterMesh->AnimationTime=0.0f;// important to reset this
			}
			break;
		};
		if((DemoControl.Peer !=0 ) & (header!=-1) ){
			if(MyCharacter.get()){
				if(MyCharacter->MovementState != movestate) DemoControl.ReliableSendTo(header);
			}
		}
	}

	void UI_Camera::KeyUp(){
		if(Current_Key == KEY_ESCAPE) {
			DemoControl.Focus = ShuttingDown;// shut down the democontrol class
		} 
		uint8_t movestate =0;
		if(MyCharacter.get()){
			movestate = MyCharacter->MovementState;
		}
		uint16_t header=-1;
		switch(Current_Key){
		case(KEY_Q): 
			Camera.Strafing =0.0f;
			break;
		case(KEY_E): 
			Camera.Strafing =0.0f;
			break;
		case(KEY_W): 
		case(KEY_UP): 
			Camera.Walking = 0.0f;
			if(MyCharacter.get()){
				MyCharacter->StopMovement();
				header = Network::GS_STOP_MOVEMENT;		
				if(MyCharacterMesh->mAnimator.SetAnimation("Idle"))
					MyCharacterMesh->AnimationTime=0.0f;// important to reset this
			}
			break;
		case(KEY_S): 
		case(KEY_DOWN):
			Camera.Walking = 0.0f;
			if(MyCharacter.get()){
				MyCharacter->StopMovement();
				header = Network::GS_STOP_MOVEMENT;
				MyCharacterMesh->mAnimator.PlayAnimationForward();
				if(MyCharacterMesh->mAnimator.SetAnimation("Idle"))
					MyCharacterMesh->AnimationTime=0.0f;// important to reset this
			}
			break;
		case(KEY_A): 
		case(KEY_LEFT): 
			Camera.Strafing =  Camera.Rotating = 0.0f;
			if(MyCharacter.get()){
				MyCharacter->StopRotation();
				header = Network::GS_STOP_ROTATION;
			}
			break;
		case(KEY_D): 
		case(KEY_RIGHT): 
			Camera.Strafing = Camera.Rotating =0.0f;
			if(MyCharacter.get()){
				MyCharacter->StopRotation();
				header = Network::GS_STOP_ROTATION;
			}
			break;
		};
		if((DemoControl.Peer !=0 ) & (header!=-1)){
			if(MyCharacter.get()){
				if(MyCharacter->MovementState != movestate) DemoControl.ReliableSendTo(header);
			}
		}
	}

