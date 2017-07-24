// generated by modules/sqlpp11/scripts/ddl2cpp db/tables.sql.tmp rest/src/database/Tables tables
#ifndef TABLES_TABLES_H
#define TABLES_TABLES_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace tables
{
  namespace Users_
  {
    struct Userid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "userid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userid;
            T& operator()() { return userid; }
            const T& operator()() const { return userid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct Name
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Email
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "email";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T email;
            T& operator()() { return email; }
            const T& operator()() const { return email; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Institute
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "institute";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T institute;
            T& operator()() { return institute; }
            const T& operator()() const { return institute; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Passwd
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "passwd";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T passwd;
            T& operator()() { return passwd; }
            const T& operator()() const { return passwd; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Admin
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "admin";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T admin;
            T& operator()() { return admin; }
            const T& operator()() const { return admin; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::require_insert>;
    };
  }

  struct Users: sqlpp::table_t<Users,
               Users_::Userid,
               Users_::Name,
               Users_::Email,
               Users_::Institute,
               Users_::Passwd,
               Users_::Admin>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "users";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T users;
        T& operator()() { return users; }
        const T& operator()() const { return users; }
      };
    };
  };
  namespace Books_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Year
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "year";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T year;
            T& operator()() { return year; }
            const T& operator()() const { return year; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Title
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "title";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T title;
            T& operator()() { return title; }
            const T& operator()() const { return title; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Author
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "author";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T author;
            T& operator()() { return author; }
            const T& operator()() const { return author; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Description
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "description";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T description;
            T& operator()() { return description; }
            const T& operator()() const { return description; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Uri
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "uri";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T uri;
            T& operator()() { return uri; }
            const T& operator()() const { return uri; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Directory
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "directory";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T directory;
            T& operator()() { return directory; }
            const T& operator()() const { return directory; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Lang
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lang";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lang;
            T& operator()() { return lang; }
            const T& operator()() const { return lang; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  }

  struct Books: sqlpp::table_t<Books,
               Books_::Bookid,
               Books_::Year,
               Books_::Title,
               Books_::Author,
               Books_::Description,
               Books_::Uri,
               Books_::Directory,
               Books_::Lang>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "books";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T books;
        T& operator()() { return books; }
        const T& operator()() const { return books; }
      };
    };
  };
  namespace Projects_
  {
    struct Projectid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "projectid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T projectid;
            T& operator()() { return projectid; }
            const T& operator()() const { return projectid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct Origin
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "origin";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T origin;
            T& operator()() { return origin; }
            const T& operator()() const { return origin; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Owner
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "owner";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T owner;
            T& operator()() { return owner; }
            const T& operator()() const { return owner; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pages
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pages";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pages;
            T& operator()() { return pages; }
            const T& operator()() const { return pages; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  }

  struct Projects: sqlpp::table_t<Projects,
               Projects_::Projectid,
               Projects_::Origin,
               Projects_::Owner,
               Projects_::Pages>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "projects";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T projects;
        T& operator()() { return projects; }
        const T& operator()() const { return projects; }
      };
    };
  };
  namespace ProjectPages_
  {
    struct Projectid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "projectid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T projectid;
            T& operator()() { return projectid; }
            const T& operator()() const { return projectid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Pageid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pageid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pageid;
            T& operator()() { return pageid; }
            const T& operator()() const { return pageid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct ProjectPages: sqlpp::table_t<ProjectPages,
               ProjectPages_::Projectid,
               ProjectPages_::Pageid>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "project_pages";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T projectPages;
        T& operator()() { return projectPages; }
        const T& operator()() const { return projectPages; }
      };
    };
  };
  namespace Pages_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pageid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pageid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pageid;
            T& operator()() { return pageid; }
            const T& operator()() const { return pageid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Imagepath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "imagepath";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T imagepath;
            T& operator()() { return imagepath; }
            const T& operator()() const { return imagepath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Ocrpath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ocrpath";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ocrpath;
            T& operator()() { return ocrpath; }
            const T& operator()() const { return ocrpath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Filetype
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "filetype";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T filetype;
            T& operator()() { return filetype; }
            const T& operator()() const { return filetype; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pleft
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pleft";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pleft;
            T& operator()() { return pleft; }
            const T& operator()() const { return pleft; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Ptop
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ptop";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ptop;
            T& operator()() { return ptop; }
            const T& operator()() const { return ptop; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pright
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pright";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pright;
            T& operator()() { return pright; }
            const T& operator()() const { return pright; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pbottom
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pbottom";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pbottom;
            T& operator()() { return pbottom; }
            const T& operator()() const { return pbottom; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  }

  struct Pages: sqlpp::table_t<Pages,
               Pages_::Bookid,
               Pages_::Pageid,
               Pages_::Imagepath,
               Pages_::Ocrpath,
               Pages_::Filetype,
               Pages_::Pleft,
               Pages_::Ptop,
               Pages_::Pright,
               Pages_::Pbottom>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "pages";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T pages;
        T& operator()() { return pages; }
        const T& operator()() const { return pages; }
      };
    };
  };
  namespace Textlines_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pageid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pageid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pageid;
            T& operator()() { return pageid; }
            const T& operator()() const { return pageid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Lineid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lineid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lineid;
            T& operator()() { return lineid; }
            const T& operator()() const { return lineid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Imagepath
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "imagepath";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T imagepath;
            T& operator()() { return imagepath; }
            const T& operator()() const { return imagepath; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Lleft
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lleft";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lleft;
            T& operator()() { return lleft; }
            const T& operator()() const { return lleft; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Ltop
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ltop";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ltop;
            T& operator()() { return ltop; }
            const T& operator()() const { return ltop; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Lright
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lright";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lright;
            T& operator()() { return lright; }
            const T& operator()() const { return lright; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Lbottom
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lbottom";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lbottom;
            T& operator()() { return lbottom; }
            const T& operator()() const { return lbottom; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  }

  struct Textlines: sqlpp::table_t<Textlines,
               Textlines_::Bookid,
               Textlines_::Pageid,
               Textlines_::Lineid,
               Textlines_::Imagepath,
               Textlines_::Lleft,
               Textlines_::Ltop,
               Textlines_::Lright,
               Textlines_::Lbottom>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "textlines";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T textlines;
        T& operator()() { return textlines; }
        const T& operator()() const { return textlines; }
      };
    };
  };
  namespace Contents_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Pageid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pageid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pageid;
            T& operator()() { return pageid; }
            const T& operator()() const { return pageid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Lineid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lineid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lineid;
            T& operator()() { return lineid; }
            const T& operator()() const { return lineid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Seq
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seq";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seq;
            T& operator()() { return seq; }
            const T& operator()() const { return seq; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Ocr
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ocr";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ocr;
            T& operator()() { return ocr; }
            const T& operator()() const { return ocr; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Cor
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "cor";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T cor;
            T& operator()() { return cor; }
            const T& operator()() const { return cor; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Cut
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "cut";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T cut;
            T& operator()() { return cut; }
            const T& operator()() const { return cut; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Conf
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "conf";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T conf;
            T& operator()() { return conf; }
            const T& operator()() const { return conf; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::require_insert>;
    };
  }

  struct Contents: sqlpp::table_t<Contents,
               Contents_::Bookid,
               Contents_::Pageid,
               Contents_::Lineid,
               Contents_::Seq,
               Contents_::Ocr,
               Contents_::Cor,
               Contents_::Cut,
               Contents_::Conf>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "contents";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T contents;
        T& operator()() { return contents; }
        const T& operator()() const { return contents; }
      };
    };
  };
  namespace Profiles_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Timestamp
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "timestamp";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T timestamp;
            T& operator()() { return timestamp; }
            const T& operator()() const { return timestamp; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
  }

  struct Profiles: sqlpp::table_t<Profiles,
               Profiles_::Bookid,
               Profiles_::Timestamp>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "profiles";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T profiles;
        T& operator()() { return profiles; }
        const T& operator()() const { return profiles; }
      };
    };
  };
  namespace Errortokens_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Errortokenid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "errortokenid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T errortokenid;
            T& operator()() { return errortokenid; }
            const T& operator()() const { return errortokenid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Errortoken
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "errortoken";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T errortoken;
            T& operator()() { return errortoken; }
            const T& operator()() const { return errortoken; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  }

  struct Errortokens: sqlpp::table_t<Errortokens,
               Errortokens_::Bookid,
               Errortokens_::Errortokenid,
               Errortokens_::Errortoken>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "errortokens";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T errortokens;
        T& operator()() { return errortokens; }
        const T& operator()() const { return errortokens; }
      };
    };
  };
  namespace Suggestions_
  {
    struct Bookid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bookid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bookid;
            T& operator()() { return bookid; }
            const T& operator()() const { return bookid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Errortokenid
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "errortokenid";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T errortokenid;
            T& operator()() { return errortokenid; }
            const T& operator()() const { return errortokenid; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Suggestion
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "suggestion";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T suggestion;
            T& operator()() { return suggestion; }
            const T& operator()() const { return suggestion; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Weight
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "weight";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T weight;
            T& operator()() { return weight; }
            const T& operator()() const { return weight; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::require_insert>;
    };
    struct Distance
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "distance";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T distance;
            T& operator()() { return distance; }
            const T& operator()() const { return distance; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct Suggestions: sqlpp::table_t<Suggestions,
               Suggestions_::Bookid,
               Suggestions_::Errortokenid,
               Suggestions_::Suggestion,
               Suggestions_::Weight,
               Suggestions_::Distance>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "suggestions";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T suggestions;
        T& operator()() { return suggestions; }
        const T& operator()() const { return suggestions; }
      };
    };
  };
}
#endif
