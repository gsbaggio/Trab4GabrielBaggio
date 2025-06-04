#ifndef __VECTOR_3_H__
#define __VECTOR_3_H__

#include <math.h>
#include <stdio.h>

class Vector3
{
public:
   float x, y, z;

   Vector3()
   {
      x = y = z = 0;
   }

   Vector3(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   void set(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   float length()
   {
       return sqrt(x*x + y*y + z*z);
   }

   void normalize()
   {
       float norm = length();

       if(norm == 0.0)
       {
          printf("\n\nNormalize::Divisao por zero");
          x = 1;
          y = 1;
          z = 1;
          return;
       }
       x /= norm;
       y /= norm;
       z /= norm;
   }

   Vector3 operator - (const Vector3& v)
   {
        Vector3 aux( x - v.x, y - v.y, z - v.z);
        return( aux );
   }

   Vector3 operator + (const Vector3& v)
   {
       Vector3 aux( x + v.x, y + v.y, z + v.z);
       return( aux );
   }

   Vector3 operator * (float scalar)
   {
       Vector3 aux( x * scalar, y * scalar, z * scalar);
       return( aux );
   }

   // Produto vetorial (cross product)
   Vector3 cross(const Vector3& v)
   {
       Vector3 result;
       result.x = y * v.z - z * v.y;
       result.y = z * v.x - x * v.z;
       result.z = x * v.y - y * v.x;
       return result;
   }

   // Produto escalar (dot product)
   float dot(const Vector3& v)
   {
       return x * v.x + y * v.y + z * v.z;
   }
};

#endif
