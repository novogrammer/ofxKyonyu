using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Microsoft.Research.Kinect.Nui;
using System.Windows.Controls;
using System.Linq;
using System.Drawing;
using System.Drawing.Imaging;

using System.Windows.Media.Media3D;

namespace KinectKyonyu
{
	public partial class MainWindow : Window
	{

        Person person1;
        Person person2;

        Runtime nui;
        int imgWidth = 640;
        int imgHeight = 480;


        byte[] currentDepthData = null;

		public MainWindow()
		{
            InitializeComponent();

            person1 = new Person(oppaiLeftModel1, oppaiRightModel1, materialImageLeft1, materialImageRight1, oppaiLeftRoot1, oppaiRightRoot1);
            person1.oppaiRight = new Oppai(oppaiRightModel1, Oppai.RIGHT);
            person1.oppaiLeft = new Oppai(oppaiLeftModel1, Oppai.LEFT);

            person2 = new Person(oppaiLeftModel2, oppaiRightModel2, materialImageLeft2, materialImageRight2, oppaiLeftRoot2, oppaiRightRoot2);
            person2.oppaiRight = new Oppai(oppaiRightModel2, Oppai.RIGHT);
            person2.oppaiLeft = new Oppai(oppaiLeftModel2, Oppai.LEFT);

			this.Loaded += new RoutedEventHandler(onLoad);
			this.Closed += new EventHandler(onClose);

		}

        byte[] latestVideoBytes = null;

		/// <summary>
		/// NUIの初期化
		/// </summary>
		private void onLoad(object sender, RoutedEventArgs e)
		{
			nui = new Runtime();
			try {
                nui.Initialize(RuntimeOptions.UseColor | RuntimeOptions.UseSkeletalTracking | RuntimeOptions.UseDepthAndPlayerIndex);


				nui.VideoStream.Open(ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color);
                
                nui.DepthStream.Open(    ImageStreamType.Depth,
                                        2, 
                                        ImageResolution.Resolution320x240, 
                                        ImageType.DepthAndPlayerIndex);
                var parameters = new TransformSmoothParameters
                {
                    Smoothing = 0.75f,
                    Correction = 0.0f,
                    Prediction = 0.0f,
                    JitterRadius = 0.05f,
                    MaxDeviationRadius = 0.04f
                };
                nui.SkeletonEngine.SmoothParameters = parameters;
                nui.SkeletonFrameReady += new EventHandler<SkeletonFrameReadyEventArgs>(nui_SkeletonFrameReady);
                //nui.DepthFrameReady += new EventHandler<ImageFrameReadyEventArgs>(nui_DepthFrameReady);
			}
			catch (InvalidOperationException) {
				System.Windows.MessageBox.Show("Error: InvalidOperationException");
				return;
			}


			nui.VideoFrameReady += new EventHandler<ImageFrameReadyEventArgs>(onVideoFrameReady);
		}

