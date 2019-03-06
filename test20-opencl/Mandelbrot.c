
// angle: <0.0f;1.0f)
// grayscale: <0.0f;1.0f)
uint4 HSVtoRGB( const float angle, float grayscale )
{
	float3 color;
	float a, b;
	
	if( angle < 0.166666f )
	{
		a = angle;
		//b = 0.166666f - angle;
		color.x = 1.0f;
		color.y = a*6.0f;
		color.z = 0.0f;
	}
	else if( angle < 0.333333f )
	{	
		//a = angle - 0.166666f;
		b = 0.333333f - angle;
		color.x = b*6.0f;
		color.y = 1.0f;
		color.z = 0.0f;
	}
	else if( angle < 0.5f )
	{
		a = angle - 0.333333f;
		//b = 0.5f - angle;
		color.x = 0.0f;
		color.y = 1.0f;
		color.z = a*6.0f;
	}
	else if( angle < 0.666666f )
	{
		//a = angle - 0.5f;
		b = 0.666666f - angle;
		color.x = 0.0f;
		color.y = b*6.0f;
		color.z = 1.0f;
	}
	else if( angle < 0.833333f )
	{
		a = angle - 0.666666f;
		//b = 0.833333f - angle;
		color.x = a*6.0f;
		color.y = 0.0f;
		color.z = 1.0f;
	}
	else
	{
		//a = angle - 0.833333f;
		b = 1.0f - angle;
		color.x = 1.0f;
		color.y = 0.0f;
		color.z = b*6.0f;
	}
	
	uint4 ret;
	ret.x = convert_uint( color.x*255.0f );
	ret.y = convert_uint( color.y*255.0f );
	ret.z = convert_uint( color.z*255.0f );
	return ret;
}

float Length( float4 a )
{
	return sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w );
}

float Length2( float4 a )
{
	return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w;
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

float4 GetValue( float4 c, int count )
{
	float module = Length( c );
	float prevmodule = module, currentmodule = module;
	int i;
	float4 n = c;
	float4 ret = n;
	float cl2 = Length2(n);
	for( i = 0; i < count; ++i )
	{
		prevmodule = currentmodule;
		n = QuatMultiply( n, n ) + c;
		currentmodule = Length( n );
		//if( currentmodule > prevmodule + module )
		if( currentmodule > module * 3.0f )
			return n;
		if( Length2(n-c) <= 0.00000000000000001 )
			return n;//ret;
		
		float t = Length2(n);
		if( cl2 > t || i < 10 )
		{
			cl2 = t;
			ret = n;
		}
	}
	return n;//ret;
}

uint4 AbsVal( float4 a )
{
	return HSVtoRGB( exp(-Length(a)) * 0.833f + 0.167f, 1.0f );
}

__kernel void MandelbrotSet( __global uchar4 * img, const int ioffset, const int width, const int height, __global float4 * ox, __global float4 * oy, const int MEMORY_BLOCK_ELEMENTS )
{
    int i = ioffset + get_global_id(0);
    
    float scale = 2.3f;
    
	float2 coord;
	coord.x = i%width;
	coord.y = i/width;
	coord.x /= convert_float(width);
	coord.y /= convert_float(height);
	
	coord.x -= 0.5f;
	coord.y -= 0.5f;
	
	coord.x *= scale;
	coord.y *= scale;
	
	
	
	float4 pos = (ox[0]*coord.x) + (oy[0]*coord.y);
	
	uchar4 value;
	float4 mx = ox[0] * scale * 0.04f / convert_float(width);
	float4 my = oy[0] * scale * 0.04f / convert_float(height);
	
	value = convert_uchar4( AbsVal( GetValue( pos, 17 ) ) );
	value.w = 255;
	
	img[i%MEMORY_BLOCK_ELEMENTS] = value;
}



