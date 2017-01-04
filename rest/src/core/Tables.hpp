#ifndef pcw_Tables_hpp__
#define pcw_Tables_hpp__

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>

#define table_column(name, tname, type) \
	struct name { \
		struct _alias_t { \
			static constexpr const char _literal[] = #name; \
			using _name_t = sqlpp::make_char_sequence< \
				sizeof(_literal), _literal>; \
			template<class T> struct _member_t { \
				T tname; \
				T& operator()() noexcept \
					{return tname;} \
				const T& operator()() const noexcept \
					{return tname;} \
			}; \
		}; \
		using _traits = ::sqlpp::make_traits<::sqlpp::type>; \
		template<typename T> struct accesssor_t { \
			static decltype(T::tname)& get(T& t) {return t.tname;} \
			static const decltype(T::tname)& get(const T& t) {return t.tname;} \
		}; \
	};

namespace pcw {
	namespace Tables {
		namespace Users {
			struct Struct {
				std::string name; /*, email,
					institute, passwd;*/
				int userid;
			};
			table_column(UserId, userid, bigint);
			table_column(Name, name, varchar);
			struct Table: sqlpp::table_t<Table, UserId, Name> {
				using _value_t = sqlpp::no_value_t;
				struct _alias_t {
					static constexpr const char _literal[] =  "users";
					using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
					template<class T> struct _member_t {
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

