#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SessionStoreTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <thread>
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/SessionStore.hpp"

using namespace pcw;

struct SessionStoreFixture {
	SessionStore session_store;
	UserSptr user;

	SessionStoreFixture()
	    : session_store(),
	      user(std::make_shared<User>("name", "pass", "email", "inst",
					  42)) {}
};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(SessionStoreTest, SessionStoreFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(Size) {
	BOOST_CHECK(session_store.empty());
	session_store.new_session(*user, nullptr, Config::empty());
	BOOST_CHECK_EQUAL(session_store.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(NewSession) {
	auto session =
	    session_store.new_session(*user, nullptr, Config::empty());
	BOOST_REQUIRE(session);
	BOOST_CHECK(not session->id().empty());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FindSession) {
	auto session =
	    session_store.new_session(*user, nullptr, Config::empty());
	BOOST_REQUIRE(session);
	BOOST_CHECK_EQUAL(session, session_store.find_session(session->id()));
	BOOST_CHECK(not session_store.find_session("invalid session id"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(FindMultipleSessions) {
	auto s1 = session_store.new_session(*user, nullptr, Config::empty());
	auto s2 = session_store.new_session(*user, nullptr, Config::empty());
	auto s3 = session_store.new_session(*user, nullptr, Config::empty());
	BOOST_REQUIRE(s1);
	BOOST_REQUIRE(s2);
	BOOST_REQUIRE(s3);
	BOOST_CHECK_EQUAL(s1, session_store.find_session(s1->id()));
	BOOST_CHECK_EQUAL(s2, session_store.find_session(s2->id()));
	BOOST_CHECK_EQUAL(s3, session_store.find_session(s3->id()));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(SessionStoreClears) {
	using namespace std::literals::chrono_literals;
	for (auto i = 0U; i < (2 * SessionStore::THRESHOLD); ++i) {
		auto session =
		    session_store.new_session(*user, nullptr, Config::empty());
		BOOST_CHECK(session_store.size() <= SessionStore::THRESHOLD);
		BOOST_CHECK(session != nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(IsThreadSave) {
	std::vector<std::thread> threads(10);
	for (auto& thread : threads) {
		thread = std::thread([&]() {
			for (size_t i = 0; i < 10; ++i) {
				auto s = session_store.new_session(
				    *user, nullptr, Config::empty());
				BOOST_CHECK_EQUAL(
				    s, session_store.find_session(s->id()));
			}
		});
	}
	for (auto& thread : threads) thread.join();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
