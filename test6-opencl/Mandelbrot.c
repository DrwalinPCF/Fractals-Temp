
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

uchar4 AbsVal( float2 a )
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
	
	
	v = sqrt(a.x*a.x+a.y*a.y);
	if( v > 0.000001f )
	{
		a.x /= sqrt(a.x*a.x+a.y*a.y);
		a.y /= sqrt(a.x*a.x+a.y*a.y);
		
		v = (atan2pi( a.y, a.x )+1.0f) * 255.0f;
		
		if( v < 0.0f )
			retval.z = 0;
		else if( v > 255.0f )
			retval.z = 255;
		else
			retval.z = convert_uchar_sat(v);
	}
	else
	{
		retval.z = 0;
	}
	
	/*
	v = sqrt(a.x*a.x+a.y*a.y) * 50.0f;
	if( v < 0 )
		v *= -1.0f;
	
	if( v < 0.0f )
		retval.z = 0;
	else if( v > 255.0f )
		retval.z = 255;
	else
		retval.z = convert_uchar_sat(v);
	*/
	
	return retval;
}


__kernel void MandelbrotSet( __global uchar4 * img1, __global uchar4 * img2, __global uchar4 * img3, __global uchar4 * img4, const int ioffset, const int width, const int height )
{
    int i = ioffset + get_global_id(0);
    
    float2 pos;
	pos.x = i%width;
	pos.y = i/width;
	pos.x /= convert_float(width);
	pos.y /= convert_float(height);
	
	pos.y -= 0.5f;
	pos.x -= 0.5f;
	
	pos *= 0.0003f;
	pos.x -= 0.719898009f;
	pos.y -= 0.30453f;
	
	uchar4 value = AbsVal( GetValue( pos, 150 ) );
	
	if( i < width*height/4 )
		img1[i] = value;
	else if( i < width*height/2 )
		img2[i-(width*height/4)] = value;
	else if( i < width*height*3/4 )
		img3[i-(width*height/2)] = value;
	else if( i < width*height )
		img4[i-(width*height*3/4)] = value;
}

