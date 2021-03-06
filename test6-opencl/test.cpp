
#include <OpenCLWrapper.h>

#include <cstdio>
#include <cmath>

#include <vector>

#include <complex>

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

int width = 16384, height = 16384;

#include "..\lodepng.h"


#include <ctime>
#include <cstdlib>
#include <cstring>

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

int main()
{
	int i;
	
	srand( time( NULL ) );
	
	cl_kernel * function = CL::GetFunction( "Mandelbrot.c", "MandelbrotSet" );
	
	unsigned char * img1 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img2 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img3 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	unsigned char * img4 = CL::Allocate<unsigned char>( width*height*4/4, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	int beg = clock();
	
	 	
	
	for( i = 0; i+1024*1024*8 <= width*height; i+= 1024*1024*8 )
	{
		CL::For( 1024*1024*8, function, img1, img2, img3, img4, i, width, height );
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
	
	return 0;
}



