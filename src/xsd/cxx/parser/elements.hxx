// file      : xsd/cxx/parser/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_ELEMENTS_HXX
#define XSD_CXX_PARSER_ELEMENTS_HXX

#include <xsd/cxx/ro-string.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      // pre() and post() are overridable pre/post hooks, i.e., the derived
      // parser can override them without calling the base version. _pre()
      // and _post() are not overridable pre/post hooks in the sense that
      // the derived parser may override them but has to call the base
      // version. The call sequence is as shown below:
      //
      // pre ()
      // _pre ()
      // _post ()
      // post ()
      //
      template <typename C>
      struct parser_base
      {
        virtual
        ~parser_base ();

        virtual void
        pre ();

        virtual void
        _pre ();

        virtual void
        _start_element (const ro_string<C>& ns,
                        const ro_string<C>& name) = 0;

        virtual void
        _end_element (const ro_string<C>& ns,
                      const ro_string<C>& name) = 0;

        virtual void
        _attribute (const ro_string<C>& ns,
                    const ro_string<C>& name,
                    const ro_string<C>& value) = 0;

        virtual void
        _characters (const ro_string<C>&) = 0;

        virtual void
        _post ();

        // Implementation hooks for _pre and _post. The _pre and _post
        // hooks should never be called directly. Instead, the *_impl
        // versions should be used. By default _pre_impl and _post_impl
        // simply call _pre and _post respectively
        //
        virtual void
        _pre_impl ();

        virtual void
        _post_impl ();
      };
    }
  }
}

#include <xsd/cxx/parser/elements.txx>

#endif  // XSD_CXX_PARSER_ELEMENTS_HXX
