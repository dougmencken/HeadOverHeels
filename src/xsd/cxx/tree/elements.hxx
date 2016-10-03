// file      : xsd/cxx/tree/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_ELEMENTS_HXX
#define XSD_CXX_TREE_ELEMENTS_HXX

#include <map>
#include <string>
#include <memory>  // std::auto_ptr
#include <istream>
#include <sstream>
#include <cassert>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>

#include <xsd/cxx/xml/elements.hxx> // xml::properties
#include <xsd/cxx/xml/dom/auto-ptr.hxx> // dom::auto_ptr

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/istream-fwd.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      //
      //
      struct flags
      {
        // Use the following flags to modify the default behavior
        // of the parsing and serialization functions
        //

        // Keep DOM association in the resulting tree. Useful for
        // dealing with "type-less" content such as mixed content
        // models, any/anyAttribute, and anyType/anySimpleType
        //
        static const unsigned long keep_dom = 0x00000100UL;

        // Assume ownership of the DOM document. This flag only
        // makes sense together with the keep_dom flag in the call
        // to the parsing function with the DOMDocument* argument
        //
        static const unsigned long own_dom = 0x00000200UL;

        // Do not try to validate instance documents. Note that
        // the xsd runtime assumes instance documents are valid so
        // you had better make sure they are if you specify this flag
        //
        //
        static const unsigned long dont_validate = 0x00000400UL;

        // Do not initialize the Xerces-C++ runtime
        //
        static const unsigned long dont_initialize = 0x00000001UL;

        // Do not write XML declaration (<?xml ... ?>) during
        // serialization
        //
        static const unsigned long no_xml_declaration = 0x00010000UL;


        // The following flags are for internal use
        //
        static const unsigned long base = 0x01000000UL;


        // Notes on flag blocks:
        //
        // 0x000000FF - common (applicable to both parsing and serialization)
        // 0x0000FF00 - parsing (values aligned with XML parsing)
        // 0x00FF0000 - serialization (values aligned with XML serialization)
        // 0xFF000000 - internal

      public:
        flags (unsigned long x = 0)
            : x_ (x)
        {
        }

        operator unsigned long () const
        {
          return x_;
        }

        friend flags
        operator| (const flags& a, const flags& b)
        {
          return flags (a.x_ | b.x_);
        }

        friend flags
        operator| (const flags& a, unsigned long b)
        {
          return flags (a.x_ | b);
        }

        friend flags
        operator| (unsigned long a, const flags& b)
        {
          return flags (a | b.x_);
        }

      private:
        unsigned long x_;
      };


      // Parsing properties. Refer to xsd/cxx/xml/elements.hxx for XML-
      // related properties
      //
      template <typename C>
      class properties: public xml::properties<C>
      {
      };


      // DOM user data keys
      //
      template <int dummy>
      struct user_data_keys_template
      {
        // Back pointers to tree nodes
        //
        static const XMLCh node[21];
      };

      typedef user_data_keys_template<0> user_data_keys;

      // HP aCC3 complains about unresolved symbols without an explicit
      // instantiation
      //
#if defined(__HP_aCC) && __HP_aCC <= 39999
      template struct user_data_keys_template<0>;
