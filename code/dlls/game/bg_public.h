//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/bg_public.h                                   $
// $Revision:: 112                                                            $
//   $Author:: Steven                                                         $
//     $Date:: 5/10/03 2:02p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Definitions shared by both the server game and client game modules

#ifndef __BG_PUBLIC_H__
#define __BG_PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

// because games can change separately from the main system version, we need a
// second version that must match between game and cgame
#define	GAME_VERSION		"EF2-base-1"

#define DEFAULT_GRAVITY 800

// CS_SERVERINFO and CS_SYSTEMINFO and CS_NAME are defined in q_shared.h
#define	CS_SOUNDTRACK			8
#define	CS_FOGINFO  			9
#define	CS_SKYINFO  			10
#define	CS_GAME_VERSION			11
#define	CS_LEVEL_START_TIME		12		// so the timer only shows the current level
#define	CS_TERRAININFO  		13
#define	CS_ENTITYFADEINFO		14
#define	CS_DEBUGINFO			15
#define	CS_MODELS				32
#define	CS_SOUNDS				(CS_MODELS+MAX_MODELS)
#define	CS_IMAGES				(CS_SOUNDS+MAX_SOUNDS)
#define	CS_LIGHTSTYLES			(CS_IMAGES+MAX_IMAGES)
#define	CS_PLAYERS				(CS_LIGHTSTYLES+MAX_LIGHTSTYLES)
#define	CS_ITEMS				(CS_PLAYERS+MAX_CLIENTS) // strings for item names
#define	CS_AMMO					(CS_ITEMS+MAX_ITEMS)     // strings for ammo names
#define	CS_LOCATIONS			(CS_AMMO+MAX_AMMO)
#define	CS_NUM_ARENAS			(CS_LOCATIONS+MAX_LOCATIONS)
#define	CS_ARENA_INFO			(CS_NUM_ARENAS+1)
#define	CS_NUM_TEAMS			(CS_ARENA_INFO+MAX_ARENA_INFO)
#define	CS_TEAM_INFO			(CS_NUM_TEAMS+1)
#define	CS_ARCHETYPE			(CS_TEAM_INFO + MAX_TEAM_INFO)
#define	CS_FAILEDREASON			(CS_ARCHETYPE + MAX_ARCHETYPES)
#define CS_OBJECTIVE_NAME		(CS_FAILEDREASON+1)
#define	CS_BOTINFO				(CS_OBJECTIVE_NAME + MAX_OBJECTIVE_NAMES) // used for bot code
#define CS_GENERAL_STRINGS		(CS_BOTINFO + 2)
#define	CS_MAX					(CS_GENERAL_STRINGS + MAX_GENERAL_STRINGS) // 2 because that's what q3ta defined, haven't looked why yet

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

// BOTLIB needed for bot code
typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;


//
// scale to use when evaluating constantLight scale
//
#define CONSTANTLIGHT_RADIUS_SCALE 8

/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/

//hzm stvef compatibility chrissstrahl - try if you can change these without to much troubble,
//add new ones for the stvef scaling, note them down in a seperate file, try to get this working
//dynamically at the actual functions that check the values

#define	MAX_CLIP_PLANES	   5
#define	MIN_WALK_NORMAL	   0.65f		   // can't walk on very steep slopes

#define	STEPSIZE		         33.0f

#define  MINS_X               -22
#define  MINS_Y               -22
#define  MAXS_X               22
#define  MAXS_Y               22

#define	MINS_Z				   0
#define  MAXS_Z               96

//hzm coop mod chrissstrahl - test- we want to test if dead players can be used whilke standing
//#define  DEAD_MINS_Z          32
#define  DEAD_MINS_Z          64

#define  CROUCH_MAXS_Z        49
//#define	DEFAULT_VIEWHEIGHT	85
#define  CROUCH_VIEWHEIGHT	   ( CROUCH_MAXS_Z - 6 )
#define	DEAD_VIEWHEIGHT		( MAXS_Z - 6 )

