using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Net.Mime;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Threading;

namespace Seeta.AIP
{
    using Point = Unmanaged.Point;
    using ShapeType = Unmanaged.ShapeType;
    using ValueType = Unmanaged.ValueType;
    using Tag = Unmanaged.Object.Tag;

    namespace Unmanaged
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Point
        {
            public float x;
            public float y;
        }

        public enum ShapeType
        {
            Unknown = 0,
            Points = 1,
            Lines = 2,
            Rectangle = 3,
            Parallelogram = 4,
            Polygon = 5,
            Circle = 6,
            Cube = 7,
            NoShape = 255,
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Shape
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct Landmarks
            {
                public IntPtr data; // Point *
                public uint size;
            }

            public int type;
            public Landmarks landmarks;
            public float rotate;
            public float scale;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Object
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct Tag
            {
                public int label;
                public float score;
            }

            [StructLayout(LayoutKind.Sequential)]
            public struct TagArray
            {
                public IntPtr data; // Point *
                public uint size;
            }

            [StructLayout(LayoutKind.Sequential)]
            public struct Extra
            {
                [StructLayout(LayoutKind.Sequential)]
                public struct Dims
                {
                    public IntPtr data; // uint32_t *
                    public uint size;
                }

                public int type;
                public IntPtr data; // void *
                public Dims dims;
            }

            public Shape shape;
            public TagArray tags;
            public Extra extra;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Device
        {
            public IntPtr device; // const char *
            public int id;
        }