#endif


      //
      //
      struct identity
      {
        virtual
        ~identity ()
        {
        }

        identity ()
        {
        }

        virtual bool
        before (const identity&) const = 0;

        virtual void
        throw_duplicate_id () const = 0;

      private:
        identity (const identity&);

        identity&
        operator= (const identity&);
      };


      // anyType. VC++ has a name injection bug that makes it impossible
      // to have a member with the same name as a base type. To address
      // that we will have to choose some unique name for the definition
      // and typedef it to 'type'
      //
      class _type;
      typedef _type type;

      class _type
      {
      public:
        virtual
        ~_type ()
        {
          // Everyhting should have been unregistered by now
          //
          assert (map_.get () == 0 || map_->size () == 0);
        }

      public:
        _type ()
            : container_ (0)
        {
        }

        _type (const xercesc::DOMElement&, flags = 0, type* container = 0);

        _type (const xercesc::DOMAttr&, flags = 0, type* container = 0);

        template <typename C>
        _type (const std::basic_string<C>&,
               const xercesc::DOMElement*,
               flags = 0,
               type* container = 0);

        template <typename S>
        _type (istream<S>&, flags = 0, type* container = 0);

        _type (const type& t, flags = 0, type* container = 0)
            : container_ (container)
        {
          if (t.dom_info_.get ())
          {
            std::auto_ptr<dom_info> r (t.dom_info_->clone (*this, container));
            dom_info_ = r;
          }
        }

      public:
        type&
        operator= (const type&)
        {
          return *this;
        }

      public:
        virtual type*
        _clone (flags f = 0, type* container = 0) const
        {
          return new type (*this, f, container);
        }

        // Container API
        //
      public:
        const type*
        _container () const
        {
          return container_ ? container_ : this;
        }

        type*
        _container ()
        {
          return container_ ? container_ : this;
        }


        const type*
        _root () const
        {
          const type* r (_container ());

          for (const type* c = r->_container ();
               c != r;
               c = c->_container ())
            r = c;

          return r;
        }

        type*
        _root ()
        {
          type* r (_container ());

          for (type* c = r->_container (); c != r; c = c->_container ())
            r = c;

          return r;
        }

      public:
        void
        _register_id (const identity& id, type* t)
        {
          if (map_.get () == 0)
            map_.reset (new map);

          // First register on our container. If there are any duplications,
          // they will be detected by this call and we don't need to clean
          // the map
          //
          if (_container () != this)
	    _container ()->_register_id (id, t);

          if (!map_->insert (
                std::pair<const identity*, type*> (&id, t)).second)
          {
            id.throw_duplicate_id ();
          }
        }

        //@@ Does not inherit from exception
        //
        struct not_registered: std::exception
        {
          virtual const char*
          what () const throw ()
          {
            return "attempt to unregister non-existent id";
          }
        };

        void
        _unregister_id (const identity& id)
        {
          if (map_.get ())
          {
            map::iterator it (map_->find (&id));

            if (it != map_->end ())
            {
              map_->erase (it);

              if (_container () != this)
	        _container ()->_unregister_id (id);

              return;
            }
          }

          throw not_registered ();
        }

        type*
        _lookup_id (const identity& id) const
        {
          if (map_.get ())
          {
            map::const_iterator it (map_->find (&id));

            if (it != map_->end ())
              return it->second;
          }

          return 0;
        }

        // DOM association
        //
      public:
        const xercesc::DOMNode*
        _node () const
        {
          return dom_info_.get () ? dom_info_->node() : 0;
        }

        xercesc::DOMNode*
        _node ()
        {
          return dom_info_.get () ? dom_info_->node () : 0;
        }

        //@@ Does not inherit from exception
        //
        struct bad_dom_node_type: std::exception
        {
          virtual const char*
          what () const throw ()
          {
            return "DOM node is not an attribute node or element node";
          }
        };

        // Manually set node association. The DOM node should be
        // a child of our parent's DOM node. If this tree node is
        // a root of the tree, then it will assume the ownership
        // of the whole DOM document to which this DOM node belongs
        //
        void
        _node (xercesc::DOMNode* n)
        {
          switch (n->getNodeType ())
          {
          case xercesc::DOMNode::ATTRIBUTE_NODE:
            {
              if (container_ != 0)
              {
                // @@ Should be a throw
                //
                assert (_root ()->_node () != 0);
                assert (_root ()->_node ()->getOwnerDocument () ==
                        n->getOwnerDocument ());
              }

              std::auto_ptr<dom_info> r (
                dom_info_factory::create (
                  *static_cast<xercesc::DOMElement*> (n),
                  *this,
                  container_ == 0));

              dom_info_ = r;
              break;
            }
          case xercesc::DOMNode::ELEMENT_NODE:
            {
              //@@ Should be a throw
              //
              assert (container_ != 0);
              assert (_root ()->_node () != 0);
              assert (_root ()->_node ()->getOwnerDocument () ==
                      n->getOwnerDocument ());

              std::auto_ptr<dom_info> r (
                dom_info_factory::create (
                  *static_cast<xercesc::DOMAttr*> (n),
                  *this));

              dom_info_ = r;
              break;
            }
          default:
            {
              throw bad_dom_node_type ();
            }
          }
        }

      private:
        struct dom_info
        {
          virtual
          ~dom_info ()
          {
          }

          dom_info ()
          {
          }

          virtual std::auto_ptr<dom_info>
          clone (type& tree_node, type* container) const = 0;

          virtual xercesc::DOMNode*
          node () = 0;

        private:
          dom_info (const dom_info&);

          dom_info&
          operator= (const dom_info&);
        };


        struct dom_element_info: public dom_info
        {
          dom_element_info (xercesc::DOMElement& e, type& n, bool root)
              : doc_ (root ? e.getOwnerDocument () : 0), e_ (e)
          {
            e_.setUserData (user_data_keys::node, &n, 0);
          }

          virtual std::auto_ptr<dom_info>
          clone (type& tree_node, type* c) const
          {
            using std::auto_ptr;

            // Check if we are a document root
            //
            if (c == 0)
            {
              // We preserver DOM associations only in complete
              // copies from root
              //
              if (doc_.get () == 0)
                return auto_ptr<dom_info> (0);

              return auto_ptr<dom_info> (
                new dom_element_info (*doc_, tree_node));
            }

            // Check if our container does not have DOM association (e.g.,
            // because it wasn't a complete copy of the tree)
            //
            using xercesc::DOMNode;

            DOMNode* cn (c->_node ());

            if (cn == 0)
              return auto_ptr<dom_info> (0);


            // Now we are going to find the corresponding element in
            // the new tree
            //
            {
              using xercesc::DOMElement;

              DOMNode& pn (*e_.getParentNode ());
              assert (pn.getNodeType () == DOMNode::ELEMENT_NODE);

              DOMNode* sn (pn.getFirstChild ()); // Source
              DOMNode* dn (cn->getFirstChild ()); // Destination

              // We should have at least one child
              //
              assert (sn != 0);

              // Move in parallel until we get to the needed node
              //
              for (; sn != 0 && !e_.isSameNode (sn);)
              {
                sn = sn->getNextSibling ();
                dn = dn->getNextSibling ();
              }

              // e_ should be on the list
              //
              assert (sn != 0);

              assert (dn->getNodeType () == DOMNode::ELEMENT_NODE);

              return auto_ptr<dom_info> (
                new dom_element_info (static_cast<DOMElement&> (*dn),
                                      tree_node,
                                      false));
            }
          }

          virtual xercesc::DOMNode*
          node ()
          {
            return &e_;
          }

        private:
          dom_element_info (const xercesc::DOMDocument& d, type& n)
              : doc_ (static_cast<xercesc::DOMDocument*> (
                        d.cloneNode (true))),
                e_ (*doc_->getDocumentElement ())
          {
            e_.setUserData (user_data_keys::node, &n, 0);
          }

        private:
          xml::dom::auto_ptr<xercesc::DOMDocument> doc_;
          xercesc::DOMElement& e_;
        };


        struct dom_attribute_info: public dom_info
        {
          dom_attribute_info (xercesc::DOMAttr& a, type& n)
              : a_ (a)
          {
            a_.setUserData (user_data_keys::node, &n, 0);
          }

          virtual std::auto_ptr<dom_info>
          clone (type& tree_node, type* c) const
          {
            using std::auto_ptr;

            // Check if we are a document root
            //
            if (c == 0)
            {
              // We preserver DOM associations only in complete
              // copies from root
              //
              return auto_ptr<dom_info> (0);
            }

            // Check if our container does not have DOM association (e.g.,
            // because it wasn't a complete copy of the tree)
            //
            using xercesc::DOMNode;

            DOMNode* cn (c->_node ());

            if (cn == 0)
              return auto_ptr<dom_info> (0);

            // We are going to find the corresponding attribute in
            // the new tree
            //
            using xercesc::DOMAttr;
            using xercesc::DOMElement;
            using xercesc::DOMNamedNodeMap;

            DOMElement& p (*a_.getOwnerElement ());
            DOMNamedNodeMap& nl (*p.getAttributes ());

            XMLSize_t size (nl.getLength ()), i (0);

            // We should have at least one child
            //
            assert (size != 0);

            for ( ;i < size && !a_.isSameNode (nl.item (i)); ++i);

            // a_ should be in the list
            //
            assert (i < size);

            DOMNode& n (*cn->getAttributes ()->item (i));
            assert (n.getNodeType () == DOMNode::ATTRIBUTE_NODE);

            return auto_ptr<dom_info> (
              new dom_attribute_info (static_cast<DOMAttr&> (n), tree_node));
          }

          virtual xercesc::DOMNode*
          node ()
          {
            return &a_;
          }

        private:
          xercesc::DOMAttr& a_;
        };

        // For Sun C++ 5.6
        //
        struct dom_info_factory;
        friend struct _type::dom_info_factory;

        struct dom_info_factory
        {
          static std::auto_ptr<dom_info>
          create (const xercesc::DOMElement& e, type& n, bool root)
          {
            return std::auto_ptr<dom_info> (
              new dom_element_info (
                const_cast<xercesc::DOMElement&> (e), n, root));
          }

          static std::auto_ptr<dom_info>
          create (const xercesc::DOMAttr& a, type& n)
          {
            return std::auto_ptr<dom_info> (
              new dom_attribute_info (
                const_cast<xercesc::DOMAttr&> (a), n));
          }
        };


        std::auto_ptr<dom_info> dom_info_;


        // ID/IDREF map
        //
      private:
        struct identity_comparator
        {
          bool operator () (const identity* x, const identity* y) const
          {
            return x->before (*y);
          }
        };

        typedef
        std::map<const identity*, type*, identity_comparator>
        map;

        std::auto_ptr<map> map_;

      private:
        type* container_;
      };


      // anySimpleType
      //
      template <typename B>
      class simple_type: public B
      {
      public:
        simple_type ()
        {
        }

        simple_type (const xercesc::DOMElement&,
                     flags = 0,
                     type* container = 0);

        simple_type (const xercesc::DOMAttr&,
                     flags = 0,
                     type* container = 0);

        template <typename C>
        simple_type (const std::basic_string<C>&,
                     const xercesc::DOMElement*,
                     flags = 0,
                     type* container = 0);

        template <typename S>
        simple_type (istream<S>&, flags = 0, type* container = 0);

        simple_type (const simple_type&, flags = 0, type* container = 0);

      public:
        virtual simple_type*
        _clone (flags = 0, type* container = 0) const;
      };


      //
      //
      template <typename T, typename C>
      struct traits
      {
        typedef T type;

        static std::auto_ptr<T>
        create (const xercesc::DOMElement& e, flags f, tree::type* container)
        {
          return std::auto_ptr<T> (new T (e, f, container));
        }

        static std::auto_ptr<T>
        create (const xercesc::DOMAttr& a, flags f, tree::type* container)
        {
          return std::auto_ptr<T> (new T (a, f, container));
        }

        static std::auto_ptr<T>
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement* e,
                flags f,
                tree::type* container)
        {
          return std::auto_ptr<T> (new T (s, e, f, container));
        }
      };


      // Fundamental base template
      //
      template <typename X, typename C, typename B>
      class fundamental_base: public B
      {
      public:
        fundamental_base ()
            : x_ ()
        {
        }

        fundamental_base (X x)
            : x_ (x)
        {
        }

        fundamental_base (const fundamental_base& other,
                          flags f = 0,
                          type* container = 0)
            : B (other, f, container),
              x_ (other.x_)
        {
        }

        fundamental_base&
        operator= (const X& x)
        {
          if (&x_ != &x)
            x_ = x;

          return *this;
        }

      public:
        fundamental_base (const xercesc::DOMElement&,
                          flags = 0,
                          type* container = 0);

        fundamental_base (const xercesc::DOMAttr&,
                          flags = 0,
                          type* container = 0);

        fundamental_base (const std::basic_string<C>&,
                          const xercesc::DOMElement*,
                          flags = 0,
                          type* container = 0);

        template <typename S>
        fundamental_base (istream<S>&, flags = 0, type* container = 0);

      public:
        operator const X& () const
        {
          return x_;
        }

        operator X& ()
        {
          return x_;
        }

        // A call to one of the following operators causes ICE on VC++ 7.1
        // Refer to the following discussion for details:
        //
        // http://codesynthesis.com/pipermail/xsd-users/2006-June/000399.html
        //
#if defined(_MSC_VER) && _MSC_VER >= 1400
        template <typename Y>
        operator Y () const
        {
          return x_;
        }

        template <typename Y>
        operator Y ()
        {
          return x_;
        }
#endif

      public:
        virtual fundamental_base*
        _clone (flags = 0, type* container = 0) const;

      private:
        X x_;
      };

      // While this operators are not normally necessary, they
      // help resolve ambiguities between implicit conversion and
      // construction
      //
      template <typename X, typename C, typename B>
      inline bool
      operator== (const fundamental_base<X, C, B>& x,
                  const fundamental_base<X, C, B>& y)
      {
        X x_ (x);
        X y_ (y);
        return x_ == y_;
      }

      template <typename X, typename C, typename B>
      inline bool
      operator!= (const fundamental_base<X, C, B>& x,
                  const fundamental_base<X, C, B>& y)
      {
        X x_ (x);
        X y_ (y);
        return x_ != y_;
      }

      // Comparator for enum tables
      //
      template <typename C>
      struct enum_comparator
      {
        enum_comparator (const C* const* table)
            : table_ (table)
        {
        }

        bool
        operator() (std::size_t i, const std::basic_string<C>& s) const
        {
          return table_[i] < s;
        }

        bool
        operator() (const std::basic_string<C>& s, std::size_t i) const
        {
          return s < table_[i];
        }

        bool
        operator() (std::size_t i, std::size_t j) const
        {
          return std::basic_string<C> (table_[i]) < table_[j];
        }

      private:
        const C* const* table_;
      };
    }
  }
}

#include <xsd/cxx/tree/elements.txx>

#endif  // XSD_CXX_TREE_ELEMENTS_HXX

// Traits for C++ fundamental types
//
#include <xsd/cxx/tree/traits.hxx>