        void nui_DepthFrameReady(object sender, ImageFrameReadyEventArgs e)
        {
            currentDepthData = e.ImageFrame.Image.Bits;
            ImageFrame frm = e.ImageFrame;
            PlanarImage img = e.ImageFrame.Image;
            byte[] ColoredBytes = GenerateColorBytes(e.ImageFrame);

            image2.Source = BitmapSource.Create(img.Width, img.Height, 96, 96, PixelFormats.Bgr32, null,
                ColoredBytes, img.Width * PixelFormats.Bgr32.BitsPerPixel / 8);
            
            
        }
        private byte[] GenerateColorBytes(ImageFrame imageFrame)
        {
            int w = imageFrame.Image.Width;

            Byte[] depthData = imageFrame.Image.Bits;
            Byte[] colorFrame = new byte[imageFrame.Image.Height * imageFrame.Image.Width * 4];

            const int BlueIndex = 0;
            const int GreenIndex = 1;
            const int RedIndex = 2;

            var depthIndex = 0;
            int playerDepthMin = 0;
            int playerDepthMax = 0;
            int[] playerMap = new int[w*imageFrame.Image.Height];
            for (var y = 0; y < imageFrame.Image.Height; y++)
            {
                var heightOffset = y * w;
                for (var x = 0; x < w; x++)
                {
                    var mapIndex = y * w + x;
                    var index = ((w - x - 1) + heightOffset) * 4;

                    var distance = GetDistanceWithPlayerIndex(depthData[depthIndex], depthData[depthIndex + 1]);
                    int playerIndex = GetPlayerIndex(depthData[depthIndex]);
                    playerMap[mapIndex] = playerIndex;
                    byte dist = (byte)(Math.Min(255, (distance) * 0.1));
                    colorFrame[index + BlueIndex] = dist;
                    colorFrame[index + GreenIndex] = dist;
                    colorFrame[index + RedIndex] = dist;
                    if (Math.Abs(headX - x) < 5 && Math.Abs(headY - y) < 5)
                    {
                        colorFrame[index + BlueIndex] = (byte)0;
                        colorFrame[index + GreenIndex] = (byte)0;
                        colorFrame[index + RedIndex] = (byte)255;
                    }
                    if (playerIndex > 0)
                    {
                        if (playerDepthMin == 0 || playerDepthMin < distance) playerDepthMin = distance;
                        if (playerDepthMax == 0 || playerDepthMax > distance) playerDepthMax = distance;
                    }
                    depthIndex += 2;
                }
            }


            return colorFrame;
        }
        int GetDistanceWithPlayerIndex(byte firstFrame, byte secondFrame)
        {
            int distance = (int)(firstFrame >> 3 | secondFrame << 5);
            return distance;
        }

        private static int GetPlayerIndex(byte firstFrame)
        {
            return (int)firstFrame & 7;
        }

        void nui_SkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            SkeletonFrame allSkeletons = e.SkeletonFrame;
            person1.tracked = false;
            person2.tracked = false;
            int foundSkeletonCount = 0;
            for (int i = 0; i < allSkeletons.Skeletons.Count(); i++)
            {
                SkeletonData tmpSkeleton = allSkeletons.Skeletons[i];
                if (tmpSkeleton.TrackingState == SkeletonTrackingState.Tracked)
                {
                    //Trackされている skeleton発見
                    if (foundSkeletonCount == 0)
                    {
                        person1.setSkeletonId(tmpSkeleton.TrackingID);
                        person1.tracked = true;
                        person1.skeleton = tmpSkeleton;
                        foundSkeletonCount++;
                    }
                    else if (foundSkeletonCount == 1)
                    {
                        person2.setSkeletonId(tmpSkeleton.TrackingID);
                        person2.tracked = true;
                        person2.skeleton = tmpSkeleton;
                        foundSkeletonCount++;
                    }
                }
            }
            render(person1);
            render(person2);
        }

