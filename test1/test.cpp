
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
};


std::vector<vec> points;
std::vector<real> vals;
int width = 4096, height = 4096;

#include "..\lodepng.h"
#include <iostream>
void encodeOneStep( const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height )
{
	unsigned error = lodepng::encode(filename, image, width, height);
	if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void EncodeVals( const char * file )
{
	real min = 1000000.0;
	real max = -100000.0;

	for( int i = 0; i < vals.size(); ++i )
	{
		if( vals[i] > 0.0001 )
		{
			if( min > vals[i] ) min = vals[i];
			if( max < vals[i] ) max = vals[i];
		}
	}

	std::vector<unsigned char> image;
	image.resize(vals.size()*4);
	for( int i = 0; i < vals.size(); ++i )
	{
		unsigned char v = 0;
		if( vals[i] > -0.00001 )
		{
			real t = 1.0f - ( ( (vals[i]+0.00001-min) / (max-min) ) * 0.979 );
			if( t < 0.0 )
				v = 0;
			else if( t >= 1.0 )
				v = 255;
			else
				v = (unsigned char)(t*250.0);
		}
		image[(i<<2)+0] = v;
		image[(i<<2)+1] = v;
		image[(i<<2)+2] = v;
		image[(i<<2)+3] = 255;
	}

	for( int i = 0; i < points.size(); ++i )
	{

	}

	encodeOneStep(file, image, width, height);
}

struct PAIR
{
	real dist;
	int p;
};


real Modify( real x, vec p, vec Pa, vec Pb )
{
	vec a = p-Pa;
	vec b = Pb-p;
	real t = a.dot(b);
	real A = a.len2();
	real B = b.len2();
	if( A > 0.0001 && B > 0.0001 )
	{
		t /= sqrt(A*B);
		
		const real factor_angle = 0.99994;
		
		if( t > factor_angle )
		{
			t = 1.0f-(t-factor_angle)/(1.0-factor_angle);
			
			x *= 0.9;
			
			//ret *= pow( 1.0f, -t*1000.0 );
			//std::cout << "\n t=" << t;
		}
	}
	return x;
}


real Multiply( vec p )
{
	real ret = 1.0;
	
	for( int i = 0; i < points.size(); ++i )
	{
		ret += log((p-points[i]).len2());
	}
	
	std::vector<PAIR> elem;
	
	PAIR pa, pb;
	pa.p = 0;
	pb.p = 0;
	pa.dist = 1000000000000.0;
	pb.dist = 1000000000000.0;
	
	for( int i = 0; i < points.size(); ++i )
	{
		real dist = (p-points[i]).len2();
		if( dist < pa.dist )
		{
			pb = pa;
			pa.p = i;
			pa.dist = dist;
		}
		else if( dist < pb.dist )
		{
			pb.p = i;
			pb.dist = dist;
		}
	}
	
	ret = Modify( ret, p, points[pa.p], points[pb.p] );
	
	/*
	for( int i = 0; i < points.size(); ++i )
	{
		for( int j = i+1; j < points.size(); ++j )
		{
			vec a = p-points[i];
			vec b = points[j]-p;
			real t = a.dot(b);
			real A = a.len2();
			real B = b.len2();
			if( A > 0.0001 && B > 0.0001 )
			{
				t /= sqrt(A*B);
				
				const real factor_angle = 0.99994;
				
				if( t > factor_angle )
				{
					t = 1.0f-(t-factor_angle)/(1.0-factor_angle);
					
					ret *= 0.9;
					
					//ret *= pow( 1.0f, -t*1000.0 );
					//std::cout << "\n t=" << t;
					return ret;
				}
			}
		}
	}
	*/
	
	return ret;//log(ret);
}

bool Contain( vec p, real err = 0.001 )
{
	err *= err;
	for( int i = 0; i < points.size(); ++i )
	{
		if( (points[i]-p).len2() <= err )
			return true;
	}
	
	return false;
}

#include <ctime>
#include <cstdlib>

int main()
{
	srand( time( NULL ) );

	for( int i = 0; i < 16; ++i )
	{
		points.emplace_back( real(rand()%width), real(rand()%height) );
	}
	
	for( int i = 0; i < points.size(); ++i )
	{
		//points[i] += (vec(width,height)-vec(1024))*0.5;
	}

	vals.resize( width*height );

	while( true )
	{
		real max = 0.0;
		float temp = 0.0f;
		char filename[1000];
		scanf( "%f%s", &temp, filename );
		
		int beginTime = clock();
		
		for( int i = 0; i < 63; ++i )
			printf( "-" );
		printf( "|\n" );
		max = temp;
		int x, y;
		int i = 0;
		for( y = 0; y < height; ++y )
		{
			if( y%(height/64) == 0 )
			{
				printf( "." );
			}
			for( x = 0; x < width; ++x, ++i )
			{
				vals[i] = Multiply(vec(x,y));
				if( vals[i] > max )
				{
					vals[i] = -1000000.0;
				}
			}
		}
		EncodeVals(filename);
		printf( "\n Render time: %i ", clock()-beginTime );
	}

	return 0;
}


