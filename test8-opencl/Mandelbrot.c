
float Length( float4 a )
{
	return sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w );
}

float4 QuatMultiply( float4 q1, float4 q2 )
{
	float4 ret;
	ret.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	ret.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z;
	ret.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
	ret.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	return ret;
}

float GetAngle( float4 q ) // 0.0 - 1.0
{
	float len = Length(q);
	if( len > 0.00001 )
	{
		float w = q.w / len;
		if( w <= -1.0f )
			return 1.0f;
		if( w >= 1.0f )
			return 0.0f;
		return acospi( w );
	}
	return 0.0f;
}

float4 GetValue( float4 c, int count )
{
	int i;
	float4 n = c;
	for( ; count > 0; --count )
	{
		float4 a = n;
		for( i = 0; i < 1; ++i )
			n = QuatMultiply( n, a );
		n += c;
	}
	return n;
}

uchar4 AbsVal( float4 a )
{
	uchar4 retval;
	float v;
	
	v = a.x * 200.0f;
	if( v < 0 )
		v *= -1.0f;
	
	if( v < 0.0f )
		retval.x = 0;
	else if( v > 255.0f )
		retval.x = 255;
	else
		retval.x = convert_uchar_sat(v);
	
	v = a.y * 200.0f;
	if( v < 0 )
		v *= -1.0f;
	
	if( v < 0.0f )
		retval.y = 0;
	else if( v > 255.0f )
		retval.y = 255;
	else
		retval.y = convert_uchar_sat(v);
	
	v = GetAngle(a) * 255.0f;
	if( v < 0.0f )
		retval.z = 0;
	else if( v > 255.0f )
		retval.z = 255;
	else
		retval.z = convert_uchar_sat(v);
	
	retval.w = 255;
	return retval;
}

__kernel void MandelbrotSet( __global uchar4 * img1, __global uchar4 * img2, __global uchar4 * img3, __global uchar4 * img4, const int ioffset, const int width, const int height, __global float4 * ox, __global float4 * oy )
{
    int i = ioffset + get_global_id(0);
    
    float scale = 0.0005f;
    
	float2 coord;
	coord.x = i%width;
	coord.y = i/width;
	coord.x /= convert_float(width);
	coord.y /= convert_float(height);
	
	coord.x -= 0.5f;
	coord.y -= 0.5f;
	
	coord.x *= scale;
	coord.y *= scale;
	
	coord.x -= 0.65f;
	coord.y -= 0.17f;
	
	coord.x -= scale * 0.15f;
	coord.y -= scale * 0.05f;
	
	float4 pos;
	
    pos = (ox[0]*coord.x) + (oy[0]*coord.y);
	
	uchar4 value;
	value = AbsVal( GetValue( pos, 999 ) );
	
	if( i < width*height/4 )
		img1[i] = value;
	else if( i < width*height/2 )
		img2[i-(width*height/4)] = value;
	else if( i < width*height*3/4 )
		img3[i-(width*height/2)] = value;
	else if( i < width*height )
		img4[i-(width*height*3/4)] = value;
}

