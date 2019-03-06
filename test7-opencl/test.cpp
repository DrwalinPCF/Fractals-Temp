

#include <OpenCLWrapper.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <complex>
#include <ctime>
#include <cstdlib>
#include "..\lodepng.h"
#include <cstring>

typedef long double real;
typedef std::complex<real> complex;

class vec
{
public:
	real x, y;

	vec(real a, real b):x(a),y(b){}
	vec(real a):x(a),y(a){}
	vec():x(0),y(0){}

	vec operator+(vec o)const{return vec(x+o.x,y+o.y);}
	vec operator-(vec o)const{return vec(x-o.x,y-o.y);}
	vec& operator+=(vec o){x+=o.x;y+=o.y;return*this;}
	vec& operator-=(vec o){x-=o.x;y-=o.y;return*this;}
	real dot(vec o)const{return(x*o.x)+(y*o.y);}
	real len()const{return sqrt(this->dot(*this));}
	real len2()const{return this->dot(*this);}
	vec operator*(real o)const{return vec(x*o,y*o);}
	vec operator/(real o)const{return vec(x/o,y/o);}
	vec& operator*=(real o){x*=o;y*=o;return *this;}
	vec& operator/=(real o){x/=o;y/=o;return *this;}
	vec normal()const{return (*this)/this->len();}
	vec rotate(float arg)const
	{
		real sarg = sin(arg);
		real carg = cos(arg);
		vec ret;
		ret.x = x*carg + y*sarg;
		ret.y = -x*sarg + y*carg;
		return ret;
	};
};


float dot( float * a, float * b )
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

float Length( float * v )
{
	return sqrt( dot(v,v) );
}

void Divide( float * v, float f )
{
	for( int i = 0; i < 4; ++i )
		v[i] /= f;
}

void Normalize( float * v )
{
	float len = Length(v);
	if( len > 0.001 )
	{
		Divide( v, len );
	}
}

int width = 16384/4, height = 16384/4;



void Fractal( unsigned char color, int steps, vec a, vec b, vec dir, std::vector<unsigned char> & img, int width, int height );

complex GetValue( complex c, int count )
{
	complex n = c;
	for( ; count > 0; --count )
	{
		n = n*n + c;
	}
	return n;
}

void GetOxOy( float * ox, float * oy );

int main()
{
	int i;
	
	//srand( time( NULL ) );
	
	cl_kernel * function = CL::GetFunction( "Mandelbrot.c", "MandelbrotSet" );
	
	unsigned char * img1 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img2 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img3 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img4 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	int beg = clock();
	
	float * ox = CL::Allocate<float>( 4, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
	float * oy = CL::Allocate<float>( 4, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
	
	//GetOxOy( ox, oy );
	ox[0] = 0.0f;
	ox[1] = 1.0f;
	ox[2] = 0.0f;
	ox[3] = 0.0f;
	oy[0] = 0.0f;
	oy[1] = 0.0f;
	oy[2] = 0.0f;
	oy[3] = 1.0f;
	
	Normalize(ox);
	Normalize(oy);
	
	CL::CopyToVRAM( ox );
	CL::CopyToVRAM( oy );
	
	for( i = 0; i+1024*1024 <= width*height; i+= 1024*1024 )
	{
		CL::For( 1024*1024, function, img1, img2, img3, img4, i, width, height, ox, oy );
		CL::Finish();
	}
	
	printf( "\n Calculated in: %ims", clock()-beg );
	
	CL::CopyFromVRAM( img1 );
	CL::CopyFromVRAM( img2 );
	CL::CopyFromVRAM( img3 );
	CL::CopyFromVRAM( img4 );
	
	beg = clock();

	std::vector<unsigned char>img_( width*height*4 );
	for( i = 0; i < width*height; ++i )
	{
		unsigned char v = 255;
		unsigned char u = 255;
		unsigned char w = 255;
		if( i < width*height/4 )
		{
			v = img1[i*4];
			u = img1[(i*4)+1];
			w = img1[(i*4)+2];
		}
		else if( i < width*height/2 )
		{
			v = img2[(i-(width*height/4))*4];
			u = img2[((i-(width*height/4))*4)+1];
			w = img2[((i-(width*height/4))*4)+2];
		}
		else if( i < width*height*3/4 )
		{
			v = img3[(i-(width*height/2))*4];
			u = img3[((i-(width*height/2))*4)+1];
			w = img3[((i-(width*height/2))*4)+2];
		}
		else if( i < width*height )
		{
			v = img4[(i-(width*height*3/4))*4];
			u = img4[((i-(width*height*3/4))*4)+1];
			w = img4[((i-(width*height*3/4))*4)+2];
		}
		img_[(i<<2)+0] = v;
		img_[(i<<2)+1] = u;
		img_[(i<<2)+2] = w;
		img_[(i<<2)+3] = 255;
	}
	
	lodepng::encode( "fractal.png", img_, width, height );
	printf( "\n Saved in: %ims", clock()-beg );
	
	CL::DestroyFunction( function );
	CL::Free( img1 );
	CL::Free( img2 );
	CL::Free( img3 );
	CL::Free( img4 );
	CL::Free( ox );
	CL::Free( oy );
	
	return 0;
}









void Random( float * v )
{
	while( true )
	{
		for( int i = 0; i < 4; ++i )
			v[i] = float(rand()%8193)-4096.0f;
		if( Length(v) > 5.0f )
			break;
	}
	Normalize( v );
}

void GetOxOy( float * a, float * b )
{
	Random( a );
	Random( b );
	while( true )
	{
		if( abs(dot(a,b)) < 0.00001 )
		{
			return;
		}
		Random(b);
	}
}