        private void render(Person person)
        {
            if (!person.tracked)
            {
                person.materialImageLeft.ImageSource = null;
                person.materialImageRight.ImageSource = null;
                return;
            }
            if (person.needRefresh)
            {
                //canvas3D.Visibility = System.Windows.Visibility.Visible;
                //TODO 可視/不可視きりかえ
            }
            SkeletonData skeleton = person.skeleton;

            Joint shoulderLeft = skeleton.Joints[JointID.ShoulderLeft];
            Joint shoulderRight = skeleton.Joints[JointID.ShoulderRight];
            Joint jointCenter = skeleton.Joints[JointID.Spine];
            Joint handLeft = skeleton.Joints[JointID.HandLeft];
            Joint handRight = skeleton.Joints[JointID.HandRight];
            Joint head = skeleton.Joints[JointID.Head];

            MyVector3D vectorRight = new MyVector3D(
                shoulderRight.Position.X - jointCenter.Position.X,
                shoulderRight.Position.Y - jointCenter.Position.Y,
                shoulderRight.Position.Z - jointCenter.Position.Z
                );
            MyVector3D vectorLeft = new MyVector3D(
                shoulderLeft.Position.X - jointCenter.Position.X,
                shoulderLeft.Position.Y - jointCenter.Position.Y,
                shoulderLeft.Position.Z - jointCenter.Position.Z
                );
            MyVector3D vertical = vectorRight.getOutorProduct(vectorLeft).normalize();


            drawBoobs(skeleton.Joints[JointID.ShoulderLeft], skeleton.Joints[JointID.ShoulderRight], skeleton.Joints[JointID.HipCenter], vertical, person);

            double leftAngle = Math.Atan2(handLeft.Position.Y - shoulderLeft.Position.Y, handLeft.Position.X - shoulderLeft.Position.X);
            double rightAngle = Math.Atan2(handRight.Position.Y - shoulderRight.Position.Y, handRight.Position.X - shoulderRight.Position.X);
            double degRight = rightAngle * 180.0 / Math.PI;
            double degLeft = leftAngle * 180.0 / Math.PI;

            //bool isOppaiBanzai = Math.Abs(degRight - 45) < 15 && Math.Abs(degLeft - 135) < 15;
            if (Math.Abs(vertical.getX()) < 0.15 && Math.Abs(vertical.getY()) < 0.15 && person.isTextureExpired())
            {
                BodyTexture texture = new BodyTexture(nui, skeleton, latestVideoBytes);
                person.materialImageLeft.ImageSource = texture.ImageSourceLeft;
                person.materialImageRight.ImageSource = texture.ImageSourceRight;
                person.initDone = true;
            }
            person.needRefresh = false;
        }

        private void drawBoobs(Joint leftShoulder, Joint rightShoulder, Joint spine, MyVector3D vertical, Person person)
        {


            float oppaiOffset = -0.075f;
            Joint jointLeftBoob = createJoint(
                (leftShoulder.Position.X + spine.Position.X) / 2f + oppaiOffset * (float)vertical.getX(),
                (leftShoulder.Position.Y + spine.Position.Y) / 2f + oppaiOffset * (float)vertical.getY(),
                (leftShoulder.Position.Z + spine.Position.Z) / 2f + oppaiOffset * (float)vertical.getZ()
                );
            Joint jointRightBoob = createJoint(
                (rightShoulder.Position.X + spine.Position.X) / 2f + oppaiOffset * (float)vertical.getX(),
                (rightShoulder.Position.Y + spine.Position.Y) / 2f + oppaiOffset * (float)vertical.getY(),
                (rightShoulder.Position.Z + spine.Position.Z) / 2f + oppaiOffset * (float)vertical.getZ()
                );

            if (leftShoulder.Position.Z > 0 && rightShoulder.Position.Z > 0)
            {
                if (jointLeftBoob.Position.Z > 0)
            {

                // 肩
                MyVector3D shoulderNormalized = new MyVector3D(
                    (rightShoulder.Position.X - leftShoulder.Position.X),
                    (rightShoulder.Position.Y - leftShoulder.Position.Y),
                    (rightShoulder.Position.Z - leftShoulder.Position.Z)
                    ).normalize();
                // 頭方向 (肩 x vert)
                MyVector3D headNormalized = vertical.getOutorProduct(shoulderNormalized);

                
                Matrix3D mat = new Matrix3D
                    (
                        -shoulderNormalized.getX(), -shoulderNormalized.getY(), shoulderNormalized.getZ(), 0,
                        -headNormalized.getX(), -headNormalized.getY(), headNormalized.getZ(), 0,
                        -vertical.getX(), -vertical.getY(), vertical.getZ(), 0,
                        0, 0, 0, 1
                    );

                Transform3DGroup group = new Transform3DGroup();
                //回転
                MatrixTransform3D rot = new MatrixTransform3D(mat);
                //移動
                System.Windows.Media.Media3D.Vector3D v =
                    new System.Windows.Media.Media3D.Vector3D(jointLeftBoob.Position.X * 10, jointLeftBoob.Position.Y * 10, -jointLeftBoob.Position.Z * 10);
                person.oppaiLeft.setPosition(leftShoulder.Position);
                person.oppaiRight.setPosition(rightShoulder.Position);
                person.oppaiLeft.move(mat);
                person.oppaiRight.move(mat);
                // 変更を適用
                Transform3D translate = new TranslateTransform3D(v);
                group.Children.Add(rot);
                group.Children.Add(translate);
                person.oppaiRootLeft.Transform = group;
                person.oppaiRootRight.Transform = group;
            }
            }
        }

