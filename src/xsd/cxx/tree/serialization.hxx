// file      : xsd/cxx/tree/serialization.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_SERIALIZATION_HXX
#define XSD_CXX_TREE_SERIALIZATION_HXX

#include <limits> // std::numeric_limits
#include <string>
#include <sstream>

#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <xsd/cxx/xml/string.hxx>        // xml::{string, transcode}
#include <xsd/cxx/xml/bits/literals.hxx> // xml::bits::{xml_prefix,
                                         //             xml_namespace}
#include <xsd/cxx/xml/dom/serialization-header.hxx>  // dom::{prefix, clear}

#include <xsd/cxx/tree/exceptions.hxx>   // no_namespace_mapping
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>
#include <xsd/cxx/tree/list.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      //
      //
      template <typename C>
      struct list_stream
      {
        list_stream (std::basic_ostringstream<C>& os,
                     xercesc::DOMElement& parent)
            : os_ (os), parent_ (parent)
        {
        }

        std::basic_ostringstream<C>& os_;
        xercesc::DOMElement& parent_;
      };
    }
  }
}

// The only way to make the following serialization operators for
// fundamental types work is to defined them in the xercesc
// namespace so that they can be found by ADL. Placing them into
// global namespace does not work
//

namespace XERCES_CPP_NAMESPACE
{
  // Serialization of std::basic_string and C string. Used in other
  // serializers. Also used to serialize enumerators
  //
  template <typename C>
  void
  operator<< (xercesc::DOMElement& e, const C* s)
  {
    xsd::cxx::xml::dom::clear<char> (e);
    e.setTextContent (xsd::cxx::xml::string (s).c_str ());
  }

  template <typename C>
  void
  operator<< (xercesc::DOMAttr& a, const C* s)
  {
    a.setValue (xsd::cxx::xml::string (s).c_str ());
  }

  // We duplicate the code above instead of delegating in order to
  // allow the xml::string type to take advantage of cached string
  // sizes
  //
  template <typename C>
  void
  operator<< (xercesc::DOMElement& e, const std::basic_string<C>& s)
  {
    xsd::cxx::xml::dom::clear<char> (e);
    e.setTextContent (xsd::cxx::xml::string (s).c_str ());
  }

  template <typename C>
  void
  operator<< (xercesc::DOMAttr& a, const std::basic_string<C>& s)
  {
    a.setValue (xsd::cxx::xml::string (s).c_str ());
  }

  // Serialization of fundamental types
  //

  // 8-bit
  //
  inline void
  operator<< (xercesc::DOMElement& e, signed char c)
  {
    std::basic_ostringstream<char> os;
    os << static_cast<short> (c);
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, signed char c)
  {
    std::basic_ostringstream<char> os;
    os << static_cast<short> (c);
    a << os.str ();
  }

  inline void
  operator<< (xercesc::DOMElement& e, unsigned char c)
  {
    std::basic_ostringstream<char> os;
    os << static_cast<unsigned short> (c);
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, unsigned char c)
  {
    std::basic_ostringstream<char> os;
    os << static_cast<unsigned short> (c);
    a << os.str ();
  }

  // 16-bit
  //
  inline void
  operator<< (xercesc::DOMElement& e, short s)
  {
    std::basic_ostringstream<char> os;
    os << s;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, short s)
  {
    std::basic_ostringstream<char> os;
    os << s;
    a << os.str ();
  }

  inline void
  operator<< (xercesc::DOMElement& e, unsigned short s)
  {
    std::basic_ostringstream<char> os;
    os << s;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, unsigned short s)
  {
    std::basic_ostringstream<char> os;
    os << s;
    a << os.str ();
  }

  // 32-bit
  //
  inline void
  operator<< (xercesc::DOMElement& e, int i)
  {
    std::basic_ostringstream<char> os;
    os << i;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, int i)
  {
    std::basic_ostringstream<char> os;
    os << i;
    a << os.str ();
  }

  inline void
  operator<< (xercesc::DOMElement& e, unsigned int i)
  {
    std::basic_ostringstream<char> os;
    os << i;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, unsigned int i)
  {
    std::basic_ostringstream<char> os;
    os << i;
    a << os.str ();
  }

