// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cmath>
namespace TACommonTypes
{
	struct FTAVector2d
	{
		FTAVector2d(double _x, double _y) : x(_x), y(_y)
		{}
		FTAVector2d() : x(0), y(0){}
		float x, y;
		FTAVector2d operator+( const FTAVector2d& rhs ) const
		{
			return FTAVector2d( x + rhs.x, y + rhs.y );
		}
		FTAVector2d operator-( const FTAVector2d& rhs ) const
		{
			return FTAVector2d( x - rhs.x, y - rhs.y );
		}
		FTAVector2d operator*( const float rhs ) const
		{
			return FTAVector2d( x * rhs, y * rhs );
		}
		
		FTAVector2d operator/( const float rhs ) const
		{
			return FTAVector2d( x / rhs, y / rhs );
		}
		FTAVector2d& operator+=( const FTAVector2d& rhs )
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		FTAVector2d& operator-=( const FTAVector2d& rhs )
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		FTAVector2d& operator*=( float val)
		{
			x *= val;
			y *= val;
			return *this;
		}

		bool operator==(const FTAVector2d& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}

		float Length() const
		{
			return std::sqrt( x*x + y*y );
		}

		float SquaredLength() const
		{
			return x*x + y*y;
		}
	};

	FTAVector2d Max(const TACommonTypes::FTAVector2d& First, const TACommonTypes::FTAVector2d& Second);
	
}

