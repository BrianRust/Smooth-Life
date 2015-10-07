#pragma once
#include "stdlib.h"
#include "Vector2.hpp"

//-----------------------------------------------------------------------------------------------
const float fPi = 3.1415926535897932384626433832795f;

//-----------------------------------------------------------------------------------------------
float Interpolate( float start, float end, float fractionTowardEnd );
float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float CalcFractionInRange( float inValue, float rangeStart, float rangeEnd );
float ClampFloat( float inValue, float min, float max );
float ClampFloatZeroToOne( float inValue );
float ClampFloatNegativeOneToOne( float inValue );
int ClampInt( int inValue, int min, int max );
int RoundToNearestInt( float inValue );

float DegreesToRadians( float degrees );
float RadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( const Vector2& direction );

float CalcDistanceSquared( const Vector2& pos1, const Vector2& pos2 );
float CalcDistance( const Vector2& pos1, const Vector2& pos2 );
bool DoDiscsOverlap( const Vector2& center1, float radius1, const Vector2& center2, float radius2 );
bool IsPointInsideDisc( const Vector2& point, const Vector2& discCenter, float discRadius );

int GetRandomIntInRange( int min, int max );
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange( float min, float max );
bool CheckRandomChance( float chanceForSuccess );

//----------------------------------------------------

//-----------------------------------------------------------------------------------------------
inline float Interpolate( float start, float end, float fractionTowardEnd )
{
	return start + (fractionTowardEnd * (end - start));
}


//-----------------------------------------------------------------------------------------------
// RangeMapFloat( celsius, 0, 100, 32, 212 )
inline float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd ) 
{
	if( inEnd == inStart )
		return (outStart + outEnd) * 0.5f;

	float value = inValue - inStart;
	value /= (inEnd - inStart);
	value *= (outEnd - outStart);
	value += outStart;
	return value;
}


//-----------------------------------------------------------------------------------------------
inline float CalcFractionInRange( float inValue, float rangeStart, float rangeEnd )
{
	if( rangeStart == rangeEnd )
		return 0.5f;

	return (inValue - rangeStart) / (rangeEnd - rangeStart);
}


//-----------------------------------------------------------------------------------------------
inline float ClampFloat( float inValue, float min, float max )
{
	if( inValue < min )
		return min;
	else if( inValue > max )
		return max;
	else
		return inValue;
}


//-----------------------------------------------------------------------------------------------
inline float ClampFloatZeroToOne( float inValue )
{
	return ClampFloat( inValue, 0.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
inline float ClampFloatNegativeOneToOne( float inValue )
{
	return ClampFloat( inValue, -1.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
inline int ClampInt( int inValue, int min, int max )
{
	if( inValue < min )
		return min;
	if( inValue > max )
		return max;

	return inValue;
}


//-----------------------------------------------------------------------------------------------
inline int RoundToNearestInt( float inValue )
{
	return (int) floor( inValue + 0.5f );
}


//-----------------------------------------------------------------------------------------------
inline float DegreesToRadians( float degrees )
{
	return degrees * (fPi / 180.f);
}


//-----------------------------------------------------------------------------------------------
inline float RadiansToDegrees( float radians )
{
	return radians * (180.f / fPi);
}


//-----------------------------------------------------------------------------------------------
inline float CosDegrees( float degrees )
{
	return cos( DegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline float SinDegrees( float degrees )
{
	return sin( DegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline float Atan2Degrees( const Vector2& direction )
{
	return RadiansToDegrees( atan2( direction.y, direction.x ) );
}


//-----------------------------------------------------------------------------------------------
inline float CalcDistanceSquared( const Vector2& pos1, const Vector2& pos2 )
{
	float dx = pos2.x - pos1.x;
	float dy = pos2.y - pos1.y;
	return (dx * dx) + (dy * dy);
}


//-----------------------------------------------------------------------------------------------
inline float CalcDistance( const Vector2& pos1, const Vector2& pos2 )
{
	return sqrt( CalcDistanceSquared( pos1, pos2 ) );
}


//-----------------------------------------------------------------------------------------------
inline bool DoDiscsOverlap( const Vector2& center1, float radius1, const Vector2& center2, float radius2 )
{
	float sumOfRadii = radius1 + radius2;
	float distanceSquared = CalcDistanceSquared( center1, center2 );
	return distanceSquared < (sumOfRadii * sumOfRadii);
}


//-----------------------------------------------------------------------------------------------
inline bool IsPointInsideDisc( const Vector2& point, const Vector2& discCenter, float discRadius )
{
	float distanceSquared = CalcDistanceSquared( point, discCenter );
	return distanceSquared < (discRadius * discRadius);
}


//-----------------------------------------------------------------------------------------------
inline int GetRandomIntInRange( int min, int max )
{
	return min + rand() % (1 + max - min);
}


//-----------------------------------------------------------------------------------------------
inline float GetRandomFloatZeroToOne()
{
	const float oneOverRandMax = (1.f / static_cast< float >( RAND_MAX ) );
	return oneOverRandMax * static_cast< float >( rand() );
}


//-----------------------------------------------------------------------------------------------
inline float GetRandomFloatInRange( float min, float max )
{
	const float zeroToOne = GetRandomFloatZeroToOne();
	return min + (zeroToOne * (max - min));
}


//-----------------------------------------------------------------------------------------------
inline bool CheckRandomChance( float chanceForSuccess )
{
	if( chanceForSuccess == 0.f )
		return false;

	return( GetRandomFloatZeroToOne() <= chanceForSuccess );
}
