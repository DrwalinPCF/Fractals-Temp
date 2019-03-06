
float2 GetValue( float2 c, int count )
{
	int i;
	float2 n = c;
	for( ; count > 0; --count )
	{
		float2 a;
		for( i = 0; i < 1; ++i )
		{
			a = n;
			n.x = (a.x*a.x) - (a.y*a.y);
			n.y = 2.0*a.x*a.y;
		}
		n += c;
	}
	return n;
}

uchar2 AbsVal( float2 a )
{
	uchar2 retval;
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
	
	return retval;
}


__kernel void MandelbrotSet( __global uchar2 * img1, __global uchar2 * img2, __global uchar2 * img3, __global uchar2 * img4, const int ioffset, const int width, const int height )
{
    int i = ioffset + get_global_id(0);
    
    float2 pos;
	pos.x = i%width;
	pos.y = i/width;
	pos.x /= convert_float(width);
	pos.y /= convert_float(height);
	
	pos.y -= 0.5f;
	pos.x -= 0.5f;
	
	//pos *= 0.002f;
	//pos.x -= 0.719898009f;
	//pos.y -= 0.30453f;
	
	pos *= 0.001f;
	pos.x -= 1.403f;
	pos.y -= 0.0f;
	
	
	uchar2 value = AbsVal( GetValue( pos, 150 ) );
	
	
	if( i < width*height/4 )
	{
		img1[i] = value;
	}
	else if( i < width*height/2 )
	{
		img2[i-(width*height/4)] = value;
	}
	else if( i < width*height*3/4 )
	{
		img3[i-(width*height/2)] = value;
	}
	else if( i < width*height )
	{
		img4[i-(width*height*3/4)] = value;
	}
	
}

