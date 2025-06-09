/*
Arquivo auxiliar para representar um vetor tridimensional, alem de suas operações básicas.
Esse arquivo é utilizado por outras classes, como Objeto3D, para manipular pontos e vetores
no espaço tridimensional. Contém operações como adição, subtração, multiplicação por escalar,
normalização, produto escalar e produto vetorial.
*/


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
   }   float length() const
   {
       return sqrt(x*x + y*y + z*z);
   }

   void normalize()
   {
       float norm = length();

       if(norm == 0.0)
       {
          x = 1;
          y = 1;
          z = 1;
          return;
       }
       x /= norm;
       y /= norm;
       z /= norm;
   }   Vector3 operator - (const Vector3& v) const
   {
        Vector3 aux( x - v.x, y - v.y, z - v.z);
        return( aux );
   }

   Vector3 operator + (const Vector3& v) const
   {
       Vector3 aux( x + v.x, y + v.y, z + v.z);
       return( aux );
   }

   Vector3 operator * (float scalar) const
   {
       Vector3 aux( x * scalar, y * scalar, z * scalar);
       return( aux );
   }// produto vetorial (cross product)
   Vector3 cross(const Vector3& v) const
   {
       Vector3 result;
       result.x = y * v.z - z * v.y;
       result.y = z * v.x - x * v.z;
       result.z = x * v.y - y * v.x;
       return result;
   }// produto escalar (dot product)
   float dot(const Vector3& v) const
   {
       return x * v.x + y * v.y + z * v.z;
   }
};

#endif
