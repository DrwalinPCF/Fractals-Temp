
float2 GetValue( float2 c, int count )
{
	float2 n = c;
	for( ; count > 0; --count )
	{
		float2 a = n;
		n.x = (a.x*a.x) - (a.y*a.y) + c.x;
		n.y = 2.0*a.x*a.y + c.y;
	}
	return n;
}

uchar AbsVal( float2 a )
{
	float v = sqrt( a.x*a.x + a.y*a.y ) * 350.0f;
	if( v < 0.0f )
		return 0;
	if( v > 255 )
		return 255;
	return convert_uchar_sat(v);
}

__kernel void MandelbrotSet( __global uchar * img, const int width, const int height )
{
    int i = get_global_id(0);
    
    float2 pos;
	pos.x = i%width;
	pos.y = i/width;
	pos.x /= convert_float(width);
	pos.y /= convert_float(height);
	
	pos.y -= 0.5f;
	pos.x -= 0.5f;
	
	pos *= 0.001f;
	
	pos.x -= 0.741f;
	pos.y -= 0.207f;
	
	
	//pos.x *= 1.8f;
	//pos.y *= 1.8f;
	
	uchar value = 255 - AbsVal( GetValue( pos, 100 ) );
	
	img[i] = value;
}