  // 64-bit
  //
  inline void
  operator<< (xercesc::DOMElement& e, long long l)
  {
    std::basic_ostringstream<char> os;
    os << l;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, long long l)
  {
    std::basic_ostringstream<char> os;
    os << l;
    a << os.str ();
  }

  inline void
  operator<< (xercesc::DOMElement& e, unsigned long long l)
  {
    std::basic_ostringstream<char> os;
    os << l;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, unsigned long long l)
  {
    std::basic_ostringstream<char> os;
    os << l;
    a << os.str ();
  }

  // Boolean
  //
  inline void
  operator<< (xercesc::DOMElement& e, bool b)
  {
    std::basic_ostringstream<char> os;
    os << std::boolalpha << b;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, bool b)
  {
    std::basic_ostringstream<char> os;
    os << std::boolalpha << b;
    a << os.str ();
  }

  // Floating-point types. The formula for the number of decimla digits
  // required is given in:
  // http://www2.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1822.pdf
  //
  inline void
  operator<< (xercesc::DOMElement& e, float f)
  {
    std::basic_ostringstream<char> os;
#ifdef XSD_FP_ALL_DIGITS
    os.precision (2 + std::numeric_limits<float>::digits * 301/1000);
#else
    os.precision (std::numeric_limits<float>::digits10);
#endif
    os << f;
    e << os.str ();
  }

  inline void
  operator<< (xercesc::DOMAttr& a, float f)
  {
    std::basic_ostringstream<char> os;
#ifdef XSD_FP_ALL_DIGITS
    os.precision (2 + std::numeric_limits<float>::digits * 301/1000);
#else
    os.precision (std::numeric_limits<float>::digits10);
#endif
    os << f;
    a << os.str ();
  }

  inline void
  operator<< (xercesc::DOMElement& e, double d)
  {
    std::basic_ostringstream<char> os;
#ifdef XSD_FP_ALL_DIGITS
    os.precision (2 + std::numeric_limits<double>::digits * 301/1000);
#else
    os.precision (std::numeric_limits<double>::digits10);
#endif

    // We map both xsd:double and xsd:decimal to double and decimal
    // cannot be in scientific notation. @@ Plus decimal cannot be
    // INF/NaN which is not handled at the moment
    //
    os << std::fixed << d;
    std::string s (os.str ());

    // Remove the trailing zeros and the decimal point if necessary
    //
    std::string::size_type size (s.size ()), n (size);

    for (; n > 0 && s[n - 1] == '0'; --n);

    if (n > 0 && s[n - 1] == '.')
      --n;

    if (n != size)
      s.resize (n);

    e << s;
  }

