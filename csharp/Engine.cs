using System;
using System.Runtime.InteropServices;

namespace Seeta.AIP
{
    public class Engine
    {
        private DynamicLibrary _dynamic;
        private Package _package;

        public Engine(string libName)
        {
            _dynamic = new DynamicLibrary(libName);
            Unmanaged.seeta_aip_load load = _dynamic.Invoke<Unmanaged.seeta_aip_load>("seeta_aip_load");
            Unmanaged.Package package = new Unmanaged.Package();
            load(ref package, (uint)Marshal.SizeOf(typeof(Unmanaged.Package)));
            _package = new Package(package);
        }

        ~Engine()
        {
            Dispose();
        }

        void Dispose()
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
        private Package _package;
        private IntPtr _aip;

        public Instance(string libName, Device device, string[] models)
        {
            _engine = new Engine(libName);
            this.Construct(_engine.AIP, device, models);
        }
        
        public Instance(Engine engine, Device device, string[] models)
        {
            _engine = engine;
            this.Construct(_engine.AIP, device, models);
        }

        public Instance(Package aip, Device device, string[] models)
        {
            this.Construct(aip, device, models);
        }

        ~Instance()
        {
            Dispose();
        }

        private void Construct(Package aip, Device device, string[] models)
        {
            _package = aip;
            _aip = _package.Create(device, models);
        }

        public void Dispose()
        {
            if (_aip == IntPtr.Zero) return;
            _package.Free(_aip);
            _aip = IntPtr.Zero;
        }

        public void Reset()
        {
            _package.Reset(_aip);
        }

        public string Error(int errCode)
        {
            return _package.Error(_aip, errCode);
        }

        public int[] Property()
        {
            return _package.Property(_aip);
        }

        public void Set(int propertyId, double value)
        {
            _package.Set(_aip, propertyId, value);
        }

        public double Get(int propertyId)
        {
            return _package.Get(_aip, propertyId);
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