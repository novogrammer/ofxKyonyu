using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;
using Microsoft.Research.Kinect.Nui;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Media.Imaging;

namespace KinectKyonyu
{
    class BodyTexture
    {

        public ImageSource ImageSourceLeft;
        public ImageSource ImageSourceRight;
        private Microsoft.Research.Kinect.Nui.SkeletonData skeleton;
        private byte[] latestVideoBytes;

        int imgWidth = 64;
        int imgHeight = 64;
        int origWidth = 640;
        int origHeight = 480;

        Person person1;
        Person person2;

        Vector origPosition;
        Vector vectorX;
        Vector vectorY;

        private   Microsoft.Research.Kinect.Nui.Runtime nui;
        
        public  BodyTexture(Microsoft.Research.Kinect.Nui.Runtime nui,Microsoft.Research.Kinect.Nui.SkeletonData skeleton,byte[] latestVideoBytes)
        {
            this.skeleton = skeleton;
            this.latestVideoBytes = latestVideoBytes;
            this.nui = nui;
            byte[] thumbBitsLeft = new byte[imgWidth * imgHeight * 4];
            byte[] thumbBitsRight = new byte[imgWidth * imgHeight * 4];

            this.calcPositions();


            for (int y = 0; y < imgHeight; y++)
            {
                for (int x = 0; x < imgWidth; x++)
                {
                    float screenXLeft = 0;
                    float screenYLeft = 0;
                    float screenXRight = 0;
                    float screenYRight = 0;
                    getPosition2D((float)x * 0.5f, y, ref screenXLeft, ref screenYLeft);
                    getPosition2D((float)(x + imgWidth) * 0.5f, y, ref screenXRight, ref screenYRight);

                    if (screenXRight > 1 || screenXRight < 0) screenXRight = 0;
                    if (screenYRight > 1 || screenYRight < 0) screenYRight = 0;
                    if (screenXLeft > 1 || screenXLeft < 0) screenXLeft = 0;
                    if (screenYLeft > 1 || screenYLeft < 0) screenYLeft = 0;

                    int origIndexLeft = ((int)(screenXLeft * (float)origWidth) + (int)(screenYLeft * (float)origHeight) * origWidth) * 4;
                    int origIndexRight = ((int)(screenXRight * (float)origWidth) + (int)(screenYRight * (float)origHeight) * origWidth) * 4;

                    int newIndex = (x + y * imgWidth) * 4;

                    byte[] leftBytesOrig = {
                                               latestVideoBytes[origIndexLeft + 0],
                                               latestVideoBytes[origIndexLeft + 1],
                                               latestVideoBytes[origIndexLeft + 2],
                                               latestVideoBytes[origIndexLeft + 3],
                                           };
                    byte[] rightBytesOrig = {
                                               latestVideoBytes[origIndexRight + 0],
                                               latestVideoBytes[origIndexRight + 1],
                                               latestVideoBytes[origIndexRight + 2],
                                               latestVideoBytes[origIndexRight + 3],
                                           };
                    byte[] leftBytes = getColorAdjustedRGBA(leftBytesOrig, (double)y / (double)imgHeight);
                    byte[] rightBytes = getColorAdjustedRGBA(rightBytesOrig, (double)y / (double)imgHeight);

                    thumbBitsLeft[newIndex + 0] = leftBytes[0];
                    thumbBitsLeft[newIndex + 1] = leftBytes[1];
                    thumbBitsLeft[newIndex + 2] = leftBytes[2];
                    thumbBitsLeft[newIndex + 3] = leftBytes[3];

                    thumbBitsRight[newIndex + 0] = rightBytes[0];
                    thumbBitsRight[newIndex + 1] = rightBytes[1];
                    thumbBitsRight[newIndex + 2] = rightBytes[2];
                    thumbBitsRight[newIndex + 3] = rightBytes[3];
                }
            }
            ImageSourceLeft = BitmapSource.Create(imgWidth, imgHeight, 96, 96, PixelFormats.Bgr32, null, thumbBitsLeft, imgWidth * 4);
            ImageSourceRight = BitmapSource.Create(imgWidth, imgHeight, 96, 96, PixelFormats.Bgr32, null, thumbBitsRight, imgWidth * 4);
        }

        static double COLOR_RATIO = 0.4;

        /**
         * WPFだと陰影の表現が平坦なので色を補正。
         * */
        private byte[] getColorAdjustedRGBA(byte[] colorBytes, double positionRatio)
        {
            byte r = getColorByte(colorBytes[0], positionRatio);
            byte g = getColorByte(colorBytes[1], positionRatio);
            byte b = getColorByte(colorBytes[2], positionRatio);
            byte[] bytes = {r, g, b, colorBytes[3]};
            return bytes;
        }

        private byte getColorByte(byte _b, double positionRatio)
        {
            int b = _b;
            b += (int)((0.5-positionRatio) * (double)255 * COLOR_RATIO);
            return (byte)(Math.Min(255, Math.Max(0, b)));
        }


        float offsetTopRatio = 0.30f;
        float offsetBottomRatio = 0.37f;
        float offsetSideRatio = 0.05f;

        private void getPosition2D(float x, float y, ref float outX, ref float outY)
        {
            float ratioX = offsetSideRatio + ((float)x / (float)imgWidth) * (1f - offsetSideRatio * 2f);
            float ratioY = offsetTopRatio + ((float)y / (float)imgHeight) * (1f - offsetTopRatio - offsetBottomRatio);

            float vX = origPosition.X + ratioX * vectorX.X + ratioY * vectorY.X;
            float vY = origPosition.Y + ratioX * vectorX.Y + ratioY * vectorY.Y;
            float vZ = origPosition.Z + ratioX * vectorX.Z + ratioY * vectorY.Z;
            Joint joint = new Joint()
            {
                Position = new Microsoft.Research.Kinect.Nui.Vector()
                {
                    X = vX,
                    Y = vY,
                    Z = vZ,
                    W = 1
                }
            };
            nui.SkeletonEngine.SkeletonToDepthImage(joint.Position, out outX, out outY);
        }

        private void calcPositions()
        {
            
            //左肩 (origPosition)
            origPosition = skeleton.Joints[JointID.ShoulderRight].Position;

            //左肩->右肩のベクトルをとる (vectorX)
            Vector right = skeleton.Joints[JointID.ShoulderLeft].Position;
            Vector centerTop = skeleton.Joints[JointID.ShoulderCenter].Position;
            Vector centerBottom = skeleton.Joints[JointID.Spine].Position;

            vectorX = new Vector();
            vectorX.X = right.X - origPosition.X;
            vectorX.Y = right.Y - origPosition.Y;
            vectorX.Z = right.Z - origPosition.Z;

            vectorY = new Vector();
            vectorY.X = centerBottom.X - centerTop.X;
            vectorY.Y = centerBottom.Y - centerTop.Y;
            vectorY.Z = centerBottom.Z - centerTop.Z;
        }

    }
}