  inline void
  operator<< (xercesc::DOMAttr& a, double d)
  {
    std::basic_ostringstream<char> os;
#ifdef XSD_FP_ALL_DIGITS
    os.precision (2 + std::numeric_limits<double>::digits * 301/1000);
#else
    os.precision (std::numeric_limits<double>::digits10);
#endif

    // We map both xsd:double and xsd:decimal to double and decimal
    // cannot be in scientific notation
    //
    os << std::fixed << d;
    std::string s (os.str ());

    // Remove the trailing zeros and the decimal point if necessary
    //
    std::string::size_type size (s.size ()), n (size);

    for (; n > 0 && s[n - 1] == '0'; --n);

    if (n > 0 && s[n - 1] == '.')
      --n;

    if (n != size)
      s.resize (n);

    a << s;
  }
}

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // List serialization operators for fundamental types
      //

      template <typename C>
      void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, const C* s)
      {
        ls.os_ << s;
      }

      template <typename C>
      void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, const std::basic_string<C>& s)
      {
        ls.os_ << s;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, signed char c)
      {
        ls.os_ << static_cast<short> (c);
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, unsigned char c)
      {
        ls.os_ << static_cast<unsigned short> (c);
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, short s)
      {
        ls.os_ << s;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, unsigned short s)
      {
        ls.os_ << s;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, int i)
      {
        ls.os_ << i;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, unsigned int i)
      {
        ls.os_ << i;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, long long l)
      {
        ls.os_ << l;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, unsigned long long l)
      {
        ls.os_ << l;
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, bool b)
      {
        std::ios_base::fmtflags f (ls.os_.setf (std::ios_base::boolalpha));
        ls.os_ << b;
        ls.os_.setf (f);
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, float f)
      {
#ifdef XSD_FP_ALL_DIGITS
        std::streamsize p (
          ls.os_.precision (
            2 + std::numeric_limits<float>::digits * 301/1000));
#else
        std::streamsize p (
          ls.os_.precision (
            std::numeric_limits<float>::digits10));
#endif
        ls.os_ << f;
        ls.os_.precision (p);
      }

      template <typename C>
      inline void
      operator<< (xsd::cxx::tree::list_stream<C>& ls, double d)
      {
        /*
#ifdef XSD_FP_ALL_DIGITS
        std::streamsize p (
          ls.os_.precision (
            2 + std::numeric_limits<double>::digits * 301/1000));
#else
        std::streamsize p (
          ls.os_.precision (
            std::numeric_limits<double>::digits10));
#endif
        std::ios_base::fmtflags f (
          ls.os_.setf (std::ios_base::fixed, std::ios_base::floatfield));
        */

        std::basic_ostringstream<C> os;
#ifdef XSD_FP_ALL_DIGITS
        os.precision (2 + std::numeric_limits<double>::digits * 301/1000);
#else
        os.precision (std::numeric_limits<double>::digits10);
#endif

        // We map both xsd:double and xsd:decimal to double and decimal
        // cannot be in scientific notation. @@ Plus decimal cannot be
        // INF/NaN which is not handled at the moment
        //
        os << std::fixed << d;
        std::basic_string<C> s (os.str ());

        // Remove the trailing zeros and the decimal point if necessary
        //
        typename std::basic_string<C>::size_type size (s.size ()), n (size);

        for (; n > 0 && s[n - 1] == '0'; --n);

        if (n > 0 && s[n - 1] == '.')
          --n;

        if (n != size)
          s.resize (n);

        ls.os_ << s;

        /*
        ls.os_.setf (f, std::ios_base::floatfield);
        ls.os_.precision (p);
        */
      }

      // Insertion operators for type
      //
      inline void
      operator<< (xercesc::DOMElement& e, const type&)
      {
        xml::dom::clear<char> (e);
      }

      inline void
      operator<< (xercesc::DOMAttr&, const type&)
      {
      }

      template <typename C>
      inline void
      operator<< (list_stream<C>&, const type&)
      {
      }

      // Insertion operators for simple_type
      //
      template <typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const simple_type<B>&)
      {
        xml::dom::clear<char> (e);
      }

      template <typename B>
      inline void
      operator<< (xercesc::DOMAttr&, const simple_type<B>&)
      {
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>&, const simple_type<B>&)
      {
      }

      // Insertion operators for list
      //
      template <typename C, typename X, bool fund>
      void
      operator<< (xercesc::DOMElement& e, const list<X, C, fund>& v)
      {
        xml::dom::clear<char> (e);

        std::basic_ostringstream<C> os;
        list_stream<C> ls (os, e);

        ls << v;

        e << os.str ();
      }

      template <typename C, typename X, bool fund>
      void
      operator<< (xercesc::DOMAttr& a, const list<X, C, fund>& v)
      {
        std::basic_ostringstream<C> os;
        list_stream<C> ls (os, *a.getOwnerElement ());

        ls << v;

        a << os.str ();
      }

      template <typename C, typename X, bool fund>
      void
      operator<< (list_stream<C>& ls, const list<X, C, fund>& v)
      {
        for (typename list<X, C, fund>::const_iterator
               b (v.begin ()), e (v.end ()), i (b); i != e; ++i)
        {
          if (i != b)
            ls.os_ << C (' ');

          ls << *i;
        }
      }


      // Insertion operators for fundamental_base
      //
      template <typename X, typename C, typename B>
      void
      operator<< (xercesc::DOMElement& e, const fundamental_base<X, C, B>& x)
      {
        const X& r (x);
        e << r;
      }

      template <typename X, typename C, typename B>
      void
      operator<< (xercesc::DOMAttr& a, const fundamental_base<X, C, B>& x)
      {
        const X& r (x);
        a << r;
      }

      template <typename X, typename C, typename B>
      void
      operator<< (list_stream<C>& ls, const fundamental_base<X, C, B>& x)
      {
        const X& r (x);
        ls << r;
      }


      // Insertion operators for built-in types
      //


      namespace bits
      {
        template <typename C, typename X>
        void
        insert (xercesc::DOMElement& e, const X& x)
        {
          std::basic_ostringstream<C> os;
          os << x;
          e << os.str ();
        }

        template <typename C, typename X>
        void
        insert (xercesc::DOMAttr& a, const X& x)
        {
          std::basic_ostringstream<C> os;
          os << x;
          a << os.str ();
        }
      }


      // string
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const string<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const string<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const string<C, B>& x)
      {
        ls.os_ << x;
      }


      // normalized_string
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const normalized_string<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const normalized_string<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const normalized_string<C, B>& x)
      {
        ls.os_ << x;
      }


      // token
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const token<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const token<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const token<C, B>& x)
      {
        ls.os_ << x;
      }


      // nmtoken
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const nmtoken<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const nmtoken<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const nmtoken<C, B>& x)
      {
        ls.os_ << x;
      }


      // nmtokens
      //
      template <typename C, typename B, typename nmtoken>
      inline void
      operator<< (xercesc::DOMElement& e, const nmtokens<C, B, nmtoken>& v)
      {
        const list<nmtoken, C>& r (v);
        e << r;
      }

      template <typename C, typename B, typename nmtoken>
      inline void
      operator<< (xercesc::DOMAttr& a, const nmtokens<C, B, nmtoken>& v)
      {
        const list<nmtoken, C>& r (v);
        a << r;
      }

      template <typename C, typename B, typename nmtoken>
      inline void
      operator<< (list_stream<C>& ls, const nmtokens<C, B, nmtoken>& v)
      {
        const list<nmtoken, C>& r (v);
        ls << r;
      }


      // name
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const name<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const name<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const name<C, B>& x)
      {
        ls.os_ << x;
      }


      // ncname
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const ncname<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const ncname<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const ncname<C, B>& x)
      {
        ls.os_ << x;
      }


      // language
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const language<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const language<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const language<C, B>& x)
      {
        ls.os_ << x;
      }


      // id
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const id<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const id<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const id<C, B>& x)
      {
        ls.os_ << x;
      }


      // idref
      //
      template <typename X, typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const idref<X, C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename X, typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const idref<X, C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename X, typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const idref<X, C, B>& x)
      {
        ls.os_ << x;
      }


      // idrefs
      //
      template <typename C, typename B, typename idref>
      inline void
      operator<< (xercesc::DOMElement& e, const idrefs<C, B, idref>& v)
      {
        const list<idref, C>& r (v);
        e << r;
      }

      template <typename C, typename B, typename idref>
      inline void
      operator<< (xercesc::DOMAttr& a, const idrefs<C, B, idref>& v)
      {
        const list<idref, C>& r (v);
        a << r;
      }

      template <typename C, typename B, typename idref>
      inline void
      operator<< (list_stream<C>& ls, const idrefs<C, B, idref>& v)
      {
        const list<idref, C>& r (v);
        ls << r;
      }


      // uri
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const uri<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const uri<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const uri<C, B>& x)
      {
        ls.os_ << x;
      }


      // qname
      //
      template <typename C, typename B, typename uri, typename ncname>
      void
      operator<< (xercesc::DOMElement& e, const qname<C, B, uri, ncname>& x)
      {
        std::basic_ostringstream<C> os;
        const std::basic_string<C>& ns (x.namespace_ ());

        if (!ns.empty ())
        {
          try
          {
            std::basic_string<C> p (xml::dom::prefix (ns, e));

            if (!p.empty ())
              os << p << C (':');
          }
          catch (const xml::dom::no_prefix&)
          {
            throw no_namespace_mapping<C> (ns);
          }
        }

        os << x.name ();
        e << os.str ();
      }

      template <typename C, typename B, typename uri, typename ncname>
      void
      operator<< (xercesc::DOMAttr& a, const qname<C, B, uri, ncname>& x)
      {
        std::basic_ostringstream<C> os;
        const std::basic_string<C>& ns (x.namespace_ ());

        if (!ns.empty ())
        {
          try
          {
            std::basic_string<C> p (
              xml::dom::prefix (ns, *a.getOwnerElement ()));

            if (!p.empty ())
              os << p << C (':');
          }
          catch (const xml::dom::no_prefix&)
          {
            throw no_namespace_mapping<C> (ns);
          }
        }

        os << x.name ();
        a << os.str ();
      }

      template <typename C, typename B, typename uri, typename ncname>
      void
      operator<< (list_stream<C>& ls, const qname<C, B, uri, ncname>& x)
      {
        const std::basic_string<C>& ns (x.namespace_ ());

        if (!ns.empty ())
        {
          try
          {
            std::basic_string<C> p (xml::dom::prefix (ns, ls.parent_));

            if (!p.empty ())
              ls.os_ << p << C (':');
          }
          catch (const xml::dom::no_prefix&)
          {
            throw no_namespace_mapping<C> (ns);
          }
        }

        ls.os_ << x.name ();
      }


      // base64_binary
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const base64_binary<C, B>& x)
      {
        e << x.encode ();
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const base64_binary<C, B>& x)
      {
        a << x.encode ();
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const base64_binary<C, B>& x)
      {
        ls.os_ << x.encode ();
      }


      // hex_binary
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const hex_binary<C, B>& x)
      {
        e << x.encode ();
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const hex_binary<C, B>& x)
      {
        a << x.encode ();
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const hex_binary<C, B>& x)
      {
        ls.os_ << x.encode ();
      }


      // date
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const date<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const date<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const date<C, B>& x)
      {
        ls.os_ << x;
      }


      // date_time
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const date_time<C, B>& x)
      {

        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const date_time<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const date_time<C, B>& x)
      {
        ls.os_ << x;
      }


      // duration
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const duration<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const duration<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const duration<C, B>& x)
      {
        ls.os_ << x;
      }


      // day
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const day<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const day<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const day<C, B>& x)
      {
        ls.os_ << x;
      }


      // month
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const month<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const month<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const month<C, B>& x)
      {
        ls.os_ << x;
      }


      // month_day
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const month_day<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const month_day<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const month_day<C, B>& x)
      {
        ls.os_ << x;
      }


      // year
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const year<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const year<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const year<C, B>& x)
      {
        ls.os_ << x;
      }


      // year_month
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const year_month<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const year_month<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const year_month<C, B>& x)
      {
        ls.os_ << x;
      }


      // time
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const time<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const time<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const time<C, B>& x)
      {
        ls.os_ << x;
      }


      // entity
      //
      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMElement& e, const entity<C, B>& x)
      {
        bits::insert<C> (e, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (xercesc::DOMAttr& a, const entity<C, B>& x)
      {
        bits::insert<C> (a, x);
      }

      template <typename C, typename B>
      inline void
      operator<< (list_stream<C>& ls, const entity<C, B>& x)
      {
        ls.os_ << x;
      }


      // entities
      //
      template <typename C, typename B, typename entity>
      inline void
      operator<< (xercesc::DOMElement& e, const entities<C, B, entity>& v)
      {
        const list<entity, C>& r (v);
        e << r;
      }

      template <typename C, typename B, typename entity>
      inline void
      operator<< (xercesc::DOMAttr& a, const entities<C, B, entity>& v)
      {
        const list<entity, C>& r (v);
        a << r;
      }

      template <typename C, typename B, typename entity>
      inline void
      operator<< (list_stream<C>& ls, const entities<C, B, entity>& v)
      {
        const list<entity, C>& r (v);
        ls << r;
      }
    }
  }
}

#endif  // XSD_CXX_TREE_SERIALIZATION_HXX
