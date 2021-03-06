//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// NEWLY CREATED CODE
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING RADAR-FUNCTIONS, MANAGING OBJECTIVE RADAR

//HAZARDMODDING CO-OP SCRIPT MODIFICATION �2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopRadar.hpp"
#include "coopPlayer.hpp"
#include "player.h"
#include "mp_manager.hpp"

//reset radar, fixes issues with the radar not updating
void coop_radarReset( Player* player )
{
	if ( !player )return;

	//[b607] chrissstrahl - make sure we do not handle bots
	gentity_t *ent = NULL;
	ent = (gentity_t *)(Entity *)player;
	if (ent->svflags & SVF_BOT) {
		return;
	}

	for ( int i = 0; i < COOP_RADAR_MAX_BLIPS; i++ ) {
		player->coopPlayer.radarBlipLastPosition[i] = Vector( 0.0f , -999.0f , -999.0f );
		//[b607] chrissstrahl - reset only if active or first time - meant to reduce nettraffic
		if(player->coopPlayer.radarBlipActive[i] || !player->coopPlayer.radarFirstResetDone ) {
			DelayedServerCommand( player->entnum , va( "globalwidgetcommand cr%i disable" , i ) );
		}
		player->coopPlayer.radarBlipActive[i] = false;
	}
	//[b607] chrissstrahl - first reset shall only happen once
	player->coopPlayer.radarFirstResetDone = true;
	player->coopPlayer.radarSelectedActive = false;
}

float coop_radarAngleTo( const Vector& source , const Vector& dest )
{
	if ( source == dest ) {
		return 0;
	}
	float dot;
	dot = Vector::Dot( source , dest );
	return acosf( dot );
}

float coop_radarSignedAngleTo( const Vector& source , const Vector& dest , const Vector& planeNormal )
{
	float angle = coop_radarAngleTo( source , dest );
	Vector cross;
	cross = Vector::Cross( source , dest );
	float dot;
	dot = Vector::Dot( cross , planeNormal );

	return dot < 0 ? -angle : angle;
}


