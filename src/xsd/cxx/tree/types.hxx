// file      : xsd/cxx/tree/types.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TYPES_HXX
#define XSD_CXX_TREE_TYPES_HXX

#include <string>
#include <cstddef> // std::size_t

#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/tree/buffer.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // string
      //
      template <typename C, typename B>
      class string: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        string ()
        {
        }

        string (const C* s)
            : base_type (s)
        {
        }

        string (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        string (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        string (const primary_type& str)
            : base_type (str)
        {
        }

        string (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        string (istream<S>&, flags = 0, type* container = 0);

      public:
        string (const string& other, flags f = 0, type* container = 0)
            : B (other, f, container),
              base_type (other)
        {
        }

        virtual string*
        _clone (flags = 0, type* container = 0) const;

      public:
        string (const xercesc::DOMElement&, flags = 0, type* container = 0);

        string (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        string (const std::basic_string<C>&,
                const xercesc::DOMElement*,
                flags = 0,
                type* container = 0);

      public:
        string&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        string&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        string&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //
      };


      // normalized_string: string
      //
      template <typename C, typename B>
      class normalized_string: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        normalized_string ()
        {
        }

        normalized_string (const C* s)
            : base_type (s)
        {
        }

        normalized_string (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        normalized_string (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        normalized_string (const primary_type& str)
            : base_type (str)
        {
        }

        normalized_string (const primary_type& str,
                           std::size_t pos,
                           std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        normalized_string (istream<S>&, flags = 0, type* container = 0);

      public:
        normalized_string (const normalized_string& other,
                           flags f = 0,
                           type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual normalized_string*
        _clone (flags = 0, type* container = 0) const;

      public:
        normalized_string (const xercesc::DOMElement&,
                           flags = 0,
                           type* container = 0);


        normalized_string (const xercesc::DOMAttr&,
                           flags = 0,
                           type* container = 0);

        normalized_string (const std::basic_string<C>&,
                           const xercesc::DOMElement*,
                           flags = 0,
                           type* container = 0);

      public:
        normalized_string&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        normalized_string&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        normalized_string&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        void
        normalize ();
      };


      // token: normalized_string
      //
      template <typename C, typename B>
      class token: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        token ()
        {
        }

        token (const C* s)
            : base_type (s)
        {
        }

        token (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        token (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        token (const primary_type& str)
            : base_type (str)
        {
        }

        token (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        token (istream<S>&, flags = 0, type* container = 0);

      public:
        token (const token& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual token*
        _clone (flags = 0, type* container = 0) const;

      public:
        token (const xercesc::DOMElement&, flags = 0, type* container = 0);

        token (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        token (const std::basic_string<C>&,
               const xercesc::DOMElement*,
               flags = 0,
               type* container = 0);

      public:
        token&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        token&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        token&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        void
        collapse ();
      };


      // nmtoken: token
      //
      template <typename C, typename B>
      class nmtoken: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        nmtoken (const C* s)
            : base_type (s)
        {
        }

        nmtoken (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        nmtoken (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        nmtoken (const primary_type& str)
            : base_type (str)
        {
        }

        nmtoken (const primary_type& str,
                 std::size_t pos,
                 std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        nmtoken (istream<S>&, flags = 0, type* container = 0);

      public:
        nmtoken (const nmtoken& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual nmtoken*
        _clone (flags = 0, type* container = 0) const;

      public:
        nmtoken (const xercesc::DOMElement&, flags = 0, type* container = 0);

        nmtoken (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        nmtoken (const std::basic_string<C>&,
                 const xercesc::DOMElement*,
                 flags = 0,
                 type* container = 0);

      public:
        nmtoken&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        nmtoken&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        nmtoken&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        nmtoken ()
            : base_type ()
        {
        }
      };


      // nmtokens: list<nmtoken>
      //
      template <typename C, typename B, typename nmtoken>
      class nmtokens: public B, public list<nmtoken, C>
      {
        typedef list<nmtoken, C> base_type;

      public:
        nmtokens ()
        {
        }

        template <typename S>
        nmtokens (istream<S>&, flags = 0, type* container = 0);

        nmtokens (const nmtokens& other, flags f, type* container = 0)
            : B (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual nmtokens*
        _clone (flags = 0, type* container = 0) const;

      public:
        nmtokens (const xercesc::DOMElement&, flags = 0, type* container = 0);

        nmtokens (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        nmtokens (const std::basic_string<C>&,
                  const xercesc::DOMElement*,
                  flags = 0,
                  type* container = 0);
      };


      // name: token
      //
      template <typename C, typename B>
      class name: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        name (const C* s)
            : base_type (s)
        {
        }

        name (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        name (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        name (const primary_type& str)
            : base_type (str)
        {
        }

        name (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        name (istream<S>&, flags = 0, type* container = 0);

      public:
        name (const name& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual name*
        _clone (flags = 0, type* container = 0) const;

      public:
        name (const xercesc::DOMElement&, flags = 0, type* container = 0);

        name (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        name (const std::basic_string<C>&,
              const xercesc::DOMElement*,
              flags = 0,
              type* container = 0);

      public:
        name&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        name&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        name&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        name ()
            : base_type ()
        {
        }
      };


      // Forward declaration for Sun CC
      //
      template <typename C, typename B, typename uri, typename ncname>
      class qname;

      // ncname: name
      //
      template <typename C, typename B>
      class ncname: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        ncname (const C* s)
            : base_type (s)
        {
        }

        ncname (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        ncname (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        ncname (const primary_type& str)
            : base_type (str)
        {
        }

        ncname (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        ncname (istream<S>&, flags = 0, type* container = 0);

      public:
        ncname (const ncname& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual ncname*
        _clone (flags = 0, type* container = 0) const;

      public:
        ncname (const xercesc::DOMElement&, flags = 0, type* container = 0);

        ncname (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        ncname (const std::basic_string<C>&,
                const xercesc::DOMElement*,
                flags = 0,
                type* container = 0);

      public:
        ncname&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        ncname&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        ncname&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        ncname ()
            : base_type ()
        {
        }

        template <typename, typename, typename, typename>
        friend class qname;
      };


      // language: token
      //
      template <typename C, typename B>
      class language: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        language (const C* s)
            : base_type (s)
        {
        }

        language (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        language (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        language (const primary_type& str)
            : base_type (str)
        {
        }

        language (const primary_type& str,
                  std::size_t pos,
                  std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        language (istream<S>&, flags = 0, type* container = 0);

      public:
        language (const language& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual language*
        _clone (flags = 0, type* container = 0) const;

      public:
        language (const xercesc::DOMElement&, flags = 0, type* container = 0);

        language (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        language (const std::basic_string<C>&,
                  const xercesc::DOMElement*,
                  flags = 0,
                  type* container = 0);

      public:
        language&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        language&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        language&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        language ()
            : base_type ()
        {
        }
      };


      //
      //
      template <typename C, typename ncname>
      struct identity_impl: identity
      {
        identity_impl (const ncname& id)
            : id_ (id)
        {
        }

        virtual bool
        before (const identity& y) const;

        virtual void
        throw_duplicate_id () const;

      private:
        const ncname& id_;
      };


      // id: ncname
      //
      template <typename C, typename B>
      class id: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        ~id()
        {
          unregister_id ();
        }

      public:
        id (const C* s)
            : base_type (s), identity_ (*this)
        {
          register_id ();
        }

        id (const C* s, std::size_t n)
            : base_type (s, n), identity_ (*this)
        {
          register_id ();
        }

        id (std::size_t n, C c)
            : base_type (n, c), identity_ (*this)
        {
          register_id ();
        }

        id (const primary_type& str)
            : base_type (str), identity_ (*this)
        {
          register_id ();
        }

        id (const primary_type& str,
            std::size_t pos,
            std::size_t n = primary_type::npos)
            : base_type (str, pos, n), identity_ (*this)
        {
          register_id ();
        }

        template <typename S>
        id (istream<S>&, flags = 0, type* container = 0);

      public:
        id (const id& other, flags f = 0, type* container = 0)
            : base_type (other, f, container), identity_ (*this)
        {
          register_id ();
        }

        virtual id*
        _clone (flags = 0, type* container = 0) const;

      public:
        id (const xercesc::DOMElement&, flags = 0, type* container = 0);

        id (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        id (const std::basic_string<C>&,
            const xercesc::DOMElement*,
            flags = 0,
            type* container = 0);

      public:
        id&
        operator= (C c);

        id&
        operator= (const C* s);

        id&
        operator= (const primary_type& str);

        id&
        operator= (const id& str);

      protected:
        id ()
            : base_type (), identity_ (*this)
        {
          register_id ();
        }

        // It would have been cleaner to mention empty and _container
        // with the using-declaration but HP aCC3 can't handle it in
        // some non-trivial to track down cases. So we are going to use
        // the old-n-ugly this-> techniques
        //
      private:
        void
        register_id ();

        void
        unregister_id ();

      private:
        identity_impl<C, B> identity_;
      };


      // idref: ncname
      //
      template <typename X, typename C, typename B>
      class idref: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        typedef X type;

      public:
        idref (const C* s)
            : base_type (s), identity_ (*this)
        {
        }

        idref (const C* s, std::size_t n)
            : base_type (s, n), identity_ (*this)
        {
        }

        idref (std::size_t n, C c)
            : base_type (n, c), identity_ (*this)
        {
        }

        idref (const primary_type& str)
            : base_type (str), identity_ (*this)
        {
        }

        idref (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n), identity_ (*this)
        {
        }

        template <typename S>
        idref (istream<S>&, flags = 0, type* container = 0);

      public:
        idref (const idref& other, flags f = 0, tree::type* container = 0)
            : base_type (other, f, container), identity_ (*this)
        {
        }

        virtual idref*
        _clone (flags = 0, tree::type* container = 0) const;

      public:
        idref (const xercesc::DOMElement&,
               flags = 0,
               tree::type* container = 0);

        idref (const xercesc::DOMAttr&,
               flags = 0,
               tree::type* container = 0);

        idref (const std::basic_string<C>&,
               const xercesc::DOMElement*,
               flags = 0,
               tree::type* container = 0);

      public:
        idref&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        idref&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        idref&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        idref&
        operator= (const idref& str)
        {
          base () = str;
          return *this;
        }

      public:
        const X*
        operator-> () const
        {
          return get ();
        }

        X*
        operator-> ()
        {
          return get ();
        }

        const X&
        operator* () const
        {
          return *(get ());
        }

        X&
        operator* ()
        {
          return *(get ());
        }

        const X*
        get () const
        {
          return dynamic_cast<const X*> (get_ ());
        }

        X*
        get ()
        {
          return dynamic_cast<X*> (get_ ());
        }

        // Conversion to bool
        //
        typedef void (idref::*bool_convertible)();

        operator bool_convertible () const
        {
          return get_ () ? &idref::true_ : 0;
        }

      protected:
        idref ()
            : base_type (), identity_ (*this)
        {
        }

      private:
        const tree::type*
        get_ () const;

        tree::type*
        get_ ();

        void
        true_ ();

      private:
        identity_impl<C, B> identity_;
      };


      // idrefs: list<idref>
      //
      template <typename C, typename B, typename idref>
      class idrefs: public B, public list<idref, C>
      {
        typedef list<idref, C> base_type;

      public:
        idrefs ()
        {
        }

        template <typename S>
        idrefs (istream<S>&, flags = 0, type* container = 0);

        idrefs (const idrefs& other, flags f = 0, type* container = 0)
            : B (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual idrefs*
        _clone (flags = 0, type* container = 0) const;

      public:
        idrefs (const xercesc::DOMElement&, flags = 0, type* container = 0);

        idrefs (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        idrefs (const std::basic_string<C>&,
                const xercesc::DOMElement*,
                flags = 0,
                type* container = 0);
      };


      // uri
      //
      template <typename C, typename B>
      class uri: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> base_type;
        typedef std::basic_string<C> primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        uri (const C* s)
            : base_type (s)
        {
        }

        uri (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        uri (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        uri (const primary_type& str)
            : base_type (str)
        {
        }

        uri (const primary_type& str,
             std::size_t pos,
             std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        uri (istream<S>&, flags = 0, type* container = 0);

      public:
        uri (const uri& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual uri*
        _clone (flags = 0, type* container = 0) const;

      public:
        uri (const xercesc::DOMElement&, flags = 0, type* container = 0);

        uri (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        uri (const std::basic_string<C>&,
             const xercesc::DOMElement*,
             flags = 0,
             type* container = 0);

      public:
        uri&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        uri&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        uri&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        uri ()
            : base_type ()
        {
        }

        template <typename, typename, typename, typename>
        friend class qname;
      };


      // qname
      //
      template <typename C, typename B, typename uri, typename ncname>
      class qname: public B
      {
      public:
        qname (const uri& ns, const ncname& name)
            : ns_ (ns), name_ (name)
        {
        }

        template <typename S>
        qname (istream<S>&, flags = 0, type* container = 0);

      public:
        qname (const qname& other, flags f = 0, type* container = 0)
            : B (other, f, container),
              ns_ (other.ns_),
              name_ (other.name_)
        {
          // Note that ns_ and name_ have no DOM association
          //
        }

        virtual qname*
        _clone (flags = 0, type* container = 0) const;

      public:
        qname (const xercesc::DOMElement&, flags = 0, type* container = 0);

        qname (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        qname (const std::basic_string<C>&,
               const xercesc::DOMElement*,
               flags = 0,
               type* container = 0);

      public:
        const uri&
        namespace_ () const
        {
          return ns_;
        }

        const ncname&
        name () const
        {
          return name_;
        }

      protected:
        qname ()
            : ns_ (), name_ ()
        {
        }

      private:
        static uri
        resolve (const std::basic_string<C>&, const xercesc::DOMElement*);

      private:
        uri ns_;
        ncname name_;
      };

      template <typename C, typename B, typename uri, typename ncname>
      inline bool
      operator== (const qname<C, B, uri, ncname>& a,
                  const qname<C, B, uri, ncname>& b)
      {
        return a.name () == b.name () && a.namespace_ () == b.namespace_ ();
      }

      template <typename C, typename B, typename uri, typename ncname>
      inline bool
      operator!= (const qname<C, B, uri, ncname>& a,
                  const qname<C, B, uri, ncname>& b)
      {
        return !(a == b);
      }


      // base64_binary
      //
      template <typename C, typename B>
      class base64_binary: public B, public buffer<C>
      {
      public:
        typedef typename buffer<C>::size_t size_t;

      public:
        base64_binary (size_t size = 0);
        base64_binary (size_t size, size_t capacity);
        base64_binary (const void* data, size_t size);
        base64_binary (const void* data, size_t size, size_t capacity);

        // If the assume_ownership argument is true, the buffer will
        // assume the ownership of the data and will release the memory
        // by calling operator delete ()
        //
        base64_binary (void* data,
                       size_t size,
                       size_t capacity,
                       bool assume_ownership);

        template <typename S>
        base64_binary (istream<S>&, flags = 0, type* container = 0);

      public:
        base64_binary (const base64_binary& other,
                       flags f = 0,
                       type* container = 0)
            : B (other, f, container), buffer<C> (other)
        {
        }

        virtual base64_binary*
        _clone (flags = 0, type* container = 0) const;

      public:
        base64_binary (const xercesc::DOMElement&,
                       flags = 0,
                       type* container = 0);

        base64_binary (const xercesc::DOMAttr&,
                       flags = 0,
                       type* container = 0);

        base64_binary (const std::basic_string<C>&,
                       const xercesc::DOMElement*,
                       flags = 0,
                       type* container = 0);

      public:
        std::basic_string<C>
        encode () const;

      public:
        // Implicit copy assignment operator
        //

      private:
        void
        decode (const XMLCh*);
      };


      // hex_binary
      //
      template <typename C, typename B>
      class hex_binary: public B, public buffer<C>
      {
      public:
        typedef typename buffer<C>::size_t size_t;

      public:
        hex_binary (size_t size = 0);
        hex_binary (size_t size, size_t capacity);
        hex_binary (const void* data, size_t size);
        hex_binary (const void* data, size_t size, size_t capacity);

        // If the assume_ownership argument is true, the buffer will
        // assume the ownership of the data and will release the memory
        // by calling operator delete ()
        //
        hex_binary (void* data,
                    size_t size,
                    size_t capacity,
                    bool assume_ownership);

        template <typename S>
        hex_binary (istream<S>&, flags = 0, type* container = 0);

      public:
        hex_binary (const hex_binary& other, flags f = 0, type* container = 0)
            : B (other, f, container), buffer<C> (other)
        {
        }

        virtual hex_binary*
        _clone (flags = 0, type* container = 0) const;

      public:
        hex_binary (const xercesc::DOMElement&,
                    flags = 0,
                    type* container = 0);

        hex_binary (const xercesc::DOMAttr&,
                    flags = 0,
                    type* container = 0);

        hex_binary (const std::basic_string<C>&,
                    const xercesc::DOMElement*,
                    flags = 0,
                    type* container = 0);

      public:
        std::basic_string<C>
        encode () const;

      public:
        // Implicit copy assignment operator
        //

      private:
        void
        decode (const XMLCh*);
      };


      // date
      //
      template <typename C, typename B>
      class date: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        date (const C* s)
            : base_type (s)
        {
        }

        date (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        date (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        date (const primary_type& str)
            : base_type (str)
        {
        }

        date (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        date (istream<S>&, flags = 0, type* container = 0);

      public:
        date (const date& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual date*
        _clone (flags = 0, type* container = 0) const;

      public:
        date (const xercesc::DOMElement&, flags = 0, type* container = 0);

        date (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        date (const std::basic_string<C>&,
              const xercesc::DOMElement*,
              flags = 0,
              type* container = 0);

      public:
        date&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        date&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        date&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        date ()
            : base_type ()
        {
        }
      };


      // date_time
      //
      template <typename C, typename B>
      class date_time: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        date_time (const C* s)
            : base_type (s)
        {
        }

        date_time (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        date_time (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        date_time (const primary_type& str)
            : base_type (str)
        {
        }

        date_time (const primary_type& str,
                   std::size_t pos,
                   std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        date_time (istream<S>&, flags = 0, type* container = 0);

      public:
        date_time (const date_time& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual date_time*
        _clone (flags = 0, type* container = 0) const;

      public:
        date_time (const xercesc::DOMElement&,
                   flags = 0,
                   type* container = 0);

        date_time (const xercesc::DOMAttr&,
                   flags = 0,
                   type* container = 0);

        date_time (const std::basic_string<C>&,
                   const xercesc::DOMElement*,
                   flags = 0,
                   type* container = 0);

      public:
        date_time&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        date_time&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        date_time&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        date_time ()
            : base_type ()
        {
        }
      };


      // duration
      //
      template <typename C, typename B>
      class duration: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        duration (const C* s)
            : base_type (s)
        {
        }

        duration (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        duration (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        duration (const primary_type& str)
            : base_type (str)
        {
        }

        duration (const primary_type& str,
                  std::size_t pos,
                  std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        duration (istream<S>&, flags = 0, type* container = 0);

      public:
        duration (const duration& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual duration*
        _clone (flags = 0, type* container = 0) const;

      public:
        duration (const xercesc::DOMElement&, flags = 0, type* container = 0);

        duration (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        duration (const std::basic_string<C>&,
                  const xercesc::DOMElement*,
                  flags = 0,
                  type* container = 0);

      public:
        duration&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        duration&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        duration&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        duration ()
            : base_type ()
        {
        }
      };


      // day
      //
      template <typename C, typename B>
      class day: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        day (const C* s)
            : base_type (s)
        {
        }

        day (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        day (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        day (const primary_type& str)
            : base_type (str)
        {
        }

        day (const primary_type& str,
             std::size_t pos,
             std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        day (istream<S>&, flags = 0, type* container = 0);

      public:
        day (const day& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual day*
        _clone (flags = 0, type* container = 0) const;

      public:
        day (const xercesc::DOMElement&, flags = 0, type* container = 0);

        day (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        day (const std::basic_string<C>&,
             const xercesc::DOMElement*,
             flags = 0,
             type* container = 0);

      public:
        day&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        day&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        day&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        day ()
            : base_type ()
        {
        }
      };


      // month
      //
      template <typename C, typename B>
      class month: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        month (const C* s)
            : base_type (s)
        {
        }

        month (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        month (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        month (const primary_type& str)
            : base_type (str)
        {
        }

        month (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        month (istream<S>&, flags = 0, type* container = 0);

      public:
        month (const month& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual month*
        _clone (flags = 0, type* container = 0) const;

      public:
        month (const xercesc::DOMElement&, flags = 0, type* container = 0);

        month (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        month (const std::basic_string<C>&,
               const xercesc::DOMElement*,
               flags = 0,
               type* container = 0);

      public:
        month&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        month&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        month&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        month ()
            : base_type ()
        {
        }
      };


      // month_day
      //
      template <typename C, typename B>
      class month_day: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        month_day (const C* s)
            : base_type (s)
        {
        }

        month_day (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        month_day (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        month_day (const primary_type& str)
            : base_type (str)
        {
        }

        month_day (const primary_type& str,
                   std::size_t pos,
                   std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        month_day (istream<S>&, flags = 0, type* container = 0);

      public:
        month_day (const month_day& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual month_day*
        _clone (flags = 0, type* container = 0) const;

      public:
        month_day (const xercesc::DOMElement&,
                   flags = 0,
                   type* container = 0);

        month_day (const xercesc::DOMAttr&,
                   flags = 0,
                   type* container = 0);

        month_day (const std::basic_string<C>&,
                   const xercesc::DOMElement*,
                   flags = 0,
                   type* container = 0);

      public:
        month_day&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        month_day&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        month_day&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        month_day ()
            : base_type ()
        {
        }
      };


      // year
      //
      template <typename C, typename B>
      class year: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        year (const C* s)
            : base_type (s)
        {
        }

        year (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        year (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        year (const primary_type& str)
            : base_type (str)
        {
        }

        year (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        year (istream<S>&, flags = 0, type* container = 0);

      public:
        year (const year& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual year*
        _clone (flags = 0, type* container = 0) const;

      public:
        year (const xercesc::DOMElement&, flags = 0, type* container = 0);

        year (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        year (const std::basic_string<C>&,
              const xercesc::DOMElement*,
              flags = 0,
              type* container = 0);

      public:
        year&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        year&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        year&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        year ()
            : base_type ()
        {
        }
      };


      // year_month
      //
      template <typename C, typename B>
      class year_month: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        year_month (const C* s)
            : base_type (s)
        {
        }

        year_month (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        year_month (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        year_month (const primary_type& str)
            : base_type (str)
        {
        }

        year_month (const primary_type& str,
                    std::size_t pos,
                    std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        year_month (istream<S>&, flags = 0, type* container = 0);

      public:
        year_month (const year_month& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual year_month*
        _clone (flags = 0, type* container = 0) const;

      public:
        year_month (const xercesc::DOMElement&,
                    flags = 0,
                    type* container = 0);

        year_month (const xercesc::DOMAttr&,
                    flags = 0,
                    type* container = 0);

        year_month (const std::basic_string<C>&,
                    const xercesc::DOMElement*,
                    flags = 0,
                    type* container = 0);
      public:
        year_month&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        year_month&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        year_month&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        year_month ()
            : base_type ()
        {
        }
      };


      // time
      //
      template <typename C, typename B>
      class time: public B, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        time (const C* s)
            : base_type (s)
        {
        }

        time (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        time (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        time (const primary_type& str)
            : base_type (str)
        {
        }

        time (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        time (istream<S>&, flags = 0, type* container = 0);

      public:
        time (const time& other, flags f = 0, type* container = 0)
            : B (other, f, container), base_type (other)
        {
        }

        virtual time*
        _clone (flags = 0, type* container = 0) const;

      public:
        time (const xercesc::DOMElement&, flags = 0, type* container = 0);

        time (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        time (const std::basic_string<C>&,
              const xercesc::DOMElement*,
              flags = 0,
              type* container = 0);

      public:
        time&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        time&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        time&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        time ()
            : base_type ()
        {
        }
      };


      // entity: ncname
      //
      template <typename C, typename B>
      class entity: public B
      {
      protected:
        typedef B base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        entity (const C* s)
            : base_type (s)
        {
        }

        entity (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        entity (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        entity (const primary_type& str)
            : base_type (str)
        {
        }

        entity (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        entity (istream<S>&, flags = 0, type* container = 0);

      public:
        entity (const entity& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual entity*
        _clone (flags = 0, type* container = 0) const;

      public:
        entity (const xercesc::DOMElement&, flags = 0, type* container = 0);

        entity (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        entity (const std::basic_string<C>&,
                const xercesc::DOMElement*,
                flags = 0,
                type* container = 0);

      public:
        entity&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        entity&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        entity&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator
        //

      protected:
        entity ()
            : base_type ()
        {
        }
      };


      // entities: list<entity>
      //
      template <typename C, typename B, typename entity>
      class entities: public B, public list<entity, C>
      {
        typedef list<entity, C> base_type;

      public:
        entities ()
        {
        }

        template <typename S>
        entities (istream<S>&, flags = 0, type* container = 0);

        entities (const entities& other, flags f = 0, type* container = 0)
            : B (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual entities*
        _clone (flags = 0, type* container = 0) const;

      public:
        entities (const xercesc::DOMElement&, flags = 0, type* container = 0);

        entities (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        entities (const std::basic_string<C>&,
                  const xercesc::DOMElement*,
                  flags = 0,
                  type* container = 0);
      };
    }
  }
}

#include <xsd/cxx/tree/types.txx>

#endif  // XSD_CXX_TREE_TYPES_HXX
