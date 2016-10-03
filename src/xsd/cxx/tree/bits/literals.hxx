// file      : xsd/cxx/tree/bits/literals.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_BITS_LITERALS_HXX
#define XSD_CXX_TREE_BITS_LITERALS_HXX

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      namespace bits
      {
        // Boolean literals
        //
        template<typename C>
        const C*
        true_ ();

        template<typename C>
        const C*
        one ();

        // Float literals: INF -INF NaN
        //
        template<typename C>
        const C*
        positive_inf ();

        template<typename C>
        const C*
        negative_inf ();

        template<typename C>
        const C*
        nan ();

        // Optional "not present" literal
        //
        template<typename C>
        const C*
        not_present ();

        // XML Schema namespace
        //
        template <typename C>
        const C*
        xml_schema ()
        {
          return "http://www.w3.org/2001/XMLSchema";
        }

        // Built-in XML Schema type names
        //
        template <typename C>
        const C*
        any_type ()
        {
          return "anyType";
        }

        template <typename C>
        const C*
        any_simple_type ()
        {
          return "anySimpleType";
        }

        template <typename C>
        const C*
        string ()
        {
          return "string";
        }

        template <typename C>
        const C*
        normalized_string ()
        {
          return "normalizedString";
        }

        template <typename C>
        const C*
        token ()
        {
          return "token";
        }

        template <typename C>
        const C*
        name ()
        {
          return "Name";
        }

        template <typename C>
        const C*
        nmtoken ()
        {
          return "NMTOKEN";
        }

        template <typename C>
        const C*
        nmtokens ()
        {
          return "NMTOKENS";
        }

        template <typename C>
        const C*
        ncname ()
        {
          return "NCName";
        }

        template <typename C>
        const C*
        language ()
        {
          return "language";
        }


        template <typename C>
        const C*
        id ()
        {
          return "ID";
        }

        template <typename C>
        const C*
        idref ()
        {
          return "IDREF";
        }

        template <typename C>
        const C*
        idrefs ()
        {
          return "IDREFS";
        }

        template <typename C>
        const C*
        any_uri ()
        {
          return "anyURI";
        }

        template <typename C>
        const C*
        qname ()
        {
          return "QName";
        }

        template <typename C>
        const C*
        base64_binary ()
        {
          return "base64Binary";
        }

        template <typename C>
        const C*
        hex_binary ()
        {
          return "hexBinary";
        }

        template <typename C>
        const C*
        date ()
        {
          return "date";
        }

        template <typename C>
        const C*
        date_time ()
        {
          return "dateTime";
        }

        template <typename C>
        const C*
        duration ()
        {
          return "duration";
        }

        template <typename C>
        const C*
        gday ()
        {
          return "gDay";
        }

        template <typename C>
        const C*
        gmonth ()
        {
          return "gMonth";
        }

        template <typename C>
        const C*
        gmonth_day ()
        {
          return "gMonthDay";
        }

        template <typename C>
        const C*
        gyear ()
        {
          return "gYear";
        }

        template <typename C>
        const C*
        gyear_month ()
        {
          return "gYearMonth";
        }

        template <typename C>
        const C*
        time ()
        {
          return "time";
        }

        template <typename C>
        const C*
        entity ()
        {
          return "ENTITY";
        }

        template <typename C>
        const C*
        entities ()
        {
          return "ENTITIES";
        }
      }
    }
  }
}

#endif  // XSD_CXX_TREE_BITS_LITERALS_HXX

#include <xsd/cxx/tree/bits/literals.ixx>
