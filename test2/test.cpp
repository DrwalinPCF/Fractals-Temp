
#include <cstdio>
#include <cmath>

#include <vector>

typedef float real;

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

int main()
{
	srand( time( NULL ) );
	
	std::vector<unsigned char>img;
	img.resize( width*height*4 );
	
	for( int i = 0; i < img.size(); ++i )
		img[i] = 255;
	
	int beg = clock();
	
	Fractal( 255, 10, vec(5,5), vec(width-5,5), vec(0,1), img, width, height );
	printf( "\n Done in: %ims", clock()-beg );
	
	beg = clock();

	lodepng::encode( "fractal.png", img, width, height );
	printf( "\n Saved in: %ims", clock()-beg );
	
	return 0;
}






void Fractal( unsigned char color, int steps, vec a, vec b, vec dir, std::vector<unsigned char> & img, int width, int height )
{
	color = 0;
	
	//dir = dir/dir.len();
	real max = (a-b).len();
	vec ox = (a-b)/max;
	max /= 3.0;
	vec origin = (ox*max) + b;
	
	real x, y;
	vec p;
	
	for( x = 0.0; x <= max; x+=0.5 )
	{
		for( y = 0.0, p = origin + (ox*x); y <= max; y += 0.5, p += dir * 0.5 )
		{
			int A = int(p.x+0.5);
			int B = int(p.y+0.5);
			if( A >= 0 && B >= 0 && A < width && B < height )
			{
				int i = (B*width) + A;
				i <<= 2;
				
				img[i] = color;
				img[i+1] = color;
				img[i+2] = color;
				img[i+3] = 255;
			}
		}
	}
	
	if( steps > 1 )
	{
		vec A, B;
		
		A = origin;
		B = origin + (dir*max);
		Fractal( color-16, steps-1, A, B, ox*(-1.0), img, width, height );
		
		A = origin + (dir*max);
		B = origin + (ox*max) + (dir*max);
		Fractal( color-16, steps-1, A, B, dir, img, width, height );
		
		A = origin + (ox*max);
		B = origin + (dir*max) + (ox*max);
		Fractal( color-16, steps-1, A, B, ox, img, width, height );
		
	}
}






