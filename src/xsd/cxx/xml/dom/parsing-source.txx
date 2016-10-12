// file      : xsd/cxx/xml/dom/parsing-source.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

# include <xercesc/dom/DOMLSParser.hpp>
# include <xercesc/dom/DOMConfiguration.hpp>

# include <xercesc/dom/DOMNamedNodeMap.hpp>
# include <xercesc/dom/DOMImplementation.hpp>
# include <xercesc/dom/DOMImplementationLS.hpp>
# include <xercesc/dom/DOMImplementationRegistry.hpp>

#include <xercesc/util/XMLUni.hpp>     // xercesc::fg*
#include <xercesc/util/XMLUniDefs.hpp> // chLatin_L, etc

#include <xsd/cxx/xml/string.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        // parser
        //
        template <typename C>
        parser<C>::
        parser (const xercesc::DOMElement& e)
            : element_ (e),
              next_element_ (e.getFirstChild ()),
              a_ (e.getAttributes ()), ai_ (0)
        {
          using xercesc::DOMNode;

          for (; next_element_ != 0 &&
                 next_element_->getNodeType () != DOMNode::ELEMENT_NODE;
               next_element_ = next_element_->getNextSibling ());
        }

        template <typename C>
        void parser<C>::
        next_element ()
        {
          using xercesc::DOMNode;

          for (next_element_ = next_element_->getNextSibling ();
               next_element_ != 0 &&
                 next_element_->getNodeType () != DOMNode::ELEMENT_NODE;
               next_element_ = next_element_->getNextSibling ());
        }

        // parse()
        //

        template <typename C>
        auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMLSInput& is,
               const properties<C>& prop,
               ::xml_schema::flags flags)
        {
          using xercesc::XMLUni;

          // Instantiate the DOM parser
          //
          const XMLCh lsId[] = { xercesc::chLatin_L ,
                                 xercesc::chLatin_S ,
                                 xercesc::chNull } ;

          // Get an implementation of the Load-Store (LS) interface
          //
          xercesc::DOMImplementationLS* lsImpl = ( xercesc::DOMImplementationLS* ) xercesc::DOMImplementationRegistry::getDOMImplementation ( lsId );

          // Create a DOMLSParser
          //
          auto_ptr< xercesc::DOMLSParser > parser ( lsImpl->createLSParser( xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0 ) ) ;

          xercesc::DOMConfiguration * config = parser->getDomConfig();

          // Discard comment nodes in the document
          //
          config->setParameter ( XMLUni::fgDOMComments, false );

          // Enable datatype normalization
          //
          config->setParameter ( XMLUni::fgDOMDatatypeNormalization, true );

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created
          //
          config->setParameter ( XMLUni::fgDOMEntities, false );

          // Perform Namespace processing
          //
          config->setParameter ( XMLUni::fgDOMNamespaces, true );

          // Do not include ignorable whitespace in the DOM tree
          //
          config->setParameter ( XMLUni::fgDOMElementContentWhitespace, false );

          if ( flags & dont_validate )
          {
            config->setParameter ( XMLUni::fgDOMValidate, false );
            config->setParameter ( XMLUni::fgXercesSchema, false );
            config->setParameter ( XMLUni::fgXercesSchemaFullChecking, false );
          }
          else
          {
            config->setParameter ( XMLUni::fgDOMValidate, true );
            config->setParameter ( XMLUni::fgXercesSchema, true );

            // This feature checks the schema grammar for additional
            // errors. We most likely do not need it when validating
            // instances (assuming the schema is valid)
            //
            config->setParameter ( XMLUni::fgXercesSchemaFullChecking, false );
          }

          // We will release DOM ourselves
          //
          config->setParameter ( XMLUni::fgXercesUserAdoptsDOMDocument, true );

          // Transfer properies if any
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            config->setParameter (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            config->setParameter (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          try {
            auto_ptr< xercesc::DOMDocument > doc ( parser->parse ( &is ) );
            return doc;
          } catch ( xercesc::DOMException e ) { }

          auto_ptr< xercesc::DOMDocument > doc ( NULL );
          doc.reset ();
          return doc;
        }

        template < typename C >
        auto_ptr< xercesc::DOMDocument >
        parse ( const std::basic_string< C >& uri,
                const properties< C >& prop,
                ::xml_schema::flags flags )
        {
          fprintf ( stdout, "parse \"%s\"\n", uri.c_str () );

          using xercesc::XMLUni;

          const XMLCh lsId[] = { xercesc::chLatin_L,
                                 xercesc::chLatin_S,
                                 xercesc::chNull } ;

          // Get an implementation of the Load-Store (LS) interface
          //
          xercesc::DOMImplementationLS* lsImpl = ( xercesc::DOMImplementationLS* )( xercesc::DOMImplementationRegistry::getDOMImplementation ( lsId ) );

          // Create a DOMLSParser
          //
          auto_ptr< xercesc::DOMLSParser > parser ( lsImpl->createLSParser( xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0 ) );

          xercesc::DOMConfiguration * config = parser->getDomConfig();

          // Discard comment nodes in the document
          //
          config->setParameter ( XMLUni::fgDOMComments, false );

          // Enable datatype normalization
          //
          config->setParameter ( XMLUni::fgDOMDatatypeNormalization, true );

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created
          //
          config->setParameter ( XMLUni::fgDOMEntities, false );

          // Perform Namespace processing
          //
          config->setParameter ( XMLUni::fgDOMNamespaces, true );

          // Do not include ignorable whitespace in the DOM tree
          //
          config->setParameter ( XMLUni::fgDOMElementContentWhitespace, false );

          if ( flags & dont_validate )
          {
            config->setParameter ( XMLUni::fgDOMValidate, false );
            config->setParameter ( XMLUni::fgXercesSchema, false );
            config->setParameter ( XMLUni::fgXercesSchemaFullChecking, false );
          }
          else
          {
            config->setParameter ( XMLUni::fgDOMValidate, true );
            config->setParameter ( XMLUni::fgXercesSchema, true );

            // This feature checks the schema grammar for additional
            // errors. We most likely do not need it when validating
            // instances (assuming the schema is valid)
            //
            config->setParameter ( XMLUni::fgXercesSchemaFullChecking, false );
          }

          // We will release DOM ourselves
          //
          config->setParameter ( XMLUni::fgXercesUserAdoptsDOMDocument, true );

          // Transfer properies if any
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            config->setParameter (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            config->setParameter (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          try {
            auto_ptr< xercesc::DOMDocument > doc ( parser->parseURI (string (uri).c_str ()) );
            return doc;
          } catch ( xercesc::DOMException e ) { }

          auto_ptr< xercesc::DOMDocument > doc ( NULL );
          doc.reset ();
          return doc;
        }

      }
    }
  }
}