#define  WATER_TURBO_SPEED    1.35f
#define  WATER_TURBO_TIME     1200
#define  MINIMUM_RUNNING_TIME 800
#define  MINIMUM_WATER_FOR_TURBO  90

#define	OVERCLIP		         1.001f

typedef enum {
	PM_NORMAL,		     // normal movement mode
   PM_NOCLIP,		     // noclip movement
	PM_DEAD,		        // no acceleration or turning, but free falling
	PM_SIDESTRAFE,      // Player can only strafe side to side
	PM_FORWARDSTRAFE,    // Player can only move forward and back, but the strafe keys will be overloaded to do so
	PM_SPECTATOR,
	PM_SPECTATOR_FOLLOW,
	PM_SECRET_MOVE_MODE,
	PM_3RD_PERSON,
	PM_NONE
} pmtype_t;

// entityState_t->event values
// entity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.
typedef enum {
   EV_NONE,

   EV_FALL_VERY_SHORT,
   EV_FALL_SHORT,
   EV_FALL_MEDIUM,
   EV_FALL_FAR,
   EV_FALL_VERY_FAR,
   EV_FALL_FATAL,
   EV_TERMINAL_VELOCITY,

   EV_WATER_TOUCH,   // foot touches
   EV_WATER_LEAVE,   // foot leaves
   EV_WATER_UNDER,   // head touches
   EV_WATER_CLEAR,   // head leaves

   EV_LAST_PREDICTED      // just a marker point

   // events generated by non-players or never predicted
} entity_event_t;

// pmove->pm_flags
#define	PMF_DUCKED			   ( 1<<0 )    // player is ducked
#define	PMF_TIME_JUMP_START	   ( 1<<1 )
#define	PMF_TIME_KNOCKBACK	( 1<<2 )		// pm_time is an air-accelerate only time
#define	PMF_TIME_WATERJUMP	( 1<<3 )		// pm_time is waterjump
#define  PMF_TIME_TELEPORT    ( 1<<4 )    // pm_time is teleport
#define  PMF_NO_PREDICTION    ( 1<<5 )    // no prediction
#define  PMF_FROZEN           ( 1<<6 )    // player cannot move or look around
#define  PMF_INTERMISSION     ( 1<<7 )    // intermission view

//
// the following flag is required by the server and cannot be changed
//
#define  PMF_CAMERA_VIEW							( 1<<8	)   // use camera view instead of ps view
#define PMF_NO_MOVE									( 1<<9  )   // player cannot move but can still look around
#define PMF_HAVETARGET								( 1<<10 )   // player has a target in the crosshairs
#define	PMF_TIME_STUCKJUMP							( 1<<11 )	// pm_time is stuckjump
#define PMF_LEVELEXIT								( 1<<12 )   // player is near an exit
#define PMF_NO_GRAVITY								( 1<<13 )   // do not apply gravity to the player
#define	PMF_JUMP_HELD								( 1<<14 )	// Jump held
#define	PMF_FLIGHT									( 1<<15 )	// Flight Powerup
#define PMF_ENEMY_TARGETED							( 1<<16 )	// An enemy is targeted.
#define PMF_RADAR_MODE								( 1<<17 )	// the mode of the radar ( simple: 0, detailed : 1)
#define PMF_DISABLE_INVENTORY						( 1<<18 )	// disables the player's inventory
#define PMF_SCANNER									( 1<<22 )	// has a scanner
#define PMF_ZOOM									( 1<<23 )	// zoom is active
#define	PMF_LEAN_LEFT								( 1<<24 )	// player is leaning left
#define PMF_LEAN_RIGHT								( 1<<25 )	// player is leaning right
#define PMF_TIME_CROUCH_JUMP						( 1<<26 )	// crouch jump
#define PMF_NIGHTVISION								( 1<<27 )	// night vision
#define PMF_DAMAGE_FRONT							( 1<<28 )
#define PMF_DAMAGE_BACK								( 1<<29 )
#define PMF_DAMAGE_LEFT								( 1<<30 )
#define PMF_DAMAGE_RIGHT							( 1<<31 )

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_JUMP_START|PMF_TIME_KNOCKBACK|PMF_TIME_TELEPORT|PMF_TIME_STUCKJUMP|PMF_TIME_CROUCH_JUMP)

