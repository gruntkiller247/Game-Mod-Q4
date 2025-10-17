#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

class rvWeaponMachinegun : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponMachinegun );

	rvWeaponMachinegun ( void );

	virtual void		Spawn				( void );
	virtual void		Think				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void					PreSave				( void );
	void					PostSave			( void );

	//mattMod
	void				chooseMods();
	bool modsMade = false;
	int clipSize;
	int dmgEdit;
	float spreadEdit;


protected:

	float				spreadZoom;
	bool				fireHeld;

	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	stateResult_t		State_Idle			( const stateParms_t& parms );
	stateResult_t		State_Fire			( const stateParms_t& parms );
	stateResult_t		State_Reload		( const stateParms_t& parms );
	stateResult_t		State_Flashlight	( const stateParms_t& parms );

	CLASS_STATES_PROTOTYPE ( rvWeaponMachinegun );
};

CLASS_DECLARATION( rvWeapon, rvWeaponMachinegun )
END_CLASS

void rvWeaponMachinegun::chooseMods()
{
	idRandom temp;
	temp.SetSeed(gameLocal.time);

	this->clipSize = temp.RandomInt(80) + 30;
	dmgEdit = temp.RandomInt(4);
	spreadEdit = temp.RandomFloat();
	setClipSize(this->clipSize);

	if (dmgEdit == 0)
	{
		attackDict.Set("def_damage", "damage_machinegun_3");
	}
	else if (dmgEdit == 1)
	{
		attackDict.Set("def_damage", "damage_machinegun_30");
	}
	else if (dmgEdit == 2)
	{
		attackDict.Set("def_damage", "damage_machinegun_10");
	}
	else
	{
		//default
	}
}

/*
================
rvWeaponMachinegun::rvWeaponMachinegun
================
*/
rvWeaponMachinegun::rvWeaponMachinegun ( void ) {
}

/*
================
rvWeaponMachinegun::Spawn
================
*/
void rvWeaponMachinegun::Spawn ( void ) {
	spreadZoom = spawnArgs.GetFloat ( "spreadZoom" );
	fireHeld   = false;
		
	SetState ( "Raise", 0 );	
	
	Flashlight ( owner->IsFlashlightOn() );

	//mattMod
	idPlayer* p = static_cast<idPlayer*>(owner);

	if (!p-> lmgModsMade || p->killLMG)
	{
		chooseMods();
		p->killLMG = false;
		p->lmgModsMade = true;
		p->lmgDmg = dmgEdit;
		p->lmgSpread = spreadEdit;
		p->lmgClipSize = this->clipSize;

	}
	else
	{
		modsMade = true;
		dmgEdit = p->lmgDmg;
		spreadEdit = p->lmgSpread;
		this->clipSize = p->lmgClipSize;
		setClipSize(this->clipSize);

		if (dmgEdit == 0)
		{
			attackDict.Set("def_damage", "damage_machinegun_3");
		}
		else if (dmgEdit == 1)
		{
			attackDict.Set("def_damage", "damage_machinegun_30");
		}
		else if (dmgEdit == 2)
		{
			attackDict.Set("def_damage", "damage_machinegun_10");
		}
		else
		{
			;
		}


	}
}

/*
================
rvWeaponMachinegun::Save
================
*/
void rvWeaponMachinegun::Save ( idSaveGame *savefile ) const {
	savefile->WriteFloat ( spreadZoom );
	savefile->WriteBool ( fireHeld );
}

/*
================
rvWeaponMachinegun::Restore
================
*/
void rvWeaponMachinegun::Restore ( idRestoreGame *savefile ) {
	savefile->ReadFloat ( spreadZoom );
	savefile->ReadBool ( fireHeld );
}

/*
================
rvWeaponMachinegun::PreSave
================
*/
void rvWeaponMachinegun::PreSave ( void ) {
}

/*
================
rvWeaponMachinegun::PostSave
================
*/
void rvWeaponMachinegun::PostSave ( void ) {
}


/*
================
rvWeaponMachinegun::Think
================
*/
void rvWeaponMachinegun::Think()
{
	rvWeapon::Think();
	if ( zoomGui && owner == gameLocal.GetLocalPlayer( ) ) {
		zoomGui->SetStateFloat( "playerYaw", playerViewAxis.ToAngles().yaw );
	}
}

/*
================
rvWeaponMachinegun::UpdateFlashlight
================
*/
bool rvWeaponMachinegun::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponMachinegun::Flashlight
================
*/
void rvWeaponMachinegun::Flashlight ( bool on ) {
	owner->Flashlight ( on );
	
	if ( on ) {
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponMachinegun )
	STATE ( "Idle",				rvWeaponMachinegun::State_Idle)
	STATE ( "Fire",				rvWeaponMachinegun::State_Fire )
	STATE ( "Reload",			rvWeaponMachinegun::State_Reload )
	STATE ( "Flashlight",		rvWeaponMachinegun::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponMachinegun::State_Idle
================
*/
stateResult_t rvWeaponMachinegun::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
		
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}		
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}

			if ( fireHeld && !wsfl.attack ) {
				fireHeld = false;
			}
			if ( !clipSize ) {
				if ( !fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
			} else {
				if ( !fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}  
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}				
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponMachinegun::State_Fire
================
*/
stateResult_t rvWeaponMachinegun::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.zoom ) {
				gameLocal.Printf("LMG zoom values not changed\n");
				nextAttackTime = gameLocal.time + (altFireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( true, 1, spreadZoom, 0, 1.0f );
				fireHeld = true;
			} else {

				gameLocal.Printf("LMG dmg value: %s\n", attackDict.GetString("def_damage"));
				gameLocal.Printf("LMG mag size: %i\n", clipSize);

				nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( false, 1, spread*spreadEdit, 0, 1.0f );
			}
			PlayAnim ( ANIMCHANNEL_ALL, "fire", 0 );	
			return SRESULT_STAGE ( STAGE_WAIT );
	
		case STAGE_WAIT:		
			if ( !fireHeld && wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() && !wsfl.lowerWeapon ) {
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetState ( "Idle", 0 );
				return SRESULT_DONE;
			}		
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}			
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponMachinegun::State_Reload
================
*/
stateResult_t rvWeaponMachinegun::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SetStatus ( WP_RELOAD );
			PlayAnim ( ANIMCHANNEL_ALL, "reload", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				AddToClip ( ClipSize() );
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
			

/*
================
rvWeaponMachinegun::State_Flashlight
================
*/
stateResult_t rvWeaponMachinegun::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};	
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:			
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play		
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );
			
		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}
			
			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}
			
			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
