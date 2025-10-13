#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"
#include "../Player.h"



const int SHOTGUN_MOD_AMMO = BIT(0);


class rvWeaponShotgun : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponShotgun );

	rvWeaponShotgun ( void );

	virtual void			Spawn				( void );
	void					Save				( idSaveGame *savefile ) const;
	void					Restore				( idRestoreGame *savefile );
	void					PreSave				( void );
	void					PostSave			( void );

	//mattMod Changes
	void				chooseMods();
	int spreadEdit;
	bool modsMade = false;
	//int newHitscans;
	int damageMod;
	int clipSize;
	int newHitscans;

protected:
	int						hitscans;

private:

	stateResult_t		State_Idle		( const stateParms_t& parms );
	stateResult_t		State_Fire		( const stateParms_t& parms );
	stateResult_t		State_Reload	( const stateParms_t& parms );
	
	CLASS_STATES_PROTOTYPE( rvWeaponShotgun );
};

CLASS_DECLARATION( rvWeapon, rvWeaponShotgun )
END_CLASS

//mattMod
void rvWeaponShotgun::chooseMods()
{
	if (!modsMade)
	{
		//gameLocal.Printf("Class: %s\n", GetClassname());
		//make mod choices
		//3 mod changes: Spread, dmg, wether it has DOT/Venon?

		idRandom temp;
		temp.SetSeed(gameLocal.time);
		
		this->clipSize = temp.RandomInt(15)+1;
		setClipSize(this->clipSize);
		damageMod = temp.RandomInt(100);

		newHitscans = temp.RandomInt(4);

		//100, 20, 10, 5
		if (damageMod >= 90)
		{
			//10% of the time do massive damage
			attackDict.Set("def_damage", "damage_pellet_100");
			attackDict.Set("def_damage_flesh", "damage_pellet_100");
		}
		else if (damageMod  >=70)
		{
			attackDict.Set("def_damage", "damage_pellet_20");
			attackDict.Set("def_damage_flesh", "damage_pellet_20");
		}
		else if (damageMod >=50)
		{
			attackDict.Set("def_damage", "damage_pellet_10");
			attackDict.Set("def_damage_flesh", "damage_pellet_10");
		}
		else if (damageMod  >= 30)
		{
			attackDict.Set("def_damage", "damage_pellet_5");
			attackDict.Set("def_damage_flesh", "damage_pellet_5");
		}
		else
		{
			//change nothing and everything is normal
			attackDict.Set("def_damage", "damage_pellet_2");
			attackDict.Set("def_damage_flesh", "damage_pellet_2");
		}


		modsMade = true;
	}
	else
	{
		//sys->DebugPrintf("Mods already created for weapon");
	}
}

/*
================
rvWeaponShotgun::rvWeaponShotgun
================
*/
rvWeaponShotgun::rvWeaponShotgun( void ) {
}

