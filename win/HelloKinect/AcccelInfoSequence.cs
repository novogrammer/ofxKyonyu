using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace KinectKyonyu
{
    class AccelInfoSequence
    {
        AccelInfo[] list;
        int capacity = 10;
        int currentIndex = 0;
        AccelInfo currentInfo = null;
        AccelInfo prevInfo = null;
        AccelInfo refInfo = null;
        public AccelInfoSequence()
        {
            list = new AccelInfo[capacity];
        }


        internal void AddInfo(Microsoft.Research.Kinect.Nui.Vector vector)
        {
            prevInfo = (list[0] != null) ? this.list[(currentIndex + capacity - 1) % capacity] : null;
            refInfo = (list[5] != null) ? this.list[(currentIndex + capacity - 5) % capacity] : null;
            currentInfo = new AccelInfo(vector, refInfo);
            this.list[currentIndex] = currentInfo;
            currentIndex = (currentIndex+1) % capacity;
        }

        internal void rotate(System.Windows.Media.Media3D.Matrix3D rotationMatrix)
        {
            // 回転
            if (currentInfo == null) return;
            currentInfo.rotate(rotationMatrix);
        }

        internal AccelInfo getLast()
        {
            return currentInfo;
        }
    }
}
