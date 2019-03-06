
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

int width = 16384-1024, height = 16384-1024;

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
	
	cl_kernel * function = CL::GetFunction( "Mandelbrot.cl", "MandelbrotSet" );
	
	unsigned char * img = CL::Allocate<unsigned char>( width*height, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR );
	int beg = clock();
	
	CL::For( width*height, function, img, width, height );
	CL::Finish();
	
	printf( "\n Calculated in: %ims", clock()-beg );
	
	CL::CopyFromVRAM( img );
	
	beg = clock();

	std::vector<unsigned char>img_( width*height*4 );
	for( i = 0; i < width*height; ++i )
	{
		img_[(i<<2)+0] = img[i];
		img_[(i<<2)+1] = img[i];
		img_[(i<<2)+2] = img[i];
		img_[(i<<2)+3] = 255;
	}
	
	lodepng::encode( "fractal.png", img_, width, height );
	printf( "\n Saved in: %ims", clock()-beg );
	
	CL::DestroyFunction( function );
	CL::Free( img );
	
	return 0;
}



