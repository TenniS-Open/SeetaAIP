using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using ImageFormat = Seeta.AIP.Unmanaged.ImageFormat;
using ValueType = Seeta.AIP.Unmanaged.ValueType;

namespace Seeta.AIP
{
    public class Utils
    {
        public static ImageData ToImageData(Bitmap bitmap)
        {
            ImageData image = new ImageData(ImageFormat.U8Bgr, 1, (uint) bitmap.Height, (uint) bitmap.Width, 3);
            // TODO: make sure lock bits is BGR format, default is BGR by now
            BitmapData data = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height),
                ImageLockMode.ReadOnly,
                PixelFormat.Format24bppRgb);
            long src = data.Scan0.ToInt64();
            byte[] dst = image.Data<byte>();

            for (int h = 0; h < bitmap.Height; ++h)
            {
                Marshal.Copy(
                    new IntPtr(src + h * data.Stride),
                    dst,
                    h * bitmap.Width * 3,
                    bitmap.Width * 3);
            }

            bitmap.UnlockBits(data);
            return image;
        }
    }
}