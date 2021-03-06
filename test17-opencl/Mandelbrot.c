

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
	ret.w = 255;
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

float GetAngle( float4 q ) // 0.0 - 1.0
{
	float len = Length(q);
	if( len > 0.00001 )
	{
		return (atan2pi( q.y, q.w )+1.0f)*0.5f;
	}
	return 0.0f;
}

float4 GetValue( float4 c, int count )
{
	float module = Length2( c );
	int i;
	float4 n = c;
	float4 ret = n;
	for( i = 0; i < count; ++i )
	{
		if( i < 100 )
			ret = n;
		n = QuatMultiply( n, n ) + c;
		if( Length2(n-c) <= 0.000000000001f )
			return ret; //c; //(float4)(0.0f,0.0f,0.0f,0.0f);
		if( Length2(n) > module * 64.0f )
			return (float4)(0.0f,0.0f,0.0f,0.0f);
	}
	return ret;
}

uint4 AbsVal( float4 a )
{
	if( Length2(a) <= 0.000000001f )
		return (uint4)(0,0,0,255);
	
	return HSVtoRGB( GetAngle(a), 1.0f );
}













__kernel void MandelbrotSet( __global uchar4 * img, const int ioffset, const int width, const int height, __global float4 * ox, __global float4 * oy, const int MEMORY_BLOCK_ELEMENTS )
{
    int i = ioffset + get_global_id(0);
    
    float scale = 0.002f;
    
	float2 coord;
	coord.x = i%width;
	coord.y = i/width;
	coord.x /= convert_float(width);
	coord.y /= convert_float(height);
	
	coord.x -= 0.5f;
	coord.y -= 0.5f;
	
	coord.x *= scale;
	coord.y *= scale;
	
	//coord.x -= 0.65f;
	//coord.y -= 0.17f;
	
	//coord.x += 0.005f * 0.275;
	//coord.y -= 0.005f * 0.25f;
	
	
	coord.x += 2.1f*0.25f;
	coord.y -= 2.1f*0.125f;
	coord.x += 0.3f*0.375f;
	coord.y -= 0.3f*0.1f;
	coord.x += 0.05f*0.17f;
	coord.y -= 0.4f;
	coord.y += 0.31f*0.5f;
	coord.x -= 0.31f*0.07f;
	coord.x -= 0.03f*0.3f;
	coord.y -= 0.017f*0.2f;
	
	
	float4 pos;
	
    pos = (ox[0]*coord.x) + (oy[0]*coord.y);
	
	uchar4 value;
	float4 mx = ox[0] * scale * 0.04f / convert_float(width);
	float4 my = oy[0] * scale * 0.04f / convert_float(height);
	
	uint4 valuetemp;
	const int fractalIterations = 250;
	
	
	//valuetemp = convert_uint4( AbsVal( GetValue( pos, fractalIterations ) ) ) * 53;
	
	
	valuetemp = AbsVal( GetValue( pos, fractalIterations ) ) * 5;
	
	valuetemp += AbsVal( GetValue( pos+mx, fractalIterations ) ) * 3;
	valuetemp += AbsVal( GetValue( pos-mx, fractalIterations ) ) * 3;
	valuetemp += AbsVal( GetValue( pos+my, fractalIterations ) ) * 3;
	valuetemp += AbsVal( GetValue( pos-my, fractalIterations ) ) * 3;
	valuetemp += AbsVal( GetValue( pos+my-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-my-mx-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-my+mx+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+my-mx-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+my+mx+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+my-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+my+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-my-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-my+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my+mx+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my-mx-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-mx-mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos+my+mx+mx, fractalIterations ) ) * 2;
	valuetemp += AbsVal( GetValue( pos-my-my, fractalIterations ) );
	valuetemp += AbsVal( GetValue( pos+my+my, fractalIterations ) );
	valuetemp += AbsVal( GetValue( pos+mx+mx, fractalIterations ) );
	valuetemp += AbsVal( GetValue( pos-mx-mx, fractalIterations ) );
	
	
	/*
	valuetemp = GetValue( pos, fractalIterations ) * 5;
	valuetemp += GetValue( pos+mx, fractalIterations ) * 3;
	valuetemp += GetValue( pos-mx, fractalIterations ) * 3;
	valuetemp += GetValue( pos+my, fractalIterations ) * 3;
	valuetemp += GetValue( pos-my, fractalIterations ) * 3;
	valuetemp += GetValue( pos+my-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-my-mx-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-my+mx+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+my-mx-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+my+mx+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+my-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+my+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-my-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-my+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my+mx+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my-mx-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-mx-mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos+my+mx+mx, fractalIterations ) * 2;
	valuetemp += GetValue( pos-my-my, fractalIterations );
	valuetemp += GetValue( pos+my+my, fractalIterations );
	valuetemp += GetValue( pos+mx+mx, fractalIterations );
	valuetemp += GetValue( pos-mx-mx, fractalIterations );
	*/
	
	value = convert_uchar4( valuetemp/53 );
	value.w = 255;
	
	img[i%MEMORY_BLOCK_ELEMENTS] = value;
}

