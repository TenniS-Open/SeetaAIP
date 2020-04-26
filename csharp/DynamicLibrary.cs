using System.Runtime.InteropServices;
using System;

namespace Seeta.AIP
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
        private string _libName;

        public DynamicLibrary(string libName)
        {
            _hModule = IntPtr.Zero;
            _hModule = LoadLibrary(libName);
            if (_hModule == IntPtr.Zero)
            {
                throw new DllNotFoundException(
                    "Unable to load shared library '" + libName + "' or one of its dependencies.");
            }

            _libName = libName;
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
                throw new EntryPointNotFoundException(
                    "Unable to find entry pointer '" + funcName + "' on '" + _libName + "'.");
            }
            return (Delegate)Marshal.GetDelegateForFunctionPointer(hFunProc, type);
        }
        
        /*
        public T Invoke<T>(string funcName) where T : Delegate
        {
            IntPtr hFunProc = GetProcAddress(_hModule, funcName);
            if (hFunProc == IntPtr.Zero)
            {
                throw new EntryPointNotFoundException(
                    "Unable to find entry pointer '" + funcName + "' on '" + _libName + "'.");
            }
            return (T)Marshal.GetDelegateForFunctionPointer(hFunProc, typeof(T));
        }
        */
    }
}