        int headX = 0;
        int headY = 0;


        private void setPosition(System.Windows.Controls.Control button, Joint joint)
        {
            setPosition(button, joint, 0, 0);
        }
        private void setPosition(System.Windows.Controls.Control button, Joint joint, int offsetX, int offsetY)
        {
            Joint scaled = ScaleToImg(joint, imgWidth, imgHeight, 1.0f, 1.0f);
            button.Margin = new Thickness(offsetX + scaled.Position.X - button.Width / 2, offsetY + scaled.Position.Y - button.Height / 2, 0, 0);
        }
        private void setPosition(System.Windows.Shapes.Ellipse button, Joint joint)
        {
            Joint scaled = ScaleToImg(joint, imgWidth, imgHeight, 1.0f, 1.0f);
            button.Margin = new Thickness( scaled.Position.X - button.Width / 2,  scaled.Position.Y - button.Height / 2, 0, 0);
        }
        private Joint createJoint(float x, float y, float z)
        {
            return new Joint()
            {
                Position = new Microsoft.Research.Kinect.Nui.Vector()
                {
                    X = x,
                    Y = y,
                    Z = z,
                    W = 1
                }
            };
        }
        private  Joint ScaleToImg(Joint joint, int width, int height, float skeletonMaxX, float skeletonMaxY)
        {
            //NuiTransformSkeletonToDepthImageF
            float x = 100; ;
            float y = 100;
            nui.SkeletonEngine.SkeletonToDepthImage(joint.Position, out x, out y);
            Microsoft.Research.Kinect.Nui.Vector pos = new Microsoft.Research.Kinect.Nui.Vector()
            {
                X = x * imgWidth,
                Y = y * imgHeight,
                Z = joint.Position.Z,
                W = joint.Position.W
            };

            Joint j = new Joint()
            {
                ID = joint.ID,
                TrackingState = joint.TrackingState,
                Position = pos
            };

            return j;
        }
        private static float Scale(int maxPixel, float maxSkeleton, float position, float Z)
        {
            if (Z == 0) return 0;
            float value = ((((maxPixel / maxSkeleton) / 2) * position / Z) + (maxPixel / 2));
            if (value > maxPixel)
                return maxPixel;
            if (value < 0)
                return 0;
            return value;
        }

		/// <summary>
		/// 各フレームでKinectのビデオフレームをUIに表示します
		/// </summary>
		private void onVideoFrameReady(object sender, ImageFrameReadyEventArgs e)
		{
			// 32bit RGBA
            PlanarImage img = e.ImageFrame.Image;
            BitmapSource source = BitmapSource.Create(
                img.Width, img.Height, 96, 96, PixelFormats.Bgr32, null, img.Bits, img.Width * img.BytesPerPixel);
            video.Source = source;
            latestVideoBytes = img.Bits;
		}

		/// <summary>
		/// Kinectのランタイムを終了します
		/// </summary>
		private void onClose(object sender, EventArgs e)
		{
			nui.Uninitialize();
		}

        private void button1_Click(object sender, RoutedEventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {

        }

        private void button3_Click(object sender, RoutedEventArgs e)
        {

        }

    }
}
