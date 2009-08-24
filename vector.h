#ifndef VECTOR_H
#define VECTOR_H

struct vector
{
  vector() : x(0), y(0) {}
  vector(double nx, double ny, double nz) : x(nx), y(ny), z(nz) {}

  double magnitude()
  {
    return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
  }

  vector normalize()
  {
    double denom = magnitude();
    return vector(x/denom, y/denom, z/denom);      
  }


  double x;
  double y;
  double z;
};

vector operator+(vector l, vector r);
vector operator+(double l, vector r);
vector operator+(vector r, double l);

vector operator-(vector l, vector r);
vector operator-(vector l, double r);
vector operator-( double l, vector r);

vector operator*(double l, vector r);
vector operator*(vector l, double r);
double operator*(vector l, vector r);

std::ostream& operator<<(std::ostream& out, const vector& v);


#endif