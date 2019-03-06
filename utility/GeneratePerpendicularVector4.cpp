
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <conio.h>

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

void Print( float * v )
{
	printf( "( %f, %f, %f, %f )", v[0], v[1], v[2], v[3] );
}

int main()
{
	srand( time( NULL ) );
	float a[4];
	float b[4];
	
	Random( a );
	Random( b );
	printf( "\n " );
	Print( a );
	while( true )
	{
		if( abs(dot(a,b)) < 0.01 )
		{
			printf( "\n " );
			Print( b );
			getch();
		}
		Random(b);
	}
	
	
	return 0;
}