        public enum ValueType
        {
            Byte = 0,
            Float = 1,
            Int = 2,
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ImageData
        {
            public int type;
            public IntPtr data; // void *
            public uint number;
            public uint height;
            public uint width;
            public uint channels;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate IntPtr seeta_aip_error(IntPtr aip, int errcode);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_create([Out] IntPtr paip, IntPtr device, IntPtr models);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_free(IntPtr aip);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_reset(IntPtr aip);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_forward(IntPtr aip, uint method_id,
            IntPtr images, uint images_size,
            IntPtr objects, uint objects_size,
            [Out] IntPtr result_objects, [Out] IntPtr result_objects_size,
            [Out] IntPtr result_images, [Out] IntPtr result_image_size);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_property(IntPtr aip, [Out] IntPtr property);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_set(IntPtr aip, int property_id, double value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_get(IntPtr aip, int property_id, ref double value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate IntPtr seeta_aip_tag(IntPtr aip, uint method_id, uint label_index, int label_value);

        [StructLayout(LayoutKind.Sequential)]
        public struct Package
        {
            [MarshalAs(UnmanagedType.I4)] public int aip_version;

            public IntPtr module; // const char *

            public IntPtr description; // const char *

            public IntPtr mID; // const char *

            public IntPtr sID; // const char *

            public IntPtr version; // const char *

            public IntPtr support; // const char **

            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_error error;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_create create;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_free free;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_property property;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_get get;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_set set;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_reset reset;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_forward forward;
            [MarshalAs(UnmanagedType.FunctionPtr)] public seeta_aip_tag tag;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int seeta_aip_load(ref Package package, uint size);
    }

    public class Device
    {
        public string device;
        public int id;

        private IntPtr _device_tmp;

        public Device(string device, int id = 0)
        {
            this.device = device;
            this.id = id;
            this._device_tmp = IntPtr.Zero;
        }

        ~Device()
        {
            if (this._device_tmp != IntPtr.Zero) Marshal.FreeHGlobal(_device_tmp);
        }

        public Unmanaged.Device Raw
        {
            get
            {
                if (this._device_tmp != IntPtr.Zero) Marshal.FreeHGlobal(_device_tmp);
                _device_tmp = Marshal.StringToHGlobalAnsi(device);
                
                Unmanaged.Device raw = new Unmanaged.Device();
                raw.device = _device_tmp;
                raw.id = id;
                return raw;
            }
        }
    }

    public class Exception : System.Exception
    {
        public Exception(int errorCode) : base(_Message(errorCode))
        {
        }

        public Exception(int errorCode, string message) : base(_Message(errorCode, message))
        {
        }

        public Exception(string message) : base(_Message(message))
        {
        }

        public static string _Message(int errorCode)
        {
            return String.Format("(0x{0:x8})", errorCode);
        }

        public static string _Message(string message)
        {
            if (message == null)
            {
                return "";
            }

            return String.Format("{0}", message);
        }

        public static string _Message(int errorCode, string message)
        {
            if (message == null)
            {
                return String.Format("(0x{0:x8})", errorCode);
                ;
            }

            return String.Format("(0x{0:x8}): {1}", errorCode, message);
        }
    }

    public class ImageData
    {
        private ValueType type;
        private float[] data_float;
        private byte[] data_byte;
        private int[] data_int;
        private uint number;
        private uint height;
        private uint width;
        private uint channels;

        public ImageData(ValueType type, uint number, uint height, uint width, uint channnls)
        {
            this.type = type;
            this.number = number;
            this.height = height;
            this.width = width;
            this.channels = channnls;

            uint count = number * height * width * channnls;

            switch (type)
            {
                case ValueType.Byte:
                    this.data_byte = new byte[count];
                    break;
                case ValueType.Int:
                    this.data_int = new int[count];
                    break;
                case ValueType.Float:
                    this.data_float = new float[count];
                    break;
            }
        }

        public ImageData(Unmanaged.ImageData image)
            : this((ValueType)image.type, image.number, image.height, image.width, image.channels)
        {
            CopyFrom(image.data);
        }

        public ValueType Type
        {
            get { return type; }
        }

        public uint Number
        {
            get { return number; }
        }

        public uint Height
        {
            get { return height; }
        }

        public uint Width
        {
            get { return width; }
        }

        public uint Channels
        {
            get { return channels; }
        }

        private struct GetData<T>
        {
            public delegate T[] Getter(ImageData obj);

            public static Getter Get;

            static T[] DefaultGetter(ImageData obj)
            {
                return null;
            }

            static GetData()
            {
                Get = DefaultGetter;
            }
        }

        static byte[] GetDataByte(ImageData obj)
        {
            return obj.data_byte;
        }

        static int[] GetDataInt(ImageData obj)
        {
            return obj.data_int;
        }

        static float[] GetDataFloat(ImageData obj)
        {
            return obj.data_float;
        }

        static ImageData()
        {
            GetData<byte>.Get = GetDataByte;
            GetData<int>.Get = GetDataInt;
            GetData<float>.Get = GetDataFloat;
        }
        
        public T[] Data<T>() where T: struct
        {
            return GetData<T>.Get(this);
        }

        public IntPtr Data()
        {
            switch (type)
            {
                default: return IntPtr.Zero;
                case ValueType.Byte: return data_byte == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_byte, 0);
                case ValueType.Int: return data_int == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_int, 0);
                case ValueType.Float: return data_float == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_float, 0);
            }
        }

        public void CopyFrom(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero) return;

            uint count = number * height * width * channels;
            switch (type)
            {
                default: return;
                case ValueType.Byte:
                    Marshal.Copy(ptr, data_byte, 0, (int)count);
                    break;
                case ValueType.Int:
                    Marshal.Copy(ptr, data_int, 0, (int)count);
                    break;
                case ValueType.Float:
                    Marshal.Copy(ptr, data_float, 0, (int)count);
                    break;
            }
        }

        public Unmanaged.ImageData Raw
        {
            get
            {
                Unmanaged.ImageData raw = new Unmanaged.ImageData();
                raw.type = (int) this.type;
                raw.data = this.Data();
                raw.number = this.number;
                raw.height = this.height;
                raw.width = this.width;
                raw.channels = this.channels;
                return raw;
            }
        }
    }

    public class Shape
    {
        public ShapeType type;
        public Point[] landmarks;
        public float rotate;
        public float scale;

        public Shape()
        {
            landmarks = new Point[0];
        }

        public Shape(Unmanaged.Shape shape)
        {
            landmarks = Package.ToArrayWithSize<Point>(shape.landmarks.data, shape.landmarks.size);
            type = (ShapeType)shape.type;
            rotate = shape.rotate;
            scale = shape.scale;
        }

        private static Unmanaged.Shape.Landmarks ToLandmarks(Point[] landmarks)
        {
            Unmanaged.Shape.Landmarks raw = new Unmanaged.Shape.Landmarks();
            raw.data = landmarks == null ? IntPtr.Zero : Marshal.UnsafeAddrOfPinnedArrayElement(landmarks, 0);
            raw.size = landmarks == null ? 0 : (uint) landmarks.Length;
            return raw;
        }