#define	MAXTOUCH	32

#define  MOVERESULT_NONE     0            // nothing blocking
#define  MOVERESULT_TURNED   1            // move blocked, but player turned to avoid it
#define  MOVERESULT_BLOCKED  2            // move blocked by slope or wall
#define  MOVERESULT_HITWALL  3            // player ran into wall

typedef struct {
   // state (in / out)
	playerState_t	*ps;

	// command (in)
	usercmd_t	cmd;
	int			tracemask;			// collide against these types of surfaces
	int			debugLevel;			// if set, diagnostic output will be printed
	qboolean	   noFootsteps;		// if the game is setup for no footsteps by the server

	// results (out)
	int			numtouch;
	int			touchents[MAXTOUCH];

   int         moveresult;       // indicates whether 2the player's movement was blocked and how

	qboolean		landed;
	float			landedVelocity;

	qboolean	   stepped;		      // made a non-smooth step that can be
									      // smoothed on the client side

	int			pmoveEvent;			// events predicted on client side
	vec3_t		mins, maxs;			// bounding box size
	int			watertype;
	int			waterlevel;

	// callbacks to test the world
	// these will be different functions during game and cgame
	void        (*trace)( trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask, qboolean cylinder );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );
	qboolean		(*trypush)( int entnum, vec3_t move_origin, vec3_t move_end );
} pmove_t;

// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd );
void Pmove_GroundTrace( pmove_t *pmove );
void Pmove (pmove_t *pmove);

//===================================================================================

// content masks
#define	MASK_ALL							(-1)
#define	MASK_SOLID							(CONTENTS_SOLID)
#define	MASK_USABLE							(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_USABLE)
#define	MASK_PLAYERSOLID					(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY|CONTENTS_SETCLIP)
#define	MASK_DEADSOLID						(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_SETCLIP)
#define	MASK_MONSTERSOLID					(CONTENTS_SOLID|CONTENTS_MONSTERCLIP|CONTENTS_BODY|CONTENTS_SETCLIP)
#define	MASK_WATER							(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE							(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_CAMERACLIP)
#define	MASK_SHOT							(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_WEAPONCLIP|CONTENTS_SHOOTABLE_ONLY|CONTENTS_SETCLIP)
#define	MASK_PROJECTILE						(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_WEAPONCLIP|CONTENTS_SHOOTABLE_ONLY)
#define	MASK_PROJECTILE_NOTSHOOTABLE		(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_WEAPONCLIP)
#define	MASK_MELEE     						(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE|CONTENTS_WEAPONCLIP|CONTENTS_SHOOTABLE_ONLY)
#define	MASK_PATHSOLID						(CONTENTS_SOLID|CONTENTS_MONSTERCLIP|CONTENTS_SETCLIP)
#define	MASK_CAMERASOLID					(CONTENTS_SOLID|CONTENTS_CAMERACLIP|CONTENTS_BODY|CONTENTS_SETCLIP|MASK_WATER)
#define MASK_SETCLIP						(CONTENTS_BODY)
#define MASK_CURRENT						(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
typedef enum {
	PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
   PERS_TEAM
} persEnum_t;

// entityState_t->eFlags
#define	EF_EVENT_BIT1				0x00000001		// toggled every time an event changes
#define	EF_EVENT_BIT2				0x00000002		// toggled every time an event changes
#define	EF_EVENT_BITS				(EF_EVENT_BIT1|EF_EVENT_BIT2)
#define	EF_TELEPORT_BIT				(1<<2)   		// toggled every time the origin abruptly changes
#define EF_EVERYFRAME  				(1<<3)    		// def commands will be run every client frame
#define EF_FRIEND					(1<<4)			// the entity is a friend	
#define EF_ENEMY					(1<<5)			// the entity is a enemy
#define EF_DONT_PROCESS_COMMANDS	(1<<6)			// don't process client commands for this entity
#define EF_EFFECT_CUSTOM			(1<<7)			// Custom effect, look at the entitystate for the shader to use
#define EF_EMITTER_CUSTOM			(1<<8)			// Custom emitter, look at the entitystate for the emitter name to use

#define EF_DISPLAY_INFO				(1<<9)
#define	EF_DISPLAY_DESC1			(1<<10)			// Display description 1
#define	EF_DISPLAY_DESC2			(1<<11)			// Display description 2
#define	EF_DISPLAY_DESC3			(1<<12)			// Display description 3

#define EF_EFFECT_ELECTRIC			(1<<14)			// Transport effect
#define EF_BEHAVIOR_FAILURE			(1<<15)			// Behavior Failure Effect

#define EF_EFFECTS ( EF_EFFECT_CUSTOM | EF_EFFECT_ELECTRIC | EF_BEHAVIOR_FAILURE )

// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT		(1<<9)
#define	ANIM_BLEND        (1<<10)
#define  ANIM_NUMBITS      11

// server side anim bits
#define  ANIM_SERVER_EXITCOMMANDS_PROCESSED (1<<12)

#define  ANIM_MASK         ( ~( ANIM_TOGGLEBIT | ANIM_BLEND | ANIM_SERVER_EXITCOMMANDS_PROCESSED ) )

// if FRAME_EXPLICIT is set, don't auto animate
#define FRAME_NUMBITS		14
#define	FRAME_EXPLICIT		( 1 << 13 )
#define FRAME_MASK			( ~FRAME_EXPLICIT )

//
// Tag specific flags
//
#define TAG_NUMBITS        10                    // number of bits required to send over network
#define TAG_MASK           ( ( 1 << 10 ) - 1 )


//
// Camera Flags
//
#define CF_CAMERA_ANGLES_ABSOLUTE      ( 1 << 0 )
#define CF_CAMERA_ANGLES_IGNORE_PITCH  ( 1 << 1 )
#define CF_CAMERA_ANGLES_IGNORE_YAW    ( 1 << 2 )
#define CF_CAMERA_ANGLES_ALLOWOFFSET   ( 1 << 3 )
#define CF_CAMERA_CUT_BIT              ( 1 << 7 ) // this bit gets toggled everytime we do a hard camera cut

typedef enum {
	MOD_NONE,
	MOD_DROWN,
	MOD_SUICIDE,
	MOD_CRUSH,
	MOD_CRUSH_EVERY_FRAME,
	MOD_TELEFRAG,
	MOD_LAVA,
	MOD_SLIME,
	MOD_FALLING,
	MOD_LAST_SELF_INFLICTED,
	MOD_EXPLOSION,
	MOD_EXPLODEWALL,
	MOD_ELECTRIC,
	MOD_ELECTRICWATER,
	MOD_THROWNOBJECT,
	MOD_BEAM,
	MOD_ROCKET,
	MOD_IMPACT,
	MOD_GAS,
	MOD_GAS_BLOCKABLE,
	MOD_ACID,
	MOD_SWORD,
	MOD_PLASMA,
	MOD_PLASMABEAM,
	MOD_PLASMASHOTGUN,
	MOD_STING,
	MOD_STING2,
	MOD_SLING,
	MOD_BULLET,
	MOD_FAST_BULLET,
	MOD_VEHICLE,
	MOD_FIRE,
	MOD_FIRE_BLOCKABLE,
	MOD_VORTEX,
	MOD_LIFEDRAIN,
	MOD_FLASHBANG,
	MOD_POO_EXPLOSION,
	MOD_AXE,
	MOD_CHAINSWORD,
	MOD_ON_FIRE,
	MOD_FIRESWORD,
	MOD_ELECTRICSWORD,
	MOD_CIRCLEOFPROTECTION,
	MOD_RADIATION,
	MOD_LIGHTSWORD,
	MOD_GIB,
	MOD_IMPALE,
	MOD_UPPERCUT,
	MOD_POISON,
	MOD_EAT,
	MOD_REDEMPTION,
	MOD_STASIS,

	// Added for EF
	MOD_PHASER,
	MOD_VAPORIZE,
	MOD_COMP_RIFLE,
	MOD_VAPORIZE_COMP,
	MOD_IMOD_PRIMARY,
	MOD_IMOD_SECONDARY,	
	MOD_SMALL_EXPLOSION,
	MOD_TETRYON,
	MOD_DISRUPTOR,
	MOD_VAPORIZE_DISRUPTOR,
	MOD_VAPORIZE_PHOTON,
	MOD_SNIPER,
	MOD_MELEE,
	MOD_ALIEN_MELEE,
	MOD_KLINGON_MELEE,
	MOD_TURRET,

	// Powerups/runes

	MOD_DEATH_QUAD,
	MOD_EMPATHY_SHIELD,
	MOD_ARMOR_PIERCING,

	MOD_TOTAL_NUMBER
	} meansOfDeath_t;

// If you add to the enum above go add a string to the means_of_death_strings in g_utils.cpp
extern char means_of_death_strings[ MOD_TOTAL_NUMBER ][ 32 ];


// Context Dialog
// To add a new context, you must add the context in 4 places ( Yes, I know ).  You must add
// an enumeration to the dialogContexts_t and a corresponding string in context_strings ( inside g_utils.cpp )
// That will "create" the context ( though you still need to implement the usage ).  To make the context useful
// you will need to add a SOUNDTYPE for it as well -- Which means adding a string to soundtype_strings
// ( also in g_utils.cpp ).  
//
// In short, to add a context put it in the following places
// 1:  dialogContexts_t     ( enumerated ID           )
// 2:  context_strings      ( string form of ID       )
// 3:  soundType            ( soundType for context   )
// 4   soundtype_strings    ( string of the soundType )
//
typedef enum
   {
   CONTEXT_SPOTTED_ENEMY,
   CONTEXT_INJURED,
   CONTEXT_IN_COMBAT,
   CONTEXT_WEAPON_USELESS,
   CONTEXT_INVESTIGATING,

   CONTEXT_TOTAL_NUMBER
   } dialogContexts_t;

typedef enum 
	{
   // General Sound Types
   SOUNDTYPE_NONE,
	SOUNDTYPE_GENERAL,
	SOUNDTYPE_EXPLOSION,
	SOUNDTYPE_WEAPONFIRE,
	SOUNDTYPE_ALERT,
	SOUNDTYPE_FOOTSTEPS_WALK,
	SOUNDTYPE_FOOTSTEPS_RUN,
	SOUNDTYPE_FALL,

   // Context Dialog Sound Types
   SOUNDTYPE_DIALOG_CONTEXT_SPOTTED_ENEMY,
   SOUNDTYPE_DIALOG_CONTEXT_INJURED,
   SOUNDTYPE_DIALOG_CONTEXT_IN_COMBAT,
   SOUNDTYPE_DIALOG_CONTEXT_WEAPON_USELESS,
   SOUNDTYPE_DIALOG_CONTEXT_INVESTIGATING,

	SOUNDTYPE_TOTAL_NUMBER
	} soundType;

// If you add to the enum above go add a string to the soundtype_strings in g_utils.cpp
extern char soundtype_strings[ SOUNDTYPE_TOTAL_NUMBER ][ 32 ];
extern char context_strings[CONTEXT_TOTAL_NUMBER][32];

//---------------------------------------------------------


// mp_flags->integer flags

