using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Media.Media3D;

namespace KinectKyonyu
{
    class Oppai
    {

        public static  int LEFT = 1;
        public static int RIGHT = 2;
        private System.Windows.Media.Media3D.MeshGeometry3D boobs;

        private static int stepTheta = 10;
        private static int stepPhi = 5;
        private static double absRadius = 0.95; //おわんの半径
        private static double zRadius = 0.9; //おわんの半径(Z)
        private static double nippleDistance = 1.8; //乳頭のX座標の隔たり
        private static int phiStartIndex = -1; //おわん型よりも大きく

        float ratioX = 0.2f;
        float ratioY = 0.4f;

        // 原点からの移動
        public Vector3D delta;
        public Vector3D deltaNormalized;
        // バネ定数
        private static double springK = 0.5f;
        private static double mass = 100000f;
        public int position;

        AccelInfoSequence accelInfoSeq;

        public Oppai(System.Windows.Media.Media3D.MeshGeometry3D boobs, int position)
        {
            accelInfoSeq = new AccelInfoSequence();
            delta = new Vector3D(0, 0, 0);
            this.position = position;

            this.boobs = boobs;
            isFirst = true;
            deltaNormalized = new Vector3D(0, 0, 1);
            renderModel();
        }

        private bool isFirst;

        private void renderModel()
        {

            float baseZ = -0.4f;

            int leftBottomIndex = 0;
            int topIndex = stepTheta * (stepPhi - phiStartIndex);
            boobs.Positions.Clear();

            for (int i = phiStartIndex; i < stepPhi; i++)
            {
                double phi = Math.PI * (double)i / (2.0 * (double)stepPhi);
                double baseX = (float)(Math.Sin(phi) * deltaNormalized.X) * ratioX
                    + ((position == RIGHT) ? nippleDistance / 2.0 : -nippleDistance / 2.0);
                float baseY = 0.2f + (float)(Math.Sin(phi) * deltaNormalized.Y * ratioY);
                double radius = absRadius * Math.Cos(phi);
                double offsetZ = zRadius * Math.Sin(phi) + baseZ;
                bool isTop = i == stepPhi - 1;
                for (int j = 0; j < stepTheta; j++)
                {

                    double theta = 2.0 * Math.PI * (double)j / (double)stepTheta;
                    double thetaNext = 2.0 * Math.PI * (double)((i + 1) % stepTheta) / (double)stepTheta;
                    double offsetX = radius * Math.Cos(theta) + baseX;
                    double offsetY = radius * Math.Sin(theta) + baseY;
                    double prevOffsetX = radius * Math.Cos(thetaNext);
                    double prevOffsetY = radius * Math.Sin(thetaNext);

                    //右乳、左乳それぞれに対して
                    //ポイントを追加
                    boobs.Positions.Add(new Point3D(offsetX, offsetY, offsetZ));
                    //テクスチャ座標指定
                    if (this.isFirst)
                    {
                        Point textureCoord = new Point((offsetX + 1.0) / 2.0, (offsetY + 1.0) / 2.0);
                        //Console.WriteLine(textureCoord);
                        boobs.TextureCoordinates.Add(textureCoord);
                    }
                    int rightBottomIndex = ((j == stepTheta - 1) ? leftBottomIndex + 1 - stepTheta : leftBottomIndex + 1);


                    if (this.isFirst)
                    {
                        if (isTop)
                        { //三角形のみ
                            boobs.TriangleIndices.Add(leftBottomIndex);
                            boobs.TriangleIndices.Add(rightBottomIndex);
                            boobs.TriangleIndices.Add(topIndex);

                        }
                        else
                        {
                            // 左下-右下-右上
                            boobs.TriangleIndices.Add(leftBottomIndex);
                            boobs.TriangleIndices.Add(rightBottomIndex);
                            boobs.TriangleIndices.Add(rightBottomIndex + stepTheta);
                            //左下-右上-左上
                            boobs.TriangleIndices.Add(leftBottomIndex);
                            boobs.TriangleIndices.Add(rightBottomIndex + stepTheta);
                            boobs.TriangleIndices.Add(leftBottomIndex + stepTheta);
                        }
                    }

                    leftBottomIndex++;

                }
            }
            double topX = (float)(deltaNormalized.X) * ratioX
                + ((position == RIGHT) ? nippleDistance / 2.0 : -nippleDistance / 2.0);
            float topY = 0.2f + (float)(deltaNormalized.Y) * ratioY;
            boobs.Positions.Add(new Point3D(topX, topY, zRadius + baseZ)); //頂点

            if (this.isFirst)
            {
                boobs.TextureCoordinates.Add(new Point((topX + 1.0) / 2.0, (topY + 1.0) / 2.0));
            }

            isFirst = false;
        }


        internal void setPosition(Microsoft.Research.Kinect.Nui.Vector vector)
        {
            accelInfoSeq.AddInfo(vector);
        }

        int i = 0;
        internal void move(Matrix3D rotationMatrix)
        {
            accelInfoSeq.rotate(rotationMatrix);
            AccelInfo accelInfo = accelInfoSeq.getLast();
            //計算
            Vector3D force = (-1.0 * springK * delta) +(mass * accelInfo.localAcceleration);
            accelInfo.localVelocity = accelInfo.localVelocity + (force * accelInfo.spanSec / mass);
            delta = delta + accelInfo.localVelocity * accelInfo.spanSec;
            deltaNormalized = new Vector3D(delta.X, delta.Y, delta.Z + 0.01);
            deltaNormalized.Normalize();
            
            if (i %5 == 0)
            {
                //Console.WriteLine("accel=" + accelInfo.localAcceleration+ " NORMAL=" + deltaNormalized);
            }
            i++;
            renderModel();
        }

        internal Vector3D getCurrentVelosity()
        {
            return accelInfoSeq.getLast().localAcceleration;
        }
    }
}