        public Unmanaged.Shape Raw
        {
            get
            {
                Unmanaged.Shape raw = new Unmanaged.Shape();
                raw.type = (int)this.type;
                raw.landmarks = ToLandmarks(this.landmarks);
                raw.rotate = this.rotate;
                raw.scale = this.scale;
                return raw;
            }
        }
    }
    
    public class Tensor {
        private ValueType type;
        private float[] data_float;
        private byte[] data_byte;
        private int[] data_int;
        private uint[] dims;

        public Tensor()
        {
            type = ValueType.Byte;
        }

        public Tensor(ValueType type, uint []dims)
        {
            this.type = type;
            this.dims = dims;

            if (dims == null) return;

            uint count = 1;
            for (int i = 0; i < dims.Length; ++i)
            {
                count *= dims[i];
            }

            switch (type)
            {
                case ValueType.Byte:
                    this.data_byte = new byte[count];
                    break;
                case ValueType.Int:
                    this.data_int = new int[count];
                    break;
                case ValueType.Float:
                    this.data_float = new float[count];
                    break;
            }
        }

        public Tensor(Unmanaged.Object.Extra tensor)
            : this((ValueType)tensor.type, 
                Package.ToArrayWithSize<uint>(tensor.dims.data, tensor.dims.size))
        {
            CopyFrom(tensor.data);            
        }

        public ValueType Type
        {
            get { return type; }
        }

        public uint[] Dims
        {
            get { return dims; }
        }

        private struct GetData<T>
        {
            public delegate T[] Getter(Tensor obj);

            public static Getter Get;

            static T[] DefaultGetter(Tensor obj)
            {
                return null;
            }

            static GetData()
            {
                Get = DefaultGetter;
            }
        }

        static byte[] GetDataByte(Tensor obj)
        {
            return obj.data_byte;
        }

        static int[] GetDataInt(Tensor obj)
        {
            return obj.data_int;
        }

        static float[] GetDataFloat(Tensor obj)
        {
            return obj.data_float;
        }

        static Tensor()
        {
            GetData<byte>.Get = GetDataByte;
            GetData<int>.Get = GetDataInt;
            GetData<float>.Get = GetDataFloat;
        }
        
        public T[] Data<T>() where T: struct
        {
            return GetData<T>.Get(this);
        }

        public IntPtr Data()
        {
            switch (type)
            {
                default: return IntPtr.Zero;
                case ValueType.Byte: return data_byte == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_byte, 0);
                case ValueType.Int: return data_int == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_int, 0);
                case ValueType.Float: return data_float == null ? IntPtr.Zero : 
                    Marshal.UnsafeAddrOfPinnedArrayElement(data_float, 0);
            }
        }

        public void CopyFrom(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero || this.Empty()) return;
            
