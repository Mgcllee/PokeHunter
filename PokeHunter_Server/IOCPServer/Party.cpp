#include "Party.h"

Party::Party()
	: member_count(0)
	, in_stage(false)
	, name("Empty")
{

}

Party::~Party()
{

}

bool Party::new_member(Player& new_mem)
{
	if (member_count >= PARTY_MAX_NUM)
		return false;

	ll.lock();
	member.push_back(new_mem);
	++member_count;
	ll.unlock();

	return true;
}

bool Party::leave_member(char* memname) {
	if (member_count <= 0)
		return false;

	ll.lock();
	for (Player& mem : member)
	{
		if (0 == strcmp(mem.get_user_name(), memname))
		{
			member.remove(mem);
			if (0 == --member_count)
				in_stage = false;

			ll.unlock();
			return true;
		}
	}

	ll.unlock();
	return false;
}
bool Party::get_party_in_stage()
{
	return in_stage;
}
short Party::get_member_count()
{
	return member_count;
}
