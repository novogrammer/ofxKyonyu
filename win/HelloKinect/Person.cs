using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using Microsoft.Research.Kinect.Nui;

namespace KinectKyonyu
{
    class Person
    {
        public MeshGeometry3D oppaiLeftModel;
        public MeshGeometry3D oppaiRightModel;
        public ImageBrush materialImageLeft;
        public ImageBrush materialImageRight;

        public GeometryModel3D oppaiRootLeft;
        public GeometryModel3D oppaiRootRight;

        public Oppai oppaiLeft;
        public Oppai oppaiRight;

        public SkeletonData skeleton;
        public int skeletonId;
        public bool tracked;

        public bool initDone;
        public bool needRefresh;

        public Person(System.Windows.Media.Media3D.MeshGeometry3D oppaiLeftModel, System.Windows.Media.Media3D.MeshGeometry3D oppaiRightModel, System.Windows.Media.ImageBrush materialImageLeft, System.Windows.Media.ImageBrush materialImageRight,
            GeometryModel3D oppaiRootLeft, GeometryModel3D oppaiRootRight)
        {
            // TODO: Complete member initialization
            this.needRefresh = true;
            this.oppaiLeftModel = oppaiLeftModel;
            this.oppaiRightModel = oppaiRightModel;
            this.materialImageLeft = materialImageLeft;
            this.materialImageRight = materialImageRight;
            this.oppaiRootLeft = oppaiRootLeft;
            this.oppaiRootRight = oppaiRootRight;
        }

        internal void setSkeletonId(int pId)
        {
            needRefresh = (pId == this.skeletonId);
            this.skeletonId = pId;
        }

        System.DateTime lastTextureModified;
        internal bool isTextureExpired()
        {
            if (lastTextureModified == null) return true;
            else if ((System.DateTime.Now - lastTextureModified).TotalMilliseconds > 2000)
            {
                lastTextureModified = System.DateTime.Now;
                return true;
            }
            return false;
        }
    }
}