/*
================
rvWeaponShotgun::Spawn
================
*/
void rvWeaponShotgun::Spawn( void ) {
	//mattMod
	hitscans = spawnArgs.GetFloat("hitscans"); 
	
	SetState( "Raise", 0 );
	//gameLocal.Printf("Value of modsMade before call: " + modsMade);

	//mattMod
	idPlayer* p = static_cast<idPlayer*>(owner);

	if (!p->shotgunModsMade || p->killShotgun) {
		chooseMods(); 
		p->killShotgun = false;
		p->shotgunModsMade = true;
		p->shotgunDamageMod = damageMod;
		p->shotgunClipSize = this->clipSize;
		p->shotgunHitscans = newHitscans; //this actually is spread
	}
	else {
		damageMod = p->shotgunDamageMod;
		this->clipSize = p->shotgunClipSize;
		setClipSize(this->clipSize);
		//ammoClip = clipSize;
		modsMade = true;

		if (damageMod >= 90)
		{
			//10% of the time do massive damage
			attackDict.Set("def_damage", "damage_pellet_100");
			attackDict.Set("def_damage_flesh", "damage_pellet_100");
		}
		else if (damageMod >= 70)
		{
			attackDict.Set("def_damage", "damage_pellet_20");
			attackDict.Set("def_damage_flesh", "damage_pellet_20");
		}
		else if (damageMod >= 50)
		{
			attackDict.Set("def_damage", "damage_pellet_10");
			attackDict.Set("def_damage_flesh", "damage_pellet_10");
		}
		else if (damageMod >= 30)
		{
			attackDict.Set("def_damage", "damage_pellet_5");
			attackDict.Set("def_damage_flesh", "damage_pellet_5");
		}
		else
		{
			//change nothing and everything is normal
			attackDict.Set("def_damage", "damage_pellet_2");
			attackDict.Set("def_damage_flesh", "damage_pellet_2");
		}

		if (newHitscans == 1)
		{
			attackDict.Set("spread","1");
		}
		else if (newHitscans == 2)
		{
			attackDict.Set("spread", "3");
		}
		else if (newHitscans == 3)
		{
			attackDict.Set("spread", "5");
		}
		else if (newHitscans == 4)
		{
			attackDict.Set("spread", "15");
		}
		else
		{
			;
		}
	}

	
	//gameLocal.Printf("Value of modsMade after call: " + modsMade);
	
}

/*
================
rvWeaponShotgun::Save
================
*/
void rvWeaponShotgun::Save( idSaveGame *savefile ) const {
	savefile->WriteInt(damageMod);
	savefile->WriteBool(modsMade);
	savefile->WriteInt(clipSize);

	gameLocal.Printf("Saving damageMod as: %d\n", damageMod);
	//gameLocal.Printf("Saving modsMade as: " + modsMade);
	gameLocal.Printf("Saving clipSize as: %d" + clipSize);

}

/*
================
rvWeaponShotgun::Restore
================
*/
void rvWeaponShotgun::Restore( idRestoreGame *savefile ) {
	hitscans   = spawnArgs.GetFloat( "hitscans" );
	savefile->ReadInt(damageMod);
	savefile->ReadBool(modsMade);
	savefile->ReadInt(clipSize);
	setClipSize(clipSize);

	gameLocal.Printf("Loading damageMod as: %d\n", damageMod);
	//gameLocal.Printf("Loading modsMade as: " + damageMod);
	gameLocal.Printf("Loading clipSize as: %d", clipSize);
	
	if (modsMade) {
		if (damageMod >= 90) {
			attackDict.Set("def_damage", "damage_pellet_100");
			attackDict.Set("def_damage_flesh", "damage_pellet_100");
		}
		else if (damageMod >= 70) {
			attackDict.Set("def_damage", "damage_pellet_20");
			attackDict.Set("def_damage_flesh", "damage_pellet_20");
		}
		else if (damageMod >= 50) {
			attackDict.Set("def_damage", "damage_pellet_10");
			attackDict.Set("def_damage_flesh", "damage_pellet_10");
		}
		else if (damageMod >= 30) {
			attackDict.Set("def_damage", "damage_pellet_5");
			attackDict.Set("def_damage_flesh", "damage_pellet_5");
		}
	}
}

/*
================
rvWeaponShotgun::PreSave
================
*/
void rvWeaponShotgun::PreSave ( void ) {
}

/*
================
rvWeaponShotgun::PostSave
================
*/
void rvWeaponShotgun::PostSave ( void ) {
}


/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION( rvWeaponShotgun )
	STATE( "Idle",				rvWeaponShotgun::State_Idle)
	STATE( "Fire",				rvWeaponShotgun::State_Fire )
	STATE( "Reload",			rvWeaponShotgun::State_Reload )
END_CLASS_STATES

