using System;
using System.Runtime.InteropServices;

namespace Seeta.AIP
{
    public class Engine
    {
        private DynamicLibrary _dynamic;
        private Package _package;

        public static int AIP_VERSION = 2;

        public Engine(string libName)
        {
            _dynamic = new DynamicLibrary(libName);
            Unmanaged.seeta_aip_load load =
                (Unmanaged.seeta_aip_load)_dynamic.Invoke(
                    "seeta_aip_load", typeof(Unmanaged.seeta_aip_load));
            Unmanaged.Package package = new Unmanaged.Package();
            int errCode = load(ref package, (uint)Marshal.SizeOf(typeof(Unmanaged.Package)));
            switch ((Unmanaged.LoadError)errCode)
            {
                default:
                    throw new Exception("Unknown load error.");
                case 0:
                    break;
                case Unmanaged.LoadError.SizeNotEnough:
                    throw new Exception((int)Unmanaged.LoadError.SizeNotEnough, "Size not enough.");
                case Unmanaged.LoadError.UnhandledInternalError:
                    throw new Exception((int)Unmanaged.LoadError.SizeNotEnough, "Unhandled internal error.");
                case Unmanaged.LoadError.AipVersionMismatch:
                    throw new Exception((int)Unmanaged.LoadError.SizeNotEnough, "AIP version mismatch.");
            }
            if (package.aip_version != AIP_VERSION) throw new Exception("AIP version mismatch.");
            _package = new Package(package);
        }

        ~Engine()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (_dynamic == null) return;
            _dynamic.Dispose();
            _package = null;
            _dynamic = null;
        }
        
        public Package AIP
        {
            get
            {
                return _package;
            }
        }
    }

    public class Instance
    {
        private Engine _engine;
        private bool _engineBorrowed;
        private Package _package;
        private IntPtr _aip;

        public Instance(string libName, Device device, string[] models, Object[] args = null)
        {
            _engine = new Engine(libName);
            _engineBorrowed = false;
            this.Construct(_engine.AIP, device, models, args);
        }
        
        public Instance(Engine engine, Device device, string[] models, Object[] args = null)
        {
            _engine = engine;
            _engineBorrowed = true;
            this.Construct(_engine.AIP, device, models, args);
        }

        public Instance(Package aip, Device device, string[] models, Object[] args = null)
        {
            this.Construct(aip, device, models, args);
        }

        ~Instance()
        {
            Dispose();
        }

        private void Construct(Package aip, Device device, string[] models, Object[] args = null)
        {
            _package = aip;
            _aip = _package.Create(device, models, args);
        }

        public void Dispose()
        {
            if (_aip == IntPtr.Zero) return;
            _package.Free(_aip);
            _aip = IntPtr.Zero;
            if (_engine != null && !_engineBorrowed) _engine.Dispose();
        }

        public void Reset()
        {
            _package.Reset(_aip);
        }

        public string Error(int errCode)
        {
            return _package.Error(_aip, errCode);
        }

        public string[] Property()
        {
            return _package.Property(_aip);
        }

        public void SetD(string name, double value)
        {
            _package.SetD(_aip, name, value);
        }

        public void Set(string name, double value)
        {
            SetD(name, value);
        }

        public double GetD(string name)
        {
            return _package.GetD(_aip, name);
        }

        public void Set(string name, Object value)
        {
            _package.Set(_aip, name, value);
        }

        public Object Get(string name)
        {
            return _package.Get(_aip, name);
        }

        public string Tag(uint methodId, uint labelIndex, int labelValue)
        {
            return _package.Tag(_aip, methodId, labelIndex, labelValue);
        }

        public Package.Result Forward(uint methodId, ImageData[] images, Object[] objects)
        {
            return _package.Forward(_aip, methodId, images, objects);
        }
    }
}