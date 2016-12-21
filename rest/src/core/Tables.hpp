#ifndef pcw_Tables_hpp__
#define pcw_Tables_hpp__

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>

#define FOO(s, n, t) \
	struct s {\
		struct _alias_t {\
			static constexpr const char _literal[] = "n";\
			using _name_t = sqlpp::make_char_sequence<\
				sizeof(_literal), _literal>;\
			template<class T> struct _member_t {\
				T n;\
				T& operator()() noexcept\
					{return n;}\
				const T& operator()() const noexcept\
					{return n;}\
			};\
		};\
		using _traits = ::sqlpp::make_traits<t>;\
	};

namespace pcw {
	namespace Tables {
		namespace Users {
			struct Struct {
				std::string name; /*, email,
					institute, passwd;*/
				int userid;
			};
			struct Userid {
				struct _alias_t {
					static constexpr const char _literal[] = "userid";
					using _name_t = sqlpp::make_char_sequence<
						sizeof(_literal), _literal>;
					template<class T> struct _member_t {
						T userid;
						T& operator()() noexcept
							{return userid;}
						const T& operator()() const noexcept
							{return userid;}
					};
				};
				using _traits = ::sqlpp::make_traits<::sqlpp::bigint>;
				template<typename T> struct accesssor_t {
					static decltype(T::userid)& get(T& t) {return t.userid;}
					static const decltype(T::userid)& get(const T& t) {return t.userid;}
				};
			};

			struct Name {
				struct _alias_t {
					static constexpr const char _literal[] =  "name";
					using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
					template<typename T> struct _member_t {
						T beta;
						T& operator()() { return beta; }
						const T& operator()() const { return beta; }
					};
				};
				using _traits = ::sqlpp::make_traits<::sqlpp::varchar>;
				template<typename T> struct accesssor_t {
					static decltype(T::name)& get(T& t) {return t.name;}
					static const decltype(T::name)& get(const T& t) {return t.name;}
				};
			};
			struct Table: sqlpp::table_t<Table, Userid, Name> {
				using _value_t = sqlpp::no_value_t;
				struct alias_t {
					static constexpr const char _literal[] =  "users";
					using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
					template<class T> struct member_t {
						T table;
						T& operator()() noexcept
							{return table;}
						const T& operator()() const noexcept
							{return table;}
					};
				};
			};
		}
	}
}

/*
drop table if exists users;
create table users (
       userid int not null unique primary key auto_increment,
       name varchar(50) not null unique,
       email varchar(50) not null,
       institute varchar(50) not null,
       passwd varchar(100) not null
);
*/

#endif // pcw_Tables_hpp__

