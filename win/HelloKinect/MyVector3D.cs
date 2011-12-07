using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KinectKyonyu
{
    class MyVector3D
    {
        double x;
        double y;
        double z;

        public double getX() { return x; }
        public double getY() { return y; }
        public double getZ() { return z; }
        
        public MyVector3D(double _x, double _y, double _z)
        {
            this.x = _x;
            this.y = _y;
            this.z = _z;
        }
        public MyVector3D getOutorProduct(MyVector3D v)
        {
            return new MyVector3D(
                this.y*v.z - this.z*v.y,
                this.z*v.x - this.x*v.z,
                this.x*v.y - this.y*v.x
                );
        }
        public MyVector3D normalize()
        {
            double norm = this.getNorm();
            return new MyVector3D(x / norm, y / norm, z / norm);
        }
        public double getNorm()
        {
            return Math.Sqrt(x*x + y*y + z*z);
        }
    }
}