/*
================
rvWeaponShotgun::State_Idle
================
*/
stateResult_t rvWeaponShotgun::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( !AmmoAvailable( ) ) {
				SetStatus( WP_OUTOFAMMO );
			} else {
				SetStatus( WP_READY );
			}
		
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState( "Lower", 4 );
				return SRESULT_DONE;
			}		
			if ( !clipSize ) {
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable ( ) ) {
					SetState( "Fire", 0 );
					return SRESULT_DONE;
				}  
			} else {				
				if ( gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState( "Fire", 0 );
					return SRESULT_DONE;
				}  
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState( "Reload", 4 );
					return SRESULT_DONE;			
				}				
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponShotgun::State_Fire
================
*/
stateResult_t rvWeaponShotgun::State_Fire( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	




	switch ( parms.stage ) {
		case STAGE_INIT:
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));

			//gameLocal.Printf("Shotgun firing with damage: %d\n", attackDict.GetInt("damage"));
			gameLocal.Printf("Shotgun using damage def: %s\n", attackDict.GetString("def_damage"));
			gameLocal.Printf("Shotgun using number of spread: %d\n", attackDict.GetString("spread"));


			Attack( false, hitscans, newHitscans, 0, 1.0f );


			PlayAnim( ANIMCHANNEL_ALL, "fire", 0 );	
			return SRESULT_STAGE( STAGE_WAIT );
	
		case STAGE_WAIT:
			if ( (!gameLocal.isMultiplayer && (wsfl.lowerWeapon || AnimDone( ANIMCHANNEL_ALL, 0 )) ) || AnimDone( ANIMCHANNEL_ALL, 0 ) ) {
				SetState( "Idle", 0 );
				return SRESULT_DONE;
			}									
			if ( wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() ) {
				SetState( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( clipSize ) {
				if ( (wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip())) ) {
					SetState( "Reload", 4 );
					return SRESULT_DONE;			
				}				
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponShotgun::State_Reload
================
*/
stateResult_t rvWeaponShotgun::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
		STAGE_RELOADSTARTWAIT,
		STAGE_RELOADLOOP,
		STAGE_RELOADLOOPWAIT,
		STAGE_RELOADDONE,
		STAGE_RELOADDONEWAIT
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SetStatus ( WP_RELOAD );
			
			if ( mods & SHOTGUN_MOD_AMMO ) {				
				PlayAnim ( ANIMCHANNEL_ALL, "reload_clip", parms.blendFrames );
			} else {
				PlayAnim ( ANIMCHANNEL_ALL, "reload_start", parms.blendFrames );
				return SRESULT_STAGE ( STAGE_RELOADSTARTWAIT );
			}
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
			
		case STAGE_RELOADSTARTWAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				return SRESULT_STAGE ( STAGE_RELOADLOOP );
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
			
		case STAGE_RELOADLOOP:		
			if ( (wsfl.attack && AmmoInClip() ) || AmmoAvailable ( ) <= AmmoInClip ( ) || AmmoInClip() == ClipSize() ) {
				return SRESULT_STAGE ( STAGE_RELOADDONE );
			}
			PlayAnim ( ANIMCHANNEL_ALL, "reload_loop", 0 );
			return SRESULT_STAGE ( STAGE_RELOADLOOPWAIT );
			
		case STAGE_RELOADLOOPWAIT:
			if ( (wsfl.attack && AmmoInClip() ) || wsfl.netEndReload ) {
				return SRESULT_STAGE ( STAGE_RELOADDONE );
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				AddToClip( 1 );
				return SRESULT_STAGE ( STAGE_RELOADLOOP );
			}
			return SRESULT_WAIT;
		
		case STAGE_RELOADDONE:
			NetEndReload ( );
			PlayAnim ( ANIMCHANNEL_ALL, "reload_end", 0 );
			return SRESULT_STAGE ( STAGE_RELOADDONEWAIT );

		case STAGE_RELOADDONEWAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.attack && AmmoInClip ( ) && gameLocal.time > nextAttackTime ) {
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;	
}
			
