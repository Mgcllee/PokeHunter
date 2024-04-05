using System;
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

        public static byte[] StructureToByte(CS_CHAT_TEXT_PACK obj)
        {
            int datasize = Marshal.SizeOf(obj);//((PACKET_DATA)obj).TotalBytes; // 구조체에 할당된 메모리의 크기를 구한다.
            IntPtr buff = Marshal.AllocHGlobal(datasize); // 비관리 메모리 영역에 구조체 크기만큼의 메모리를 할당한다.
            Marshal.StructureToPtr(obj, buff, false); // 할당된 구조체 객체의 주소를 구한다.
            byte[] data = new byte[datasize]; // 구조체가 복사될 배열
            Marshal.Copy(buff, data, 0, datasize); // 구조체 객체를 배열에 복사
            Marshal.FreeHGlobal(buff); // 비관리 메모리 영역에 할당했던 메모리를 해제함
            return data; // 배열을 리턴
        }

        public void send_msg(string msg)
        {
            CS_CHAT_TEXT_PACK ctp = new CS_CHAT_TEXT_PACK();
            ctp.size = (char)(char)StructureToByte(new CS_CHAT_TEXT_PACK()).Length;
            ctp.type = (char)99; // CS_CHAT_TEXT;
            ctp.content = msg.ToCharArray();

            BinaryFormatter formatter = new BinaryFormatter();
            CS_CHAT_TEXT_PACK buf = (CS_CHAT_TEXT_PACK)formatter.Deserialize(stream);

            byte[] buff = StructureToByte(ctp);
            
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
