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

#ifndef QUAT_CPP
#define QUAT_CPP

#include "../SourceHeader/Quat.h"

#ifndef M_PI
#define M_PI 3.141596f
#endif

inline float& Quat::operator[]( const int id )
{
	switch( id )
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}
	return w;
}

inline Quat & Quat::operator = ( const Quat& src )
{
	x = src.x;
	y = src.y;
	z = src.z;
	return *this;
}

inline Quat Quat::operator * ( const Quat& src ) const
{
	return Quat(
		w*src.x + x*src.w + y*src.z - z*src.y,
		w*src.y + y*src.w + z*src.x - x*src.z,
		w*src.z + z*src.w + x*src.y - y*src.x,
		w*src.w - x*src.x - y*src.y - z*src.z
		).Normalized();
}

inline Quat& Quat::operator *= ( const Quat& src )
{
	Quat dst(
		w*src.x + x*src.w + y*src.z - z*src.y,
		w*src.y + y*src.w + z*src.x - x*src.z,
		w*src.z + z*src.w + x*src.y - y*src.x,
		w*src.w - x*src.x - y*src.y - z*src.z
		);
	memcpy( this, &dst, sizeof(Quat) );
	return Normalize();
}

inline Vector Quat::operator * ( const Vector& src ) const
{
	Vector dst;
	
	float
		xx = x * x, yy = y * y, zz = z * z,
		xy = x * y, xz = x * z,
		yz = y * z, wx = w * x,
		wy = w * y, wz = w * z;
	
	dst.x =
		( 1.0f - 2.0f * ( yy + zz ) ) * src.x +
		( 2.0f * ( xy - wz ) ) * src.y  +
		( 2.0f * ( xz + wy ) ) * src.z;
	
	dst.y =
		( 2.0f * ( xy + wz ) ) * src.x +
		( 1.0f - 2.0f * ( xx + zz )) * src.y +
		( 2.0f * ( yz - wx ) ) * src.z;
	
	dst.z =
		( 2.0f * ( xz - wy ) ) * src.x +
		( 2.0f * ( yz + wx ) ) * src.y +
		( 1.0f - 2.0f * ( xx + yy ) ) * src.z;
	
	return dst;
}

inline Quat Quat::operator - () const
{
	return Quat( -x, -y, -z, w );
}

inline bool Quat::operator == ( const Quat& src ) const
{
	return !memcmp( this, &src, sizeof(Quat) );
}

inline bool Quat::operator != ( const Quat& src ) const
{
	return memcmp( this, &src, sizeof(Quat) );
}

inline bool Quat::operator < ( const Quat& src ) const
{
	return memcmp( this, &src, sizeof(Quat) ) < 0;
}

inline bool Quat::operator > ( const Quat& src ) const
{
	return memcmp( this, &src, sizeof(Quat) ) > 0;
}

inline bool Quat::operator <= ( const Quat& src ) const
{
	return memcmp( this, &src, sizeof(Quat) ) <= 0;
}

inline bool Quat::operator >= ( const Quat& src ) const
{
	return memcmp( this, &src, sizeof(Quat) ) >= 0;
}


inline Vector Quat::GetAxis() const
{
	float val = 1.0f - w * w;
	if( val <= 0.0f )
	{
		return Vector( 0.0f, 0.0f, 0.0f );
	}
	val = 1.0f / sqrtf( val );
	return Vector( x * val, y * val, z * val ).Versor();
}

inline float Quat::GetAngle() const
{
	if( w <= -1.0f )
		return 360.0f;
	if( w >= 1.0f )
		return 0.0f;
	return acos( w ) * 360.0f / M_PI;
}

inline float Quat::Length() const
{
	return sqrtf( x*x + y*y + z*z + w*w );
}

inline Quat Quat::Normalized() const
{
	Quat dst;
	float lenDiv = 1.0f / Length();
	dst.x = x*lenDiv;
	dst.y = y*lenDiv;
	dst.z = z*lenDiv;
	dst.w = w*lenDiv;
	return dst;
}

inline Quat& Quat::Normalize()
{
	float lenDiv = 1.0f / Length();
	x *= lenDiv;
	y *= lenDiv;
	z *= lenDiv;
	w *= lenDiv;
	return *this;
}

inline Quat Quat::Inversed() const
{
	return Quat( -x, -y, -z, w );
}

