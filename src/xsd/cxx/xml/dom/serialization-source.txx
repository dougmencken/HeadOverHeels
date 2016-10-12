// file      : xsd/cxx/xml/dom/serialization-source.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xercesc/util/XMLUni.hpp>     // xercesc::fg*
#include <xercesc/util/XMLUniDefs.hpp> // chLatin_L, etc

# include <xercesc/dom/DOMLSSerializer.hpp>
# include <xercesc/dom/DOMLSOutput.hpp>

#include <xercesc/dom/DOMElement.hpp>

# include <xercesc/dom/DOMImplementationLS.hpp>
# include <xercesc/dom/DOMImplementation.hpp>
# include <xercesc/dom/DOMImplementationRegistry.hpp>

#include <xsd/cxx/xml/string.hxx>
#include <xsd/cxx/xml/bits/literals.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        //
        //
        template <typename C>
        xercesc::DOMAttr&
        create_attribute (const C* name, xercesc::DOMElement& parent)
        {
          xercesc::DOMDocument* doc (parent.getOwnerDocument ());
          xercesc::DOMAttr* a (doc->createAttribute (string (name).c_str ()));
          parent.setAttributeNode (a);
          return *a;
        }

        template <typename C>
        xercesc::DOMAttr&
        create_attribute (const C* name,
                          const C* ns,
                          xercesc::DOMElement& parent)
        {
          if (ns[0] == C ('\0'))
            return create_attribute (name, parent);

          xercesc::DOMDocument* doc (parent.getOwnerDocument ());

          xercesc::DOMAttr* a;
          std::basic_string<C> p (prefix<C> (ns, parent));

          if (!p.empty ())
          {
            p += ':';
            p += name;
            a = doc->createAttributeNS (string (ns).c_str (),
                                        string (p).c_str ());
          }
          else
            a = doc->createAttributeNS (string (ns).c_str (),
                                        string (name).c_str ());

          parent.setAttributeNodeNS (a);
          return *a;
        }

        template <typename C>
        xercesc::DOMElement&
        create_element (const C* name, xercesc::DOMElement& parent)
        {
          xercesc::DOMDocument* doc (parent.getOwnerDocument ());
          xercesc::DOMElement* e (doc->createElement (string (name).c_str ()));
          parent.appendChild (e);
          return *e;
        }

        template <typename C>
        xercesc::DOMElement&
        create_element (const C* name,
                        const C* ns,
                        xercesc::DOMElement& parent)
        {
          if (ns[0] == C ('\0'))
            return create_element (name, parent);

          xercesc::DOMDocument* doc (parent.getOwnerDocument ());

          xercesc::DOMElement* e;
          std::basic_string<C> p (prefix<C> (ns, parent));

          if (!p.empty ())
          {
            p += ':';
            p += name;
            e = doc->createElementNS (string (ns).c_str (),
                                      string (p).c_str ());
          }
          else
            e = doc->createElementNS (string (ns).c_str (),
                                      string (name).c_str ());

          parent.appendChild (e);
          return *e;
        }


        //
        //
        template < typename C >
        auto_ptr< xercesc::DOMDocument >
        serialize ( const std::basic_string< C >& el,
                    const std::basic_string< C >& ns,
                    const namespace_infomap< C >& map,
                    unsigned long )
        {
          typedef std::basic_string< C > string;
          typedef namespace_infomap< C > infomap;
          typedef typename infomap::const_iterator infomap_iterator;

          C colon ( ':' ), space ( ' ' );

          string prefix;

          if (!ns.empty ())
          {
            infomap_iterator i ( map.begin () ), e ( map.end () );

            for ( ;i != e; ++i)
            {
              if (i->second.name == ns)
              {
                prefix = i->first;
                break;
              }
            }

            if (i == e)
              throw mapping<C> (ns);
          }

          const XMLCh lsId [] = {  xercesc::chLatin_L ,
                                   xercesc::chLatin_S ,
                                   xercesc::chNull  } ;

          xercesc::DOMImplementation * impl = xercesc::DOMImplementationRegistry::getDOMImplementation ( lsId );

          string nameOfDoc = prefix.empty () ? string ( el ) : string ( prefix + colon + el ) ;
          fprintf ( stdout, "serialize \"%s\"\n", nameOfDoc.c_str () );

          auto_ptr< xercesc::DOMDocument > doc (
            impl->createDocument (
              (ns.empty () ? 0 : xml::string (ns).c_str ()),
              xml::string ( nameOfDoc ).c_str (),
              0));

          xercesc::DOMElement* root (doc->getDocumentElement ());

          // Check if we need to provide xsi mapping.
          //
          bool xsi (false);
          string xsi_prefix (xml::bits::xsi_prefix<C> ());
          string xmlns_prefix (xml::bits::xmlns_prefix<C> ());

          for (infomap_iterator i (map.begin ()), e (map.end ()); i != e; ++i)
          {
            if (!i->second.schema.empty ())
            {
              xsi = true;
              break;
            }
          }

          // Check if we were told to provide xsi mapping.
          //
          if (xsi)
          {
            for (infomap_iterator i (map.begin ()), e (map.end ());
                 i != e;
                 ++i)
            {
              if (i->second.name == xml::bits::xsi_namespace<C> ())
              {
                xsi_prefix = i->first;
                xsi = false;
                break;
              }
            }

            if (xsi)
            {
              // If we were not told to provide xsi mapping, make sure our
              // prefix does not conflict with user-defined prefixes.
              //
              infomap_iterator i (map.find (xsi_prefix));

              if (i != map.end ())
                throw xsi_already_in_use ();
            }
          }

          // Create xmlns:xsi attribute.
          //
          if (xsi)
          {
            root->setAttributeNS (
              xml::string (xml::bits::xmlns_namespace<C> ()).c_str (),
              xml::string (xmlns_prefix + colon + xsi_prefix).c_str (),
              xml::string (xml::bits::xsi_namespace<C> ()).c_str ());
          }


          // Create user-defined mappings.
          //
          for (infomap_iterator i (map.begin ()), e (map.end ()); i != e; ++i)
          {
            if (i->first.empty ())
            {
              // Empty prefix.
              //
              if (!i->second.name.empty ())
                root->setAttributeNS (
                  xml::string (xml::bits::xmlns_namespace<C> ()).c_str (),
                  xml::string (xmlns_prefix).c_str (),
                  xml::string (i->second.name).c_str ());
            }
            else
            {
              root->setAttributeNS (
                xml::string (xml::bits::xmlns_namespace<C> ()).c_str (),
                xml::string (xmlns_prefix + colon + i->first).c_str (),
                xml::string (i->second.name).c_str ());
            }
          }

          // Create xsi:schemaLocation and xsi:noNamespaceSchemaLocation
          // attributes.
          //
          string schema_location;
          string no_namespace_schema_location;

          for (infomap_iterator i (map.begin ()), e (map.end ()); i != e; ++i)
          {
            if (!i->second.schema.empty ())
            {
              if (i->second.name.empty ())
              {
                if (!no_namespace_schema_location.empty ())
                  no_namespace_schema_location += space;

                no_namespace_schema_location += i->second.schema;
              }
              else
              {
                if (!schema_location.empty ())
                  schema_location += space;

                schema_location += i->second.name + space + i->second.schema;
              }
            }
          }

          if (!schema_location.empty ())
          {
            root->setAttributeNS (
              xml::string (xml::bits::xsi_namespace<C> ()).c_str (),
              xml::string (xsi_prefix + colon +
                           xml::bits::schema_location<C> ()).c_str (),
              xml::string (schema_location).c_str ());
          }

          if (!no_namespace_schema_location.empty ())
          {
            root->setAttributeNS (
              xml::string (xml::bits::xsi_namespace<C> ()).c_str (),
              xml::string (
                xsi_prefix + colon +
                xml::bits::no_namespace_schema_location<C> ()).c_str (),
              xml::string (no_namespace_schema_location).c_str ());
          }

          return doc;
        }


        template < typename C >
        bool
        serialize ( xercesc::XMLFormatTarget& destination,
                    const xercesc::DOMDocument& doc,
                    const std::basic_string< C >& encoding,
                    unsigned long flags )
        {
          using xercesc::XMLUni;

          const XMLCh lsId [] = {  xercesc::chLatin_L,
                                   xercesc::chLatin_S,
                                   xercesc::chNull  } ;

          xercesc::DOMImplementationLS * lsImpl = ( xercesc::DOMImplementationLS * ) xercesc::DOMImplementationRegistry::getDOMImplementation ( lsId );

          // Get an instance of DOMLSSerializer
          //
          xml::dom::auto_ptr< xercesc::DOMLSSerializer > writer ( lsImpl->createLSSerializer () );
          xercesc::DOMConfiguration * config = writer->getDomConfig();

          // Set some nice features if the serializer supports them
          //
          if ( config->canSetParameter ( XMLUni::fgDOMWRTDiscardDefaultContent, true ) )
            config->setParameter ( XMLUni::fgDOMWRTDiscardDefaultContent, true );

          if ( config->canSetParameter ( XMLUni::fgDOMWRTFormatPrettyPrint, true ) )
            config->setParameter ( XMLUni::fgDOMWRTFormatPrettyPrint, true );

          // See if we need to write XML declaration
          //
          if ( flags & no_xml_declaration )
          {
            if ( config->canSetParameter ( XMLUni::fgDOMXMLDeclaration, false ) )
              config->setParameter ( XMLUni::fgDOMXMLDeclaration, false );
          }

          xml::dom::auto_ptr< xercesc::DOMLSOutput > out ( lsImpl->createLSOutput () ) ;
          out->setEncoding ( xml::string (encoding).c_str () );
          out->setByteStream ( &destination );

          bool okay = writer->write ( &doc, out.get () );
          return okay;
        }

      }
    }
  }
}
