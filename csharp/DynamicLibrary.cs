using System.Runtime.InteropServices;
using System;

namespace com.seetatech.aip
{
    public class DynamicLibrary
    {
        [DllImport("kernel32")]
        private static extern IntPtr LoadLibrary(string path);
        [DllImport("kernel32")]
        private static extern IntPtr GetProcAddress(IntPtr lib, string funcName);
        [DllImport("kernel32")]
        private static extern bool FreeLibrary(IntPtr lib);
        
        private IntPtr _hModule;

        public DynamicLibrary(string libName)
        {
            _hModule = IntPtr.Zero;
            _hModule = LoadLibrary(libName);
            if (_hModule == IntPtr.Zero)
            {
                throw new Exception("Can not open library：" + libName + ".");
            }
        }

        ~DynamicLibrary()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (_hModule == IntPtr.Zero) return;
            FreeLibrary(_hModule);
            _hModule = IntPtr.Zero;
        }
        
        public Delegate Invoke(string funcName, Type type)
        {
            IntPtr hFunProc = GetProcAddress(_hModule, funcName);
            if (hFunProc == IntPtr.Zero)
            {
                throw new Exception("Can not find entry：" + funcName + ".");
            }
            return (Delegate)Marshal.GetDelegateForFunctionPointer(hFunProc, type);
        }
        
        public T Invoke<T>(string funcName) where T : Delegate
        {
            IntPtr hFunProc = GetProcAddress(_hModule, funcName);
            if (hFunProc == IntPtr.Zero)
            {
                throw new Exception("Can not find entry：" + funcName + ".");
            }
            return (T)Marshal.GetDelegateForFunctionPointer(hFunProc, typeof(T));
        }
    }
}