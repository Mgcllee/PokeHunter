using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Windows.Forms;

namespace winform_dummy_client
{



    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct CS_CHAT_TEXT_PACK
    {
        public char size;
        public char type;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)] public char[] content;


        public byte[] Serialize()
        {
            var buffer = new byte[Marshal.SizeOf(typeof(CS_CHAT_TEXT_PACK))];

            var gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            var pBuffer = gch.AddrOfPinnedObject();

            Marshal.StructureToPtr(this, pBuffer, false);
            gch.Free();

            return buffer;
        }
    };

    public class network_module
    {
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        static TcpClient tc = new TcpClient();
        NetworkStream stream;

        public void Connect()
        {
            tc = new TcpClient("127.0.0.1", 7777);
            stream = tc.GetStream();
        }

        #region
        // private static byte[] StructToBytes(object obj)
        private static byte[] StructToBytes(CS_CHAT_TEXT_PACK obj)
        {
            int iSize = Marshal.SizeOf(obj);
            byte[] arr = new byte[iSize];

            // IntPtr ptr = Marshal.AllocHGlobal(iSize);
            // Marshal.StructureToPtr(obj, ptr, false);
            // Marshal.Copy(ptr, arr, 0, iSize);
            // Marshal.FreeHGlobal(ptr);

            IntPtr ptr = IntPtr.Zero;
            try
            {
                ptr = Marshal.AllocHGlobal(iSize);
                Marshal.StructureToPtr(obj, ptr, true);
                Marshal.Copy(ptr, arr, 0, iSize);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
            return arr;

            return arr;
        }
        #endregion

        public void send_msg(string msg)
        {
            CS_CHAT_TEXT_PACK ctp = new CS_CHAT_TEXT_PACK();
            

            ctp.size = (char)(StructToBytes(new CS_CHAT_TEXT_PACK()).Length);
            ctp.type = (char)99; // CS_CHAT_TEXT;
            ctp.content = msg.ToCharArray();

            byte[] buff = ctp.Serialize();

            stream.Write(buff, 0, buff.Length);

            byte[] outbuf = new byte[1024];
            int nbytes = stream.Read(outbuf, 0, outbuf.Length);
            string output = Encoding.ASCII.GetString(outbuf, 0, nbytes);
        }

        void Close()
        {
            // (5) 스트림과 TcpClient 객체 닫기
            stream.Close();
            tc.Close();
        }

    }
}
