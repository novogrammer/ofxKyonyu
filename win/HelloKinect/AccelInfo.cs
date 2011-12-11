using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.Kinect.Nui;
using System.Windows.Media.Media3D;

namespace KinectKyonyu
{
    class AccelInfo
    {
        // ワールド座標系における位置
        private Vector3D vector;
        // ワールド座標系における速度
        public Vector3D velocity;
        // ワールド座標系における加速度
        public Vector3D acceleration;

        // 胸板座標系における位置
        private Vector3D localVector;
        // 胸板座標系における速度
        public Vector3D localVelocity;
        // 胸板座標系における加速度
        public Vector3D localAcceleration;

        public float spanSec;
        

        public System.DateTime timestamp;

        int i = 0;
        public AccelInfo(Vector vector, AccelInfo prevInfo)
        {
            i++;
            timestamp = System.DateTime.Now;
            this.vector = new Vector3D(vector.X, vector.Y, vector.Z);
            if (prevInfo != null)
            {
                TimeSpan span = timestamp - prevInfo.timestamp;
                spanSec = (float)span.TotalMilliseconds / 1000f;
                if (spanSec < 20f /1000f)
                {
                    spanSec = 20f/1000f;
                }
                //速度を計算
                velocity = new Vector3D();
                velocity.X = (vector.X - prevInfo.vector.X)/spanSec;
                velocity.Y = (vector.Y - prevInfo.vector.Y) / spanSec;
                velocity.Z = (vector.Z - prevInfo.vector.Z) / spanSec;
                //加速度を計算
                acceleration = new Vector3D();
                acceleration.X = (velocity.X - prevInfo.velocity.X) / spanSec;
                acceleration.Y = (velocity.Y - prevInfo.velocity.Y) / spanSec;
                acceleration.Z = (velocity.Z - prevInfo.velocity.Z) / spanSec;
            }
            else
            {
                velocity = new Vector3D();
                velocity.X = 0;
                velocity.Y = 0;
                velocity.Z = 0;
                acceleration = new Vector3D();
                acceleration.X = 0;
                acceleration.Y = 0;
                acceleration.Z = 0;
            }
        }

        internal void rotate(System.Windows.Media.Media3D.Matrix3D rotationMatrix)
        {
            //localAcceleration = acceleration;
            localAcceleration = -1f * acceleration * rotationMatrix;

        }
    }
}