void coop_radarUpdate( Player *player )
//hzm coop mod chrissstrahl - check for mission objective entities update radar
//CALCULATION CODE BY ALBERT DORN (dorn.albert)
{
	//[b607] chrissstrahl - make sure we do not handle bots
	gentity_t *ent = NULL;
	ent = (gentity_t *)(Entity *)player;
	if (ent->svflags & SVF_BOT) {
		return;
	}

	//hzm coop mod chrissstrahl - do not update for spectator and dead players
	Entity *target = NULL;
	target = player->GetTargetedEntity();

	//hzm coop mod chrissstrahl - 
	if (	player->coopPlayer.installed && player->health <= 0.0f ||
			multiplayerManager.isPlayerSpectator( player ) ||
			!target )
	{
		if ( player->coopPlayer.radarSelectedActive ) {
			player->coopPlayer.radarSelectedActive = false;
			DelayedServerCommand( player->entnum , "globalwidgetcommand crs disable" );
		}
	}

	//hzm coop mod chrissstrahl - update the radar every server frame (1 / sv_fps 20)
	if (	player->coopPlayer.installed != 1 ||
			level.mission_failed ||
			(player->coopPlayer.timeEntered + 3) > level.time || //[b607] chrissstrahl - make sure the player does not get updates in the beginning for better connect
			player->coopPlayer.lastTimeRadarUpdated + 0.05f > level.time || //[b607] chrissstrahl - changed from 0.1 to 0.05
			multiplayerManager.isPlayerSpectator( player ) ||
			player->health <= 0.0f )
	{
		return;
	}

	player->coopPlayer.lastTimeRadarUpdated = level.time;

	//hzm coop mod chrissstrahl - do not send radar update data while player is doing a puzzle
	ScriptVariable *solvingPuzzle = NULL;
	solvingPuzzle = player->entityVars.GetVariable( "_playerIsSolvingPuzzle" );
	if ( solvingPuzzle && solvingPuzzle->floatValue() == 1.0f ) {
		//gi.Printf("info: player doing puzzle\n");
		return;
	}

	//radar COOP_RADAR_CIRCLE_START at: 09 09 of hud
	//radar COOP_RADAR_BLIP_SIZE /2 = precise blip pos
	//radar center pos = 55 55
	//centerpos (55 55) plus radar circle start (55 + 9 - 55 + 9 = 64 64)
	//blip correction = (64 64 - -12 -12 = 52 52 )
	//radar centerpos = 52 52
	//calculate offset dynamicly
	///////////////////////////////////////////////////////////////////
	Vector	vRadarCenterPos( 55.0f , 55.0f , 0.0f );
	vRadarCenterPos[0] = ( ( vRadarCenterPos[0] + COOP_RADAR_CIRCLE_START ) - ( COOP_RADAR_BLIP_SIZE / 2 ) );
	vRadarCenterPos[1] = ( ( vRadarCenterPos[1] + COOP_RADAR_CIRCLE_START ) - ( COOP_RADAR_BLIP_SIZE / 2 ) );

	Entity	*eMiObjEntity = NULL;
	Entity	*eMiObjEntityValid = NULL;
	Vector	vRadarBlipLastPosition( 0.0f , 0.0f , 0.0f );
	Vector	vRealDistance;
	Vector	vRadarDistance;
	Vector	vNorth( -1.0f , 0.0f , 0.0f );
	int		iMiObjEntityItemNumber = 0;
	int		i;
	float	fRealDistance;
	float	fRadarDistance;
	float	fRadarAngle;
//	float	fPlayerAngle; //[b607] chrissstrahl - removed, it was not in use
	float	fBlipAxisX , fBlipAxisY;
	bool	targetedStillValid = false;

	//check routine is needed to detect what data needs to be send and not be send
	for ( i = 0; i < maxentities->integer; i++ ){
		eMiObjEntity = g_entities[i].entity;
		if ( !eMiObjEntity || eMiObjEntity->edict->s.missionObjective != 1 || iMiObjEntityItemNumber >= COOP_RADAR_MAX_BLIPS ){
			continue;
		}

		vRealDistance = eMiObjEntity->origin - player->origin;
		vRealDistance.z = 0.0f;

		fRealDistance = vRealDistance.lengthSquared();
		fRadarDistance = fRealDistance / COOP_RADAR_SCALE_FACTOR;

		//make sure the blip does not go outside the actual radar
		if ( fRadarDistance > COOP_RADAR_MAX_RADIUS ) fRadarDistance = COOP_RADAR_MAX_RADIUS;

		//set vector to length 1, but keep it pointed into the actual direction (Einheitsvector (L�nge = 1))
		vRadarDistance = vRealDistance;
		vRadarDistance.normalize();


		//make real pos of the object to a relative pos of the radar
		//Die reale position des objected wird zu realtiven einer position des radars
		vRadarDistance *= fRadarDistance;

		trace_t trace;
		player->GetViewTrace( trace , MASK_PROJECTILE , 1.0f );

		//[b607] chrissstrahl - fix radar not working when player is in cg_3rd_person
		Vector viewDir = Vector(0,0,0);
		Vector vorg = Vector(0,0,0);
		if (player->client->ps.pm_flags & PMF_CAMERA_VIEW){ // 3rd person automatic camera
			vorg = player->client->ps.camera_origin;
		}
		else if (!player->checkthirdperson()){ // First person
			vorg = player->origin;
			vorg.z += player->client->ps.viewheight;
		}
		else{ // Third person
			vorg = player->client->ps.camera_origin;
		}

		viewDir = trace.endpos - vorg; //[b607] chrissstrahl - use the correct player/camera origin now
		viewDir.z = 0.0f;

		vRealDistance.normalize();
		viewDir.normalize();
		Vector up( 0.0f , 0.0f , 1.0f );

		fRadarAngle = coop_radarSignedAngleTo( viewDir , vRealDistance , up );

		//debug
		//gi.Printf( va( "PlayerPos(%f,%f)\n" , player->origin.x , player->origin.y ) );
		//gi.Printf( va( "fRadarAngle : %f\n" , fRadarAngle ) );

		vRadarDistance = Vector( 0.0f , -fRadarDistance , 0.0f );

		fRadarAngle -= M_PI / 2;

		fBlipAxisX = vRadarCenterPos.x + ( ( sinf( fRadarAngle ) * vRadarDistance.x ) + ( cosf( fRadarAngle ) * vRadarDistance.y ) );
		fBlipAxisY = vRadarCenterPos.y + ( ( cosf( fRadarAngle ) * vRadarDistance.x ) - ( sinf( fRadarAngle ) * vRadarDistance.y ) );

		vRadarBlipLastPosition[0] = fBlipAxisX;
		vRadarBlipLastPosition[1] = fBlipAxisY;

		//hzm coop mod chrissstrahl - construct client command
		if ( player->coopPlayer.radarBlipLastPosition[iMiObjEntityItemNumber] != vRadarBlipLastPosition ){
			player->coopPlayer.radarBlipLastPosition[iMiObjEntityItemNumber] = vRadarBlipLastPosition;

			//hzm coop mod chrissstrahl - convert floats to int, to reduce traffic
			//[b607] chrissstrahl - if there is a lot send to player, do not update blips
			//The blips are updated every time the player moves or turns, so chances are high
			//no one will ever notice this, and if they do then there are nettraffic issues anyway
			//used to be send via DelayedServerCommand 
			if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
				gi.SendServerCommand(player->entnum,va("stufftext \"globalwidgetcommand cr%i rect %i %i %i %i\"\n", iMiObjEntityItemNumber, (int)fBlipAxisX, (int)fBlipAxisY, COOP_RADAR_BLIP_SIZE, COOP_RADAR_BLIP_SIZE));
			}

			if ( target ){
				if ( target == eMiObjEntity ){
					targetedStillValid = true;
					//[b607] chrissstrahl - if there is a lot send to player, do not update blips
					//The blips are updated every time the player moves or turns, so chances are high
					//no one will ever notice this, and if they do then there are nettraffic issues anyway
					//used to be send via DelayedServerCommand 
					if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
						gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand crs rect %i %i %i %i\"\n", (int)fBlipAxisX, (int)fBlipAxisY, COOP_RADAR_BLIP_SIZE, COOP_RADAR_BLIP_SIZE));
					}
				}
			}
		}

		//hzm coop mod chrissstrahl - enable active blip if it is not already
		if ( player->coopPlayer.radarBlipActive[iMiObjEntityItemNumber] == false ){
			player->coopPlayer.radarBlipActive[iMiObjEntityItemNumber] = true;
			//we need it to be reliable
			DelayedServerCommand( player->entnum , va( "globalwidgetcommand cr%i enable" , iMiObjEntityItemNumber ) );
		}

		//hzm coop mod chrissstrahl - keep track of the current Mission Objective Blip Marker
		iMiObjEntityItemNumber++;
	}

	//update (enable/disable) selected marker
	//bugfix - chrissstrahl - make sure it is deactivated if not needed
	if( targetedStillValid ) {
		if ( !player->coopPlayer.radarSelectedActive ) {
			player->coopPlayer.radarSelectedActive = true;
			//[b607] chrissstrahl - made seperate, because we need it to be reliable
			DelayedServerCommand(player->entnum,"globalwidgetcommand crs enable");
		}
	}

	//hzm coop mod chrissstrahl - update radar background compas disc, update it first that way the radar seams more responsive
	int iRadarAngle = ( int )player->client->ps.viewangles[1];
	if ( iRadarAngle < 0 ){ iRadarAngle = ( 359 + iRadarAngle ); }

	if ( iRadarAngle != player->coopPlayer.lastRadarAngle ){
		player->coopPlayer.lastRadarAngle = iRadarAngle;
		//[b607] chrissstrahl - if there is a lot send to player, do not update disc
		//The disc is updated every time the player turns, so chances are high
		//no one will ever notice this, and if they do then there are nettraffic issues anyway
		//used to be send via DelayedServerCommand 
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 64 ){
			gi.SendServerCommand(player->entnum,va("stufftext \"set coop_r %i\"\n",player->coopPlayer.lastRadarAngle));
		}
	}

	if ( iMiObjEntityItemNumber >= COOP_RADAR_MAX_BLIPS )return;

	//hzm coop mod chrissstrahl - disable all other inactive blips
	for ( i = iMiObjEntityItemNumber; i < COOP_RADAR_MAX_BLIPS; i++ ){
		if ( player->coopPlayer.radarBlipActive[i] == true ){
			player->coopPlayer.radarBlipActive[i] = false;
			//we need it to be reliable
			DelayedServerCommand( player->entnum , va( "globalwidgetcommand cr%i disable" , i ) );
		}
	}
}

