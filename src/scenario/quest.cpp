#include "quest.hpp"

// CHECK_EQ macro for equality comparisons
#ifndef CHECK_EQ
	#define CHECK_EQ(other, field) if (this->field != other.field) return false
#endif

bool cQuest::operator==(const cQuest& other) {
	CHECK_EQ(other, deadline_is_relative);
	CHECK_EQ(other, auto_start);
	CHECK_EQ(other, deadline);
	CHECK_EQ(other, event);
	CHECK_EQ(other, xp);
	CHECK_EQ(other, gold);
	CHECK_EQ(other, bank1);
	CHECK_EQ(other, bank2);
	CHECK_EQ(other, name);
	CHECK_EQ(other, descr);
	return true;
}