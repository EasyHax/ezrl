#include "cmath"

// ScriptStruct Core.Object.Vector
// 0x000C
struct FVector
{
	FVector operator-(const FVector& other) const
	{
		FVector result = FVector{ this->X - other.X, this->Y - other.Y, this->Z - other.Z };
		return result;
	}

	FVector operator+(const FVector& other) const
	{
		FVector result = FVector{ this->X + other.X, this->Y + other.Y, this->Z + other.Z };
		return result;
	}

	FVector operator/(const float& value) const
	{
		FVector result = FVector{ this->X / value, this->Y / value, this->Z / value };
		return result;
	}

	bool operator==( const FVector& v ) const
	{
		bool result = this->X == v.Y && this->Y == v.Y && this->Z == v.Z;
		return result;
	}

	FVector operator*(const float& value) const
	{
		FVector result = FVector{ this->X * value, this->Y * value, this->Z * value };
		return result;
	}

	float length() const
	{
		float length = sqrt(this->X * this->X + this->Y * this->Y);
		return length;
	}

	float distance(const FVector& other) const
	{
		FVector diff = other - *this;
		float distance = sqrt(diff.X * diff.X + diff.Y * diff.Y);
		return distance;
	}

	float angle(const FVector& other) const
	{
		float angle = (std::atan2(other.Y, other.X) - std::atan2(this->Y, this->X)) * 57.2957795131;
		if ( angle >= 180.f ) angle -= 360.f;
		if ( angle < -180.f ) angle += 360.f;
		return angle;
	}

	bool is_null( void ) const {
		return !this->X && !this->Y && !this->Z;
	}

	float dot( const FVector& v ) const {
		float dot = this->X* v.X + this->Y * v.Y + this->Z * v.Z;
		return dot;
	}

	FVector cross( const FVector& v ) {
		FVector result;
		result.X = this->Y * v.Z - this->Z * v.Y;
		result.Y = this->Z * v.X - this->X * v.Z;
		result.Z = this->X * v.Y - this->Y * v.X;
		return result;
	}

	FVector normalized() const
	{
		float magnitude = sqrt(this->X * this->X + this->Y * this->Y);
		FVector n = FVector{ this->X / magnitude, this->Y / magnitude, 0 };
		return n;
	}

	float                                              X;                                             // 0x0000 (0x0004) [0x0000000000000001] (CPF_Edit)    
	float                                              Y;                                             // 0x0004 (0x0004) [0x0000000000000001] (CPF_Edit)    
	float                                              Z;                                             // 0x0008 (0x0004) [0x0000000000000001] (CPF_Edit)
};