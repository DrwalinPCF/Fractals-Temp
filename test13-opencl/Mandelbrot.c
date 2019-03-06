
/*
 * H(Hue): 0 - 2 degree (float)
 * S(Saturation): 0 - 1.00 (float)
 * V(Value): 0 - 1.00 (float)
 * 
 * output[3]: Output, array size 3, int
*/
/*
fmod( float a, float b )
{
	
	
	
	
}

float abs( float v )
{
	if( v < 0 )
		return -v;
	return v;
}

uint4 HSVtoRGB( float3 HSV )
{
	float C = HSV.y * HSV.z;
	float X = C * (1.0f - abs(fmod(HSV.x * 30.0f, 2.0f) - 1.0f));
	float m = HSV.z - C;
	float Rs, Gs, Bs;

	if( HSV.x < 0.333333f )
	{
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if( HSV.x < 0.666666f )
	{	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if( HSV.x < 1.0f )
	{
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if( HSV.x < 1.333333f )
	{
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if( HSV.x < 1.666666f )
	{
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else
	{
		Rs = C;
		Gs = 0;
		Bs = X;	
	}
	
	//return (uint4)( (Rs + m) * 255, (Gs + m) * 255, (Bs + m) * 255, 255 );
	uint4 ttt;
	return ttt;
}
*/


/*
int MandelbrotSetPrimitive( float2 point, int maximum )
{
	int counter = 0;
	float zReal = point.x;
	float zImag = point.y;
	float nextRe;
	
	while( zReal*zReal + zImag*zImag <= 4.0 && counter <= maximum )
	{
		nextRe = zReal*zReal - zImag*zImag + point.y;
		zImag = 2.0 * zReal * zImag + point.y;
		zReal = nextRe;
		if( zReal == startReal && zImag == startImag )
		{ // a repetition indicates that the point is in the Mandelbrot set
			return -1; // points in the Mandelbrot set are represented by a return value of -1
		}
		counter += 1;
	}
	
	if( counter >= maximum )
		return -1; // -1 is used here to indicate that the point lies within the Mandelbrot set
	return counter; // returning the number of iterations allows for colouring
}

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

uint4 GetValue( float4 c, int count )
{
	int v = MandelbrotSetPrimitive( c, count );
	if( v < 0 )
		return uint4(0,0,0,0);
	return HSVtoRGB( float3( convert_float(v), 1.0f, 1.0f ) );
}
*/


























double Length( double4 a )
{
	return sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w );
}

double Length2( double4 a )
{
	return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w;
}

double4 QuatMultiply( double4 q1, double4 q2 )
{
	double4 ret;
	ret.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	ret.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z;
	ret.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
	ret.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	return ret;
}

double GetAngle( double4 q ) // 0.0 - 1.0
{
	double len = Length(q);
	if( len > 0.00001 )
	{
		double w = q.w / len;
		if( w <= -1.0f )
			return 1.0f;
		if( w >= 1.0f )
			return 0.0f;
		return acospi( w );
	}
	return 0.0f;
}




double4 GetValue( double4 c, int count )
{
	double module = Length2( c );
	int i;
	double4 n = c;
	for( ; count > 0; --count )
	{
		n = QuatMultiply( n, n ) + c;
		if( Length2(n-c) <= 0.0000001f )
			return c;
		if( Length2(n) > module * 1024.0f )
			return (double4)(0.0f,0.0f,0.0f,1.0f)*1000000.0;
	}
	return n;
}

uchar4 AbsVal( double4 a )
{
	if( Length2(a) <= 0.000001f )
		return (uchar4)(0,0,0,255);
	
	uchar4 retval;
	double v;
	
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


























__kernel void MandelbrotSet( __global uchar4 * img, const int ioffset, const int width, const int height, __global double4 * ox, __global double4 * oy, const int MEMORY_BLOCK_ELEMENTS )
{
    int i = ioffset + get_global_id(0);
    
    double scale = 0.4f;
    
	double2 coord;
	coord.x = i%width;
	coord.y = i/width;
	coord.x /= convert_double(width);
	coord.y /= convert_double(height);
	
	coord.x -= 0.5f;
	coord.y -= 0.5f;
	
	coord.x *= scale;
	coord.y *= scale;
	
	coord.y -= 0.1f;
	coord.y -= 0.5f;
	coord.x += 0.7f;
	
	//coord.x += 0.005f * 0.275;
	//coord.y -= 0.005f * 0.25f;
	
	double4 pos;
	
    pos = (ox[0]*coord.x) + (oy[0]*coord.y);
	
	uchar4 value;
	double4 mx = ox[0] * scale * 0.04f / convert_double(width);
	double4 my = oy[0] * scale * 0.04f / convert_double(height);
	
	uint4 valuetemp;
	const int fractalIterations = 120;
	
	
	//valuetemp = convert_uint4( AbsVal( GetValue( pos, fractalIterations ) ) ) * 53;
	
	
	valuetemp = convert_uint4( AbsVal( GetValue( pos, fractalIterations ) ) ) * 5;
	
	valuetemp += convert_uint4( AbsVal( GetValue( pos+mx, fractalIterations ) ) ) * 3;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-mx, fractalIterations ) ) ) * 3;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my, fractalIterations ) ) ) * 3;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my, fractalIterations ) ) ) * 3;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-my-mx-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-my+mx+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+my-mx-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+my+mx+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+my-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+my+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-my-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-my+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my+mx+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my-mx-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-mx-mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+mx+mx, fractalIterations ) ) ) * 2;
	valuetemp += convert_uint4( AbsVal( GetValue( pos-my-my, fractalIterations ) ) );
	valuetemp += convert_uint4( AbsVal( GetValue( pos+my+my, fractalIterations ) ) );
	valuetemp += convert_uint4( AbsVal( GetValue( pos+mx+mx, fractalIterations ) ) );
	valuetemp += convert_uint4( AbsVal( GetValue( pos-mx-mx, fractalIterations ) ) );
	
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

