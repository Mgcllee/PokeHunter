using System.Collections.Generic;

namespace PLAYER
{
    static class PLAYERS
    {
        static public Dictionary<string, Player> players;
    }

    class Player
    {
        public string _p_name
        { 
            get { return _p_name; }
            private set { _p_name = value; }
        }

        public Player(string new_name) 
        {
            _p_name = new_name;

        }
    }
}
