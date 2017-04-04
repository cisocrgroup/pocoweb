#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CacheTest

#include <atomic>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <random>
#include <thread>
#include "core/Cache.hpp"

using namespace pcw;

struct Mock {
	Mock(int i): id_(i) {}
	int id() const noexcept {return id_;}
	const int id_;
};

struct CacheFixture {
	Cache<Mock> cache;
	// use atomic here to enable testing with threads
	std::atomic<bool> generator_was_called;
	CacheFixture(): cache(2), generator_was_called(false) {}
	std::shared_ptr<Mock> make_mock(int id)
	{
		generator_was_called = true;
		return std::make_shared<Mock>(id);
	}
	void reset() noexcept
	{
		generator_was_called = false;
	}

};

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(CacheTest, CacheFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CacheMaxSize)
{
	BOOST_CHECK_EQUAL(cache.max_size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CacheSizeGrowsToMax)
{
	BOOST_CHECK_EQUAL(cache.size(), 0);
	cache.put(make_mock(1));
	BOOST_CHECK_EQUAL(cache.size(), 1);
	cache.put(make_mock(2));
	BOOST_CHECK_EQUAL(cache.size(), 2);
	cache.put(make_mock(3));
	BOOST_CHECK_EQUAL(cache.size(), 2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GeneratorFunctionIsCalled)
{
	auto mock = cache.get(13, [this](int id){return make_mock(id);});
	BOOST_CHECK(generator_was_called);
	BOOST_REQUIRE(mock);
	BOOST_CHECK_EQUAL(mock->id(), 13);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CacheGetInsertsOnlyOnce)
{
	BOOST_CHECK_EQUAL(cache.size(), 0);
	auto mock1 = cache.get(13, [this](int id){return make_mock(id);});
	BOOST_CHECK_EQUAL(cache.size(), 1);
	auto mock2 = cache.get(13, [this](int id){return make_mock(id);});
	BOOST_CHECK_EQUAL(cache.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(GeneratorFunctionIsCalledOnlyOnce)
{
	auto mock1 = make_mock(13);
	cache.put(mock1);
	BOOST_CHECK(generator_was_called);

	// reset and make sure that generator is not called
	reset();
	auto mock2 = cache.get(13, [this](int id){return make_mock(id);});
	BOOST_CHECK(not generator_was_called);
	BOOST_REQUIRE(mock1);
	BOOST_REQUIRE(mock2);
	BOOST_CHECK_EQUAL(mock2->id(), 13);
	BOOST_CHECK_EQUAL(mock1->id(), mock2->id());
	BOOST_CHECK_EQUAL(mock1, mock2);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(CacheIsThreadSave)
{
	std::mt19937 g;
	std::uniform_int_distribution<char> d(0, 9);
	std::vector<int> ids(100);
	std::generate(begin(ids), end(ids), [&](){return d(g);});
	std::vector<std::thread> threads(10);
	for (auto i = 0U; i < 10; ++i) {
		threads[i] = std::thread([=](){
			for (auto j = 0U; j < 10; ++j) {
				auto idx = (i * 10) + j;
				auto id = ids[idx];
				auto mock = cache.get(id, [&](int id) {
					return make_mock(id);
				});
				BOOST_CHECK_EQUAL(mock->id(), id);
			}
		});
	}
	for (auto& thread: threads)
		thread.join();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