#define MP_FLAG_NO_HEALTH				(1<<0)
#define MP_FLAG_NO_POWERUPS   			(1<<1)
#define MP_FLAG_WEAPONS_STAY			(1<<2)
#define MP_FLAG_NO_FALLING				(1<<3)
#define MP_FLAG_INSTANT_ITEMS			(1<<4)
#define MP_FLAG_SAME_LEVEL				(1<<5)
#define MP_FLAG_SKINTEAMS				(1<<6)
#define MP_FLAG_MODELTEAMS				(1<<7)
#define MP_FLAG_FRIENDLY_FIRE			(1<<8)
#define MP_FLAG_SPAWN_FARTHEST			(1<<9)
#define MP_FLAG_FORCE_RESPAWN			(1<<10)
#define MP_FLAG_NO_ARMOR				(1<<11)
#define MP_FLAG_FAST_WEAPONS			(1<<12)
#define MP_FLAG_NOEXIT					(1<<13)
#define MP_FLAG_INFINITE_AMMO			(1<<14)
#define MP_FLAG_FIXED_FOV				(1<<15)
#define MP_FLAG_NO_DROP_WEAPONS			(1<<16)
#define MP_FLAG_NO_FOOTSTEPS			(1<<17)
#define MP_FLAG_DONT_ALLOW_VOTE			(1<<18)
#define MP_FLAG_FULL_COLLISION			(1<<19)
#define MP_FLAG_NO_AUTO_JOIN_TEAM		(1<<20)
#define MP_FLAG_AUTO_BALANCE_TEAMS		(1<<21)
#define MP_FLAG_FORCE_DEFAULT_MODEL		(1<<22)

// teamflags->integer flags
#define TF_TEAMPLAY				1
#define TF_NO_FRIENDLY_FIRE	2

//
// entityState_t->eType
//
typedef enum {
   ET_MODELANIM,
	ET_PLAYER,
	ET_ITEM,
	ET_GENERAL,
	ET_MISSILE,
	ET_MOVER,
	ET_BEAM,
   ET_MULTIBEAM,
   ET_SPRITE,
	ET_PORTAL,
	ET_EVENT_ONLY,
   ET_RAIN,
   ET_LEAF,
	ET_SPEAKER,
	ET_PUSH_TRIGGER,
	ET_TELEPORT_TRIGGER,
   ET_DECAL,
   ET_EMITTER,
   ET_ROPE,
   ET_EVENTS
} entityType_t;

void	EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void	EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );

#define MAX_LETTERBOX_SIZE        0x7fff

#define ITEM_NAME_AMMO_LEFT       0
#define ITEM_NAME_AMMO_RIGHT      1
#define ITEM_NAME_WEAPON_LEFT     2
#define ITEM_NAME_WEAPON_RIGHT    3
#define ITEM_NAME_WEAPON_DUAL     4

// added for bot code BOTLIB
#define ARENAS_PER_TIER		4
#define MAX_ARENAS			1024
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			1024
#define MAX_BOTS_TEXT		8192

typedef enum {
	GTS_RED_CAPTURE,
	GTS_BLUE_CAPTURE,
	GTS_RED_RETURN,
	GTS_BLUE_RETURN,
	GTS_RED_TAKEN,
	GTS_BLUE_TAKEN,
	GTS_REDOBELISK_ATTACKED,
	GTS_BLUEOBELISK_ATTACKED,
	GTS_REDTEAM_SCORED,
	GTS_BLUETEAM_SCORED,
	GTS_REDTEAM_TOOK_LEAD,
	GTS_BLUETEAM_TOOK_LEAD,
	GTS_TEAMS_ARE_TIED,
	GTS_KAMIKAZE
} global_team_sound_t;

//team task
typedef enum {
	TEAMTASK_NONE,
	TEAMTASK_OFFENSE, 
	TEAMTASK_DEFENSE,
	TEAMTASK_PATROL,
	TEAMTASK_FOLLOW,
	TEAMTASK_RETRIEVE,
	TEAMTASK_ESCORT,
	TEAMTASK_CAMP
} teamtask_t;

// end bot additions

typedef enum  { 
	UNKNOWN,
	OBJECT_LOCATION, 
	OBJECT_NONINTERACTIVE,
	OBJECT_USABLE,
	OBJECT_DESTRUCTABLE,
	ACTOR_FRIENDLY,
	ACTOR_ENEMY,
	ACTOR_TEAMMATE 
} EntityType;

typedef enum 
{ 
	HEALTHY_STATUS = 0,
	INJURED_STATUS,
	CRITICAL_STATUS 
} TeammateStatus;


#define MAX_GAMEPLAY_STRING_LENGTH 256

#ifdef __cplusplus
   }
#endif

#endif // __BG_PUBLIC_H__