inline Quat& Quat::Inverse()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

inline bool Quat::IsOpposite( const Quat& src ) const
{
	Vector axisThis = GetAxis(), axisSrc = src.GetAxis();
	float angleThis = GetAngle(), angleSrc = src.GetAngle();
	
	bool angleOpposite = ( angleThis < 0.0f && angleSrc > 0.0f ) || ( angleThis > 0.0f && angleSrc < 0.0f );
	bool axesOpposite = axisThis.Dot( axisSrc ) < 0.0f;
	
	return ( angleOpposite && !axesOpposite ) || ( !angleOpposite && axesOpposite );
}

inline Quat Quat::Slerp( const Quat& dst, const float time ) const
{
	float cosOmega = x*dst.x + y*dst.y + z*dst.z + w*dst.w;
	Quat mid = dst;
	
	if( cosOmega < 0.0f )
	{
		mid.x = -mid.x;
		mid.y = -mid.y;
		mid.z = -mid.z;
		mid.w = -mid.w;
		cosOmega = -cosOmega;
	}
	
	float k0, k1;
	if( cosOmega > 0.9999f )
	{
		k0 = 1.0f - time;
		k1 = time;
	}
	else
	{
		float sinOmega = sqrtf( 1.0f - cosOmega*cosOmega );
		float omega = atan2( sinOmega, cosOmega );
		float oneOverSinOmega = 1.0f / sinOmega;
		k0 = sin( (1.0f - time) * omega ) * oneOverSinOmega;
		k1 = sin( time * omega ) * oneOverSinOmega;
	}
	
	mid.x = x*k0 + mid.x*k1;
	mid.y = y*k0 + mid.y*k1;
	mid.z = z*k0 + mid.z*k1;
	mid.w = w*k0 + mid.w*k1;
	
	return mid;
}

inline void Quat::ClampAngle( const float maxAngle )
{
	float angle = GetAngle();
	if( angle > maxAngle )
		*this = Quat( GetAxis(), maxAngle );
	else if( angle < -maxAngle )
		*this = Quat( GetAxis(), -maxAngle );
}

inline void Quat::SetAngle( const float angle )
{
	*this = Quat( GetAxis(), angle );
}

inline Quat& Quat::FromEuler( const Vector& rotator )
{
	float yaw = rotator.x;			////
	float roll = rotator.y;			////
	float pitch = rotator.z;		////
	
	float cy = cos(yaw * 0.5);
	float sy = sin(yaw * 0.5);
	float cr = cos(roll * 0.5);
	float sr = sin(roll * 0.5);
	float cp = cos(pitch * 0.5);
	float sp = sin(pitch * 0.5);

	w = cy * cr * cp + sy * sr * sp;
	x = cy * sr * cp - sy * cr * sp;
	y = cy * cr * sp + sy * sr * cp;
	z = sy * cr * cp - cy * sr * sp;
	
	return *this;
}

inline Vector Quat::GetEuler() const
{
	Vector dst;
	dst.x = atan2( 2.0f * ( w * x + y * z ), 1.0f - 2.0f * ( x * x + y * y ) );
	
	float temp = 2.0f * ( w * y - z * x );
	if( fabs( temp ) >= 1.0f )
		dst.y = copysign( M_PI / 2.0f, temp );
	else
		dst.y = asin(temp);
	
	dst.z = atan2( 2.0f * ( w * z + x * y ), 1.0f - 2.0f * ( y * y + z * z ) );
	
	return dst * ( 180.0f / M_PI );
}

Quat::Quat( const Quat& src_a, const Quat& src_b )
{
	*this = src_a.Inversed() * src_b;
	Normalize();
}

Quat::Quat( const Vector& axis, const float angle )
{
	Vector axis_ = axis.Versor();
	float ang = angle * 0.5f * M_PI / 180.0f;
	w = cos( ang );
	ang = sin( ang );
	x = axis_[0] * ang;
	y = axis_[1] * ang;
	z = axis_[2] * ang;
	Normalize();
}

Quat::Quat( const Vector& rotator )
{
	FromEuler( rotator );
}

Quat::Quat()
{
	x = y = z = 0.0f;
	w = 1.0f;
}

Quat::Quat( const float a, const float b, const float c, const float d )
{
	x = a;
	y = b;
	z = c;
	w = d;
}

#endif

