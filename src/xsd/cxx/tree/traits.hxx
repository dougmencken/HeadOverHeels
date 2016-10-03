// file      : xsd/cxx/tree/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TRAITS_HXX
#define XSD_CXX_TREE_TRAITS_HXX

// Do not include this file directly. Rather include elements.hxx
//

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // bool
      //
      template <typename C>
      struct traits<bool, C>
      {
        typedef bool type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      // 8 bit
      //

      template <typename C>
      struct traits<signed char, C>
      {
        typedef signed char type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      template <typename C>
      struct traits<unsigned char, C>
      {
        typedef unsigned char type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      // 16 bit
      //

      template <typename C>
      struct traits<short, C>
      {
        typedef short type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      template <typename C>
      struct traits<unsigned short, C>
      {
        typedef unsigned short type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      // 32 bit
      //

      template <typename C>
      struct traits<int, C>
      {
        typedef int type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      template <typename C>
      struct traits<unsigned int, C>
      {
        typedef unsigned int type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      // 64 bit
      //

      template <typename C>
      struct traits<long long, C>
      {
        typedef long long type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      template <typename C>
      struct traits<unsigned long long, C>
      {
        typedef unsigned long long type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      // floating point
      //

      template <typename C>
      struct traits<float, C>
      {
        typedef float type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };


      template <typename C>
      struct traits<double, C>
      {
        typedef double type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };
    }
  }
}

#include <xsd/cxx/tree/traits.txx>

#endif  // XSD_CXX_TREE_TRAITS_HXX

