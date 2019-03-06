/*
	Copyright (C) 2017 Marek Zalewski
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.
	2.	Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source distribution.
*/

#ifndef QUAT_H
#define QUAT_H

#include "cmath"

#include "Vector.h"

class Quat
{
private:
	
	float x, y, z, w;
	
public:
	
	inline float& operator[]( const int id );
	
	inline Quat & operator = ( const Quat& src );
	inline Quat operator * ( const Quat& src ) const;
	inline Quat& operator *= ( const Quat& src );
	inline Vector operator * ( const Vector& src ) const;
	inline Quat operator - () const;
	
	inline bool operator == ( const Quat& src ) const;
	inline bool operator != ( const Quat& src ) const;
	inline bool operator < ( const Quat& src ) const;
	inline bool operator > ( const Quat& src ) const;
	inline bool operator <= ( const Quat& src ) const;
	inline bool operator >= ( const Quat& src ) const;
	
	inline Vector GetAxis() const;
	inline float GetAngle() const;
	inline float Length() const;
	inline Quat Normalized() const;
	inline Quat& Normalize();
	
	inline Quat Inversed() const;
	inline Quat& Inverse();
	inline bool IsOpposite( const Quat& src ) const;
	
	inline Quat Slerp( const Quat& dst, const float time ) const;
	inline void ClampAngle( const float maxAngle );
	inline void SetAngle( const float angle );
	
	inline Vector GetEuler() const;
	inline Quat& FromEuler( const Vector& rotator );
	
	Quat( const Quat& src_a, const Quat& src_b );
	Quat( const Vector& axis, const float angle );
	Quat( const Vector& rotator );		// from euler
	Quat();
	Quat( const float a, const float b, const float c, const float d );
};

#endif

