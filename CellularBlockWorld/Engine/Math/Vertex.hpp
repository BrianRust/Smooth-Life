#ifndef included_Vertex
#define included_Vertex
#pragma once

//-------------------------------------------------------------
#include "MyMathFiles.hpp"

//-------------------------------------------------------------

/*
	x y z r g b a u v
*/

//-------------------------------------------------------------
class Vertex
{
public:
	Vertex() {
		//vertexPosition = Vector3(0.f, 0.f, 0.f);
		side = 0;
		color = 0.f;
	};

	Vertex( const Vector3& InitialPosition, unsigned char Side = 0, float Color = 0.f ) {
		//vertexPosition = InitialPosition;
		SetPosBytesFromFloats( InitialPosition.x, InitialPosition.y, InitialPosition.z);
		color = Color;
		side = Side;
	}

	Vertex( unsigned char x, unsigned char y, unsigned char z, unsigned char Side, float Color ) {
		positionBytes[0] = x;
		positionBytes[1] = y;
		positionBytes[2] = z;
		color = Color;
		side = Side;
	}

	void SetPosBytesFromFloats( float x, float y, float z) {
		x = ClampFloat( x, 0.f, 255.f );
		y = ClampFloat( y, 0.f, 255.f );
		z = ClampFloat( z, 0.f, 255.f );

		positionBytes[0] = (unsigned char) x;
		positionBytes[1] = (unsigned char) y;
		positionBytes[2] = (unsigned char) z;
		
		color = 0.f;
		side = 0;
	}

	//Vector3 vertexPosition;
	unsigned char positionBytes[3];  //x, y, z
	unsigned char side;
	float color;
};

#endif //included_Vertex