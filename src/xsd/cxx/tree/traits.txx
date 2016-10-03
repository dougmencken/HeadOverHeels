// file      : xsd/cxx/tree/traits.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <limits>

#include <xsd/cxx/ro-string.hxx>
#include <xsd/cxx/zc-istream.hxx>

#include <xsd/cxx/xml/string.hxx> // xml::transcode

#include <xsd/cxx/tree/text.hxx>  // text_content
#include <xsd/cxx/tree/bits/literals.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // Note that none of the types implemented here can have
      // whitespaces in the value. As result we don't need to
      // waste time collapsing whitespaces. All we need to do
      // is trim the string representation which can be done
      // without copying.
      //

      //
      // bool
      //
      template <typename C>
      bool traits<bool, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      bool traits<bool, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      bool traits<bool, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        return tmp == bits::true_<C> () || tmp == bits::one<C> ();
      }


      // 8 bit
      //

      template <typename C>
      signed char traits<signed char, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      signed char traits<signed char, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      signed char traits<signed char, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        short t;
        is >> t;

        return static_cast<type> (t);
      }



      template <typename C>
      unsigned char traits<unsigned char, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      unsigned char traits<unsigned char, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      unsigned char traits<unsigned char, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        unsigned short t;
        is >> t;

        return static_cast<type> (t);
      }


      // 16 bit
      //

      template <typename C>
      short traits<short, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      short traits<short, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      short traits<short, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      template <typename C>
      unsigned short traits<unsigned short, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      unsigned short traits<unsigned short, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      unsigned short traits<unsigned short, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      // 32 bit
      //

      template <typename C>
      int traits<int, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      int traits<int, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      int traits<int, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      template <typename C>
      unsigned int traits<unsigned int, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      unsigned int traits<unsigned int, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      unsigned int traits<unsigned int, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      // 64 bit
      //

      template <typename C>
      long long traits<long long, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      long long traits<long long, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      long long traits<long long, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      template <typename C>
      unsigned long long traits<unsigned long long, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      unsigned long long traits<unsigned long long, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      unsigned long long traits<unsigned long long, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      // floating point
      //

      template <typename C>
      float traits<float, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      float traits<float, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      float traits<float, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        if (tmp == bits::positive_inf<C> ())
          return std::numeric_limits<float>::infinity ();

        if (tmp == bits::negative_inf<C> ())
          return -std::numeric_limits<float>::infinity ();

        if (tmp == bits::nan<C> ())
          return std::numeric_limits<float>::quiet_NaN ();

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }


      template <typename C>
      double traits<double, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      double traits<double, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      double traits<double, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        ro_string<C> tmp (s);
        trim (tmp);

        // @@ We map both xsd:double and xsd:decimal to double and decimal
        // cannot be in scientific notation or be INF/NaN.
        //
        if (tmp == bits::positive_inf<C> ())
          return std::numeric_limits<double>::infinity ();

        if (tmp == bits::negative_inf<C> ())
          return -std::numeric_limits<double>::infinity ();

        if (tmp == bits::nan<C> ())
          return std::numeric_limits<double>::quiet_NaN ();

        zc_istream<C> is (tmp);

        type t;
        is >> t;

        return t;
      }
    }
  }
}

