
#include "..\OpenCLwrapper\OpenCLWrapper.h"
//#include <OpenCLWrapper.h>
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

template < typename T >
T dot( T * a, T * b )
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

template < typename T >
T Length( T * v )
{
	return sqrt( dot(v,v) );
}

template < typename T >
void Divide( T * v, T f )
{
	for( int i = 0; i < 4; ++i )
		v[i] /= f;
}

template < typename T >
void Normalize( T * v )
{
	T len = Length(v);
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
	//srand( time( NULL ) );
	
	int i, j, k, l;
	const int MEMORY_BLOCK_ELEMENTS = 1024*1024*32;
	const int ITERATIONS_QUEUEING = 1024*32;
	
	cl_kernel * function = CL::GetFunction( "Mandelbrot.c", "MandelbrotSet" );
	
	std::vector<unsigned char>img( width*height*4 );
	
	unsigned char * img1 = CL::Allocate<unsigned char>( MEMORY_BLOCK_ELEMENTS*4, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	
	double * ox = CL::Allocate<double>( 4, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
	double * oy = CL::Allocate<double>( 4, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
	
	//GetOxOy( ox, oy );
	ox[0] = 0.0;
	ox[1] = 1.0;
	ox[2] = 0.0;
	ox[3] = 0.0;
	Normalize(ox);
	CL::CopyToVRAM( ox );
	
	oy[0] = 0.0;
	oy[1] = 0.0;
	oy[2] = 0.0;
	oy[3] = 1.0;
	Normalize(oy);
	CL::CopyToVRAM( oy );
	
	int beg = clock();
	
	for( l = 0, i = 0, j = 0; j <= (width*height)/MEMORY_BLOCK_ELEMENTS; ++j )
	{
		for( k = 0; i+ITERATIONS_QUEUEING <= width*height && k+ITERATIONS_QUEUEING <= MEMORY_BLOCK_ELEMENTS; i += ITERATIONS_QUEUEING, k += ITERATIONS_QUEUEING, ++l )
		{
			CL::For( ITERATIONS_QUEUEING, function, img1, i, width, height, ox, oy, MEMORY_BLOCK_ELEMENTS );
			CL::Finish();
			if( l % 32 == 0 )
				printf( "\n Done %7.3f%% in %7.3fs estimated time to end: %7.3fs = %6.3fh", float(i)*100.0f/float(width*height), float(clock()-beg)/1000.0f, (float(clock()-beg)/1000.0f)*float(width*height-i)/float(i+1), (float(clock()-beg)/1000.0f)*float(width*height-i)/float(i+1)/3600.0f );
		}
		CL::CopyFromVRAM( img1 );
		unsigned bytes = MEMORY_BLOCK_ELEMENTS*4;
		if( img.size() - j*bytes < bytes )
			bytes = img.size() % bytes;
		memmove( &(img[j*MEMORY_BLOCK_ELEMENTS*4]), img1, bytes );
	}
	printf( "\n Calculated in: %ims", clock()-beg );
	
	CL::Free( img1 );
	
	beg = clock();
	lodepng::encode( "fractal.png", img, width, height );
	printf( "\n Saved in: %ims", clock()-beg );
	
	CL::DestroyFunction( function );
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