            uint count = 1;
            for (int i = 0; i < dims.Length; ++i)
            {
                count *= dims[i];
            }
            switch (type)
            {
                default: return;
                case ValueType.Byte:
                    Marshal.Copy(ptr, data_byte, 0, (int)count);
                    break;
                case ValueType.Int:
                    Marshal.Copy(ptr, data_int, 0, (int)count);
                    break;
                case ValueType.Float:
                    Marshal.Copy(ptr, data_float, 0, (int)count);
                    break;
            }
        }
        
        private static Unmanaged.Object.Extra.Dims ToDims(uint[] dims)
        {
            Unmanaged.Object.Extra.Dims raw = new Unmanaged.Object.Extra.Dims();
            raw.data = dims == null ? IntPtr.Zero : Marshal.UnsafeAddrOfPinnedArrayElement(dims, 0);
            raw.size = dims == null ? 0 : (uint) dims.Length;
            return raw;
        }

        public Unmanaged.Object.Extra Raw
        {
            get
            {
                Unmanaged.Object.Extra raw = new Unmanaged.Object.Extra();
                raw.type = (int) this.type;
                raw.data = this.Data();
                raw.dims = ToDims(this.dims);
                return raw;
            }
        }

        public bool Empty()
        {
            return (data_byte == null && data_int == null && data_float == null) || dims == null;
        }
    }
    
    public class Object
    {
        public Shape shape;    // must be set
        public Tag[] tags;    // must be set
        public Tensor extra;    // can be null, or empty tensor

        public Object()
        {
            shape = new Shape();
            tags = new Tag[0];
            extra = new Tensor();
        }

        public Object(Unmanaged.Object obj)
        {
            shape = new Shape(obj.shape);
            extra = new Tensor(obj.extra);
            tags = Package.ToArrayWithSize<Tag>(obj.tags.data, obj.tags.size);
            if (tags == null) tags = new Tag[0];
        }

        public Unmanaged.Object Raw
        {
            get
            {
                Unmanaged.Object raw = new Unmanaged.Object();
                raw.shape = shape.Raw;
                raw.tags = new Unmanaged.Object.TagArray();
                raw.tags.data = Marshal.UnsafeAddrOfPinnedArrayElement(tags, 0);
                raw.tags.size = (uint) tags.Length;
                if (extra == null)
                {
                    extra = new Tensor();
                }
                raw.extra = extra.Raw;
                return raw;
            }
        }
    }

    public class Package
    {
        public int aip_version;
        public string module;
        public string description;
        public string mID;
        public string sID;
        public string version;
        public string[] support;

        private Unmanaged.Package mAIP;

        public static T[] ToArrayWithEndValue<T>(IntPtr array, T endValue)
        {
            if (array == IntPtr.Zero) return null;
            List<T> tmp = new List<T>();
            int index = 0;
            while (true)
            {
                object elementObject = Marshal.PtrToStructure(
                    new IntPtr(array.ToInt64() + index * Marshal.SizeOf(typeof(T))), typeof(T));
                if (elementObject == null) break;
                T element = (T) elementObject;
                if (element.Equals(endValue)) break;
                tmp.Add(element);
                index++;
            }

            return tmp.ToArray();
        }

        public static T[] ToArrayWithSize<T>(IntPtr array, uint size)
        {
            if (array == IntPtr.Zero) return null;
            List<T> tmp = new List<T>();
            for (uint i = 0; i < size; ++i)
            {
                object elementObject = Marshal.PtrToStructure(
                    new IntPtr(array.ToInt64() + i * Marshal.SizeOf(typeof(T))), typeof(T));
                if (elementObject == null) break;
                T element = (T) elementObject;
                tmp.Add(element);
            }
            return tmp.ToArray();
        }

        public static string[] ToString(IntPtr[] objects)
        {
            string[] tmp = new string[objects.Length];
            for (int i = 0; i < objects.Length; ++i)
            {
                tmp[i] = Marshal.PtrToStringAnsi(objects[i]);
            }

            return tmp;
        }

        public static string ToString(IntPtr obj, string ifNull = null)
        {
            if (obj == IntPtr.Zero) return ifNull;
            return Marshal.PtrToStringAnsi(obj);
        }

        public static int ToInt(IntPtr obj, int ifNull = 0)
        {
            if (obj == IntPtr.Zero) return ifNull;
            return Marshal.ReadInt32(obj);
        }

        public static uint ToUint(IntPtr obj, uint ifNull = 0)
        {
            if (obj == IntPtr.Zero) return ifNull;
            return (uint)Marshal.ReadInt32(obj);
        }

        public Package(Unmanaged.Package raw)
        {
            mAIP = raw;

            aip_version = raw.aip_version;
            module = Marshal.PtrToStringAnsi(raw.module);
            description = Marshal.PtrToStringAnsi(raw.description);
            mID = Marshal.PtrToStringAnsi(raw.mID);
            sID = Marshal.PtrToStringAnsi(raw.sID);
            version = Marshal.PtrToStringAnsi(raw.version);

            support = ToString(ToArrayWithEndValue<IntPtr>(raw.support, IntPtr.Zero));
        }

        public string Error(IntPtr? aip, int errCode)
        {
            if (aip == null) aip = IntPtr.Zero;
            IntPtr errorMessage = mAIP.error(aip.Value, errCode);
            if (errorMessage == IntPtr.Zero) return "";
            return Marshal.PtrToStringAnsi(errorMessage);
        }

        private class UnmanagedStringList
        {
            private IntPtr[] _unmanaged;
            private int _size;
            public UnmanagedStringList(string[] models, bool appendNullEnd = true)
            {
                _size = models.Length;
                _unmanaged = new IntPtr[appendNullEnd ? _size + 1 : _size];
                for (int i = 0; i < _size; ++i)
                {
                    _unmanaged[i] = Marshal.StringToHGlobalAnsi(models[i]);
                }

                if (appendNullEnd)
                {
                    _unmanaged[_size] = IntPtr.Zero;
                }
            }

            ~UnmanagedStringList()
            {
                Dispose();
            }

            public void Dispose()
            {
                if (_unmanaged == null) return;
                for (int i = 0; i < _size; ++i)
                {
                    Marshal.FreeHGlobal(_unmanaged[i]);
                }
                _unmanaged = null;
            }

            public IntPtr[] Unmanaged
            {
                get
                {
                    return _unmanaged;
                }
            }
        }

        public IntPtr Create(Device device, string[] models)
        {
            if (models == null) models = new string[0];
            Unmanaged.Device[] rawDevice = {device.Raw};
            IntPtr[] rawAip = new IntPtr[1];
            UnmanagedStringList rawModelList = new UnmanagedStringList(models, true);
            IntPtr[] rawModels = rawModelList.Unmanaged;
            int errCode = mAIP.create(
                Marshal.UnsafeAddrOfPinnedArrayElement(rawAip, 0),
                Marshal.UnsafeAddrOfPinnedArrayElement(rawDevice, 0),
                Marshal.UnsafeAddrOfPinnedArrayElement(rawModels, 0)
            );
            rawModelList.Dispose();
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
            return rawAip[0];
        }

        public void Free(IntPtr aip)
        {
            int errCode = mAIP.free(aip);
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
        }

        public void Reset(IntPtr aip)
        {
            int errCode = mAIP.reset(aip);
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
        }

        public int[] Property(IntPtr aip)
        {
            IntPtr[] rawProperty = new IntPtr[1];
            int errCode = mAIP.property(aip,
                Marshal.UnsafeAddrOfPinnedArrayElement(rawProperty, 0));
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
            return ToArrayWithEndValue<int>(rawProperty[0], 0);
        }

        public void Set(IntPtr aip, int propertyId, double value)
        {
            int errCode = mAIP.set(aip, propertyId, value);
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
        }

        public double Get(IntPtr aip, int propertyId)
        {
            double value = 0;
            int errCode = mAIP.get(aip, propertyId, ref value);
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));
            return value;
        }

        public string Tag(IntPtr aip, uint methodId, uint labelIndex, int labelValue)
        {
            return ToString(mAIP.tag(aip, methodId, labelIndex, labelValue));
        }

        public struct Result
        {
            public ImageData[] images;
            public Object[] objects;
        }

        public Result Forward(IntPtr aip, uint methodId, ImageData[] images, Object[] objects)
        {
            if (images == null) images = new ImageData[0];
            if (objects == null) objects = new Object[0];
            Unmanaged.ImageData[] rawImages = new Unmanaged.ImageData[images.Length];
            for (int i = 0; i < images.Length; ++i) rawImages[i] = images[i].Raw;
            Unmanaged.Object[] rawObjects = new Unmanaged.Object[objects.Length];
            for (int i = 0; i < objects.Length; ++i) rawObjects[i] = objects[i].Raw;
            IntPtr[] rawOutputObjects = {IntPtr.Zero, IntPtr.Zero, };
            uint[] rawOutputSizes = {0, 0};

            int errCode = mAIP.forward(aip, methodId,
                Marshal.UnsafeAddrOfPinnedArrayElement(rawImages, 0),
                (uint) images.Length,
                Marshal.UnsafeAddrOfPinnedArrayElement(rawObjects, 0),
                (uint) objects.Length,
                Marshal.UnsafeAddrOfPinnedArrayElement(rawOutputObjects, 0),
                Marshal.UnsafeAddrOfPinnedArrayElement(rawOutputSizes, 0),
                Marshal.UnsafeAddrOfPinnedArrayElement(rawOutputObjects, 1),
                Marshal.UnsafeAddrOfPinnedArrayElement(rawOutputSizes, 1));
            if (errCode != 0) throw new Exception(errCode, Error(null, errCode));

            uint rawResultObjectsSize = rawOutputSizes[0];
            Unmanaged.Object[] rawResultObjects = ToArrayWithSize<Unmanaged.Object>(rawOutputObjects[0], rawResultObjectsSize);
            uint rawResultImagesSize = rawOutputSizes[1];
            Unmanaged.ImageData[] rawResultImages = ToArrayWithSize<Unmanaged.ImageData>(rawOutputObjects[1], rawResultImagesSize);

            Result result;
            result.images = new ImageData[rawResultImagesSize];
            result.objects = new Object[rawResultObjectsSize];
            for (int i = 0; i < rawResultImagesSize; ++i) result.images[i] = new ImageData(rawResultImages[i]);
            for (int i = 0; i < rawResultObjectsSize; ++i) result.objects[i] = new Object(rawResultObjects[i]);

            return result;
        }
    }
}