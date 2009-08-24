#include <cmath>
#include <iostream>
#include "vector.h";

//Operator: +
//Adds to vectors togther and returns the resulting vector
vector operator+(vector l, vector r)
{
  return vector(l.x+r.x, l.y+r.y, l.z+r.z);
}

//Operator: +
//Adds a vector and a scaler togther
vector operator+(double l, vector r)
{
  return vector(l+r.x, l+r.y, l+r.z);
}

//Operator: +
//Adds a vector and a scaler togther
vector operator+(vector r, double l)
{
  return vector(l+r.x, l+r.y, l+r.z);
}


//operator: -
//subtracts two vectors and retunrs the resulting vector
vector operator-(vector l, vector r)
{
  return vector(l.x-r.x, l.y-r.y, l.z-r.z);
}

//operator: -
//subtracts a vector and a scaler resulting in a vector
vector operator-(vector l, double r)
{
  return vector(l.x-r, l.y-r, l.z-r);
}


//operator: -
//subtracts a vector and a scaler resulting in a vector
vector operator-(double r, vector l)
{
  return vector(l.x-r, l.y-r, l.z-r);
}

//operator: *
//multiplys a vector by a scaler
vector operator*(double l, vector r)
{
  return vector(l*r.x, l*r.y, l*r.z);
}

//operator: *
//multiplys a vector by a scaler
vector operator*(vector l, double r)
{
  return vector(l.x*r, l.y*r, l.z*r);
}

//operator: *
//returns the dot product of two vectors
double operator*(vector l, vector r)
{
  return l.x*r.x + l.y*r.y + l.z*r.z;
}

//operater: <<
//Vector to string function
std::ostream& operator<<(std::ostream& out, const vector& v)
{
  out << "(" << v.x << "," << v.y << "," << v.z << ")"; 
  return out;
}