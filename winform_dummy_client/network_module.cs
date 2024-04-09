using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Windows.Forms;
using static System.Net.WebRequestMethods;

namespace winform_dummy_client
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct CS_CHAT_TEXT_PACK
    {
        public char size;
        public char type;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 60)]
        public string content;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    struct CS_LOGIN_PACK
    {
        public char size;
        public char type;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 60)]
        public string Token;

        public char Token_size;
    };

    public class network_module
    {
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        static TcpClient tc = new TcpClient();
        NetworkStream stream;
        public string name = "";

        public void Connect()
        {
            tc = new TcpClient("127.0.0.1", 7777);
            stream = tc.GetStream();
        }

        public static byte[] Serialize(Object m_datapacket)
        {
            int datasize = Marshal.SizeOf(m_datapacket);
            IntPtr buffer = Marshal.AllocHGlobal(datasize);
            Marshal.StructureToPtr(m_datapacket, buffer, false);
            byte[] RawData = new byte[datasize];
            Marshal.Copy(buffer, RawData, 0, datasize);
            Marshal.FreeHGlobal(buffer);

            return RawData;
        }

        public void send_name()
        {
            CS_LOGIN_PACK log = new CS_LOGIN_PACK();
            log.size = (char)Serialize(log).Length;
            log.type = (char)0; // CS_LOGIN;
            log.Token = "dummy_client_" + name;
            
            byte[] buff = Serialize(log);
            stream.Write(buff, 0, buff.Length);
        }

        public void send_msg(string msg)
        {
            CS_CHAT_TEXT_PACK ctp = new CS_CHAT_TEXT_PACK();
            ctp.size = (char)Serialize(ctp).Length;
            ctp.type = (char)99; // CS_CHAT_TEXT;
            ctp.content = msg;

            byte[] buff = Serialize(ctp);

            stream.Write(buff, 0, buff.Length);

            /*byte[] outbuf = new byte[1024];
            int nbytes = stream.Read(outbuf, 0, outbuf.Length);
            string output = Encoding.ASCII.GetString(outbuf, 0, nbytes);*/
        }

        void Close()
        {
            // (5) 스트림과 TcpClient 객체 닫기
            stream.Close();
            tc.Close();
        }

    }
}
