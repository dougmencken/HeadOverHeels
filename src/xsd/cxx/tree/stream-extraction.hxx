// file      : xsd/cxx/tree/stream-extraction.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_STREAM_EXTRACTION_HXX
#define XSD_CXX_TREE_STREAM_EXTRACTION_HXX

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/tree/istream.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // type
      //
      template <typename S>
      inline _type::
      _type (istream<S>&, flags, type* container)
          : container_ (container)
      {
      }

      // simple_type
      //
      template <typename B>
      template <typename S>
      inline simple_type<B>::
      simple_type (istream<S>& s, flags f, type* container)
          : type (s, f, container)
      {
      }

      // fundamental_base
      //
      template <typename X, typename C, typename B>
      template <typename S>
      inline fundamental_base<X, C, B>::
      fundamental_base (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        X& r (*this);
        s >> r;
      }

      // list
      //
      template <typename X, typename C>
      template <typename S>
      list<X, C, false>::
      list (istream<S>& s, flags f, type* container)
          : sequence<X> (f, container)
      {
        std::size_t size;
        istream_common::as_size<std::size_t> as_size (size);
        s >> as_size;

        if (size > 0)
        {
          this->reserve (size);

          while (size--)
          {
            std::auto_ptr<X> p (new X (s, f, container));
            push_back (p);
          }
        }
      }

      template <typename X, typename C>
      template <typename S>
      list<X, C, true>::
      list (istream<S>& s, flags f, type* container)
          : sequence<X> (f, container)
      {
        std::size_t size;
        istream_common::as_size<std::size_t> as_size (size);
        s >> as_size;

        if (size > 0)
        {
          this->reserve (size);

          while (size--)
          {
            X x;
            s >> x;
            push_back (x);
          }
        }
      }

      // Extraction operators for built-in types
      //


      // string
      //
      template <typename C, typename B>
      template <typename S>
      inline string<C, B>::
      string (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // normalized_string
      //
      template <typename C, typename B>
      template <typename S>
      inline normalized_string<C, B>::
      normalized_string (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // token
      //
      template <typename C, typename B>
      template <typename S>
      inline token<C, B>::
      token (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // nmtoken
      //
      template <typename C, typename B>
      template <typename S>
      inline nmtoken<C, B>::
      nmtoken (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // nmtokens
      //
      template <typename C, typename B, typename nmtoken>
      template <typename S>
      inline nmtokens<C, B, nmtoken>::
      nmtokens (istream<S>& s, flags f, type* container)
          : B (s, f, container), base_type (s, f, container)
      {
      }


      // name
      //
      template <typename C, typename B>
      template <typename S>
      inline name<C, B>::
      name (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // ncname
      //
      template <typename C, typename B>
      template <typename S>
      inline ncname<C, B>::
      ncname (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // language
      //
      template <typename C, typename B>
      template <typename S>
      inline language<C, B>::
      language (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // id
      //
      template <typename C, typename B>
      template <typename S>
      inline id<C, B>::
      id (istream<S>& s, flags f, type* container)
          : B (s, f, container), identity_ (*this)
      {
        register_id ();
      }


      // idref
      //
      template <typename X, typename C, typename B>
      template <typename S>
      inline idref<X, C, B>::
      idref (istream<S>& s, flags f, type* container)
          : B (s, f, container), identity_ (*this)
      {
      }


      // idrefs
      //
      template <typename C, typename B, typename idref>
      template <typename S>
      inline idrefs<C, B, idref>::
      idrefs (istream<S>& s, flags f, type* container)
          : B (s, f, container), base_type (s, f, container)
      {
      }


      // uri
      //
      template <typename C, typename B>
      template <typename S>
      inline uri<C, B>::
      uri (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // qname
      //
      template <typename C, typename B, typename uri, typename ncname>
      template <typename S>
      inline qname<C, B, uri, ncname>::
      qname (istream<S>& s, flags f, type* container)
          : B (s, f, container), ns_ (s), name_ (s)
      {
      }


      // base64_binary
      //
      template <typename C, typename B>
      template <typename S>
      inline base64_binary<C, B>::
      base64_binary (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        buffer<C>& r (*this);
        s >> r;
      }


      // hex_binary
      //
      template <typename C, typename B>
      template <typename S>
      inline hex_binary<C, B>::
      hex_binary (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        buffer<C>& r (*this);
        s >> r;
      }


      // date
      //
      template <typename C, typename B>
      template <typename S>
      inline date<C, B>::
      date (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // date_time
      //
      template <typename C, typename B>
      template <typename S>
      inline date_time<C, B>::
      date_time (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // duration
      //
      template <typename C, typename B>
      template <typename S>
      inline duration<C, B>::
      duration (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // day
      //
      template <typename C, typename B>
      template <typename S>
      inline day<C, B>::
      day (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // month
      //
      template <typename C, typename B>
      template <typename S>
      inline month<C, B>::
      month (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // month_day
      //
      template <typename C, typename B>
      template <typename S>
      inline month_day<C, B>::
      month_day (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // year
      //
      template <typename C, typename B>
      template <typename S>
      inline year<C, B>::
      year (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // year_month
      //
      template <typename C, typename B>
      template <typename S>
      inline year_month<C, B>::
      year_month (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // time
      //
      template <typename C, typename B>
      template <typename S>
      inline time<C, B>::
      time (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // entity
      //
      template <typename C, typename B>
      template <typename S>
      inline entity<C, B>::
      entity (istream<S>& s, flags f, type* container)
          : B (s, f, container)
      {
      }


      // entities
      //
      template <typename C, typename B, typename entity>
      template <typename S>
      inline entities<C, B, entity>::
      entities (istream<S>& s, flags f, type* container)
          : B (s, f, container), base_type (s, f, container)
      {
      }
    }
  }
}

#endif  // XSD_CXX_TREE_STREAM_EXTRACTION_HXX
