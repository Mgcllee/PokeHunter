using System;
using System.Drawing;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;

namespace Protocol
{
    public struct RULE
    {
        public const int MAX_USER = 10;
        public const int MAX_PARTY = 100;
        public const int BUF_SIZE = 128;
        public const int PORT_NUM = 7777;

        // constexpr short CHAR_SIZE = 20;
        public const short CHAR_SIZE = 64;

        public const short PACK_SIZE = 128;
    }

    public struct PACKET_TYPE
    {
        public const byte CS_LOGIN = 0;
        public const byte CS_PARTY_SEARCHING = 2;
        public const byte CS_PARTY_INFO = 3;
        public const byte CS_PARTY_ENTER = 4;
        public const byte SC_PARTY_ENTER_OK = 5;
        public const byte CS_PARTY_READY = 6;
        public const byte CS_PARTY_LEAVE = 8;
        public const byte CS_QUEST_INVENTORY = 9;
        public const byte CS_SAVE_INVENTORY = 10;
        public const byte CS_LOGOUT = 11;
        public const byte CS_QUEST_STORAGE = 12;

        public const byte CS_CHAT = 98;
        public const byte SC_CHAT = 99;

        public const byte SC_FAIL = 20;
        public const byte SC_LOGIN_SUCCESS = 21;
        public const byte SC_LOGIN_INFO = 22;
        public const byte SC_PARTY_LIST_INFO = 23;
        public const byte SC_PARTY_INFO = 24;
        public const byte SC_PARTY_STAFF_READY = 25;
        public const byte SC_PARTY_START = 26;
        public const byte SC_PARTY_JOIN_FAIL = 27;
        public const byte SC_PARTY_JOIN_SUCCESS = 28;
        public const byte SC_PARTY_LEAVE_FAIL = 29;
        public const byte SC_PARTY_LEAVE_SUCCESS = 30;
        public const byte SC_LOGOUT_RESULT = 31;
        public const byte SC_ITEM_INFO = 33;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public class BASE_PACKET
    {
        public byte size;
        public byte type;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public class CS_CHAT_PACK : BASE_PACKET
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = RULE.CHAR_SIZE)]
        public string content;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public class SC_CHAT_PACK : BASE_PACKET
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = RULE.CHAR_SIZE)]
        public string content;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public class CS_LOGIN_PACK : BASE_PACKET
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = RULE.CHAR_SIZE)]
        public string Token;

        public byte Token_size;
    };

    public class SC_LOGIN_PACK : BASE_PACKET
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = RULE.CHAR_SIZE)]
        public string Token;

        public byte Token_size;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public class SC_LOGIN_INFO_PACK : BASE_PACKET
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = RULE.CHAR_SIZE)]
        public string name;
        public byte _player_skin;
        public byte _pet_num;
    };
}
