// file      : xsd/cxx/tree/std-ostream-operators.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX
#define XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX

#include <ostream>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/types.hxx>
#include <xsd/cxx/tree/list.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // type
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const type&)
      {
        return os;
      }


      // simple_type
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const simple_type<B>&)
      {
        return os;
      }


      // fundamental_base
      //
      template <typename X, typename C, typename B>
      inline
      std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, fundamental_base<X, C, B> x)
      {
        X& r (x);
        return os << r;
      }

      // optional: see containers.hxx
      //

      // list
      //

      // This is an xsd:list-style format (space-separated)
      //
      template <typename C, typename X, bool fund>
      std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const list<X, C, fund>& v)
      {
        for (typename list<X, C, fund>::const_iterator
               b (v.begin ()), e (v.end ()), i (b); i != e; ++i)
        {
          if (i != b)
            os << C (' ');

          os << *i;
        }

        return os;
      }


      // Operators for built-in types
      //


      // string
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const string<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // normalized_string
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const normalized_string<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // token
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const token<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // nmtoken
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const nmtoken<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // nmtokens
      //
      template <typename C, typename B, typename nmtoken>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const nmtokens<C, B, nmtoken>& v)
      {
        const list<nmtoken, C>& r (v);
        return os << r;
      }


      // name
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const name<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // ncname
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const ncname<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // language
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const language<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // id
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const id<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // idref
      //
      template <typename X, typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const idref<X, C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // idrefs
      //
      template <typename C, typename B, typename idref>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const idrefs<C, B, idref>& v)
      {
        const list<idref, C>& r (v);
        return os << r;
      }


      // uri
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const uri<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // qname
      //
      template <typename C, typename B, typename uri, typename ncname>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os,
                  const qname<C, B, uri, ncname>& n)
      {
        return os << n.namespace_ () << C ('#') << n.name ();
      }


      // base64_binary
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const base64_binary<C, B>& v)
      {
        return os << v.encode ();
      }


      // hex_binary
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const hex_binary<C, B>& v)
      {
        return os << v.encode ();
      }


      // date
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const date<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // date_time
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const date_time<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // duration
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const duration<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // day
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const day<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // month
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const month<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // month_day
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const month_day<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // year
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const year<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // year_month
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const year_month<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // time
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const time<C, B>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // entity
      //
      template <typename C, typename B>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const entity<C, B>& v)
      {
        const B& r (v);
        return os << r;
      }


      // entities
      //
      template <typename C, typename B, typename entity>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const entities<C, B, entity>& v)
      {
        const list<entity, C>& r (v);
        return os << r;
      }
    }
  }
}

#endif  // XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX
