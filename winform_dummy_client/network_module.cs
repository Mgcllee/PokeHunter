using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Windows.Forms;
using static System.Net.WebRequestMethods;
using Protocol;
using static Protocol.PACKET_TYPE;
using PLAYER;
using System.Collections.Generic;
using static PLAYER.PLAYERS;
using System.Drawing.Drawing2D;

namespace winform_dummy_client
{
    public class network_module
    {
        static TcpClient tc = new TcpClient();
        NetworkStream stream;

        public string _name { get; private set; }
        public byte _player_skin { get; private set; }
        public byte _pet_num{ get; private set; }

        public string new_chat = "";

        public network_module()
        {
            _name = "";
        }
        private byte[] Serialize(Object m_datapacket)
        {
            int datasize = Marshal.SizeOf(m_datapacket);
            IntPtr buffer = Marshal.AllocHGlobal(datasize);
            Marshal.StructureToPtr(m_datapacket, buffer, false);
            byte[] RawData = new byte[datasize];
            Marshal.Copy(buffer, RawData, 0, datasize);
            Marshal.FreeHGlobal(buffer);

            return RawData;
        }
        private object ByteToObject(byte[] buffer, object packet_type)
        {
            try
            {
                GCHandle gcHandle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
                var ret = Marshal.PtrToStructure(gcHandle.AddrOfPinnedObject(), packet_type.GetType());
                gcHandle.Free();
                return ret;
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception.ToString());
            }
            return null;
        }

        public bool Connect(string my_name = "")
        {
            if (my_name == "") 
                return false;

            _name = my_name;

            tc = new TcpClient("127.0.0.1", RULE.PORT_NUM);
            stream = tc.GetStream();

            CS_LOGIN_PACK log = new CS_LOGIN_PACK();
            log.size = (byte)Serialize(log).Length;
            log.type = CS_LOGIN;
            log.Token = "dummy_client_" + _name;

            byte[] buff = Serialize(log);
            stream.Write(buff, 0, buff.Length);

            byte[] recv_buffer = new byte[RULE.PACK_SIZE];
            int nbytes = stream.Read(recv_buffer, 0, recv_buffer.Length);
            byte[] recv_pack = new byte[nbytes];
            Array.Copy(recv_buffer, recv_pack, recv_pack.Length);

            if (nbytes != 0)
            {
                GCHandle gcHandle = GCHandle.Alloc(recv_pack, GCHandleType.Pinned);
                SC_LOGIN_INFO_PACK packet = (SC_LOGIN_INFO_PACK)Marshal.PtrToStructure(gcHandle.AddrOfPinnedObject(), typeof(SC_LOGIN_INFO_PACK));
                gcHandle.Free();
                _player_skin = packet._player_skin;
                _pet_num = packet._pet_num;
                return true;
            }
            else
            {
                Disconnect();
                return false;
            }
        }

        public void Disconnect()
        {
            stream.Close();
            tc.Close();
        }

        

        public void send_msg(string msg)
        {
            CS_CHAT_PACK ctp = new CS_CHAT_PACK();
            ctp.size = (byte)Serialize(ctp).Length;
            ctp.type = CS_CHAT;
            ctp.content = msg;
            byte[] buff = Serialize(ctp);
            stream.Write(buff, 0, buff.Length);
        }

        public byte recv_packet()
        {
            byte[] recv_buffer = new byte[RULE.PACK_SIZE];
            int recv_byte_cnt = stream.Read(recv_buffer, 0, recv_buffer.Length);

            if (recv_byte_cnt == 0)
                return 0;

            byte[] recv_pack = new byte[recv_buffer[0]];
            Array.Copy(recv_buffer, recv_pack, recv_pack.Length);

            switch (recv_pack[1])
            {
                case SC_LOGIN_INFO:
                    {
                        // is not my player info
                        SC_LOGIN_INFO_PACK packet = (SC_LOGIN_INFO_PACK)ByteToObject(recv_pack, new SC_LOGIN_INFO_PACK());
                        if(packet != null)
                            players.Add(packet.name, new Player(packet.name));
                    }
                    break;
                case SC_CHAT:
                    {
                        SC_CHAT_PACK packet = (SC_CHAT_PACK)ByteToObject(recv_pack, new SC_CHAT_PACK());
                        if(packet != null)
                            new_chat = packet.content;
                    }
                    break;
            }

            return recv_pack[1];
        }


        void Close()
        {
            stream.Close();
            tc.Close();
        }

    }
}
