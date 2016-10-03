// file      : xsd/cxx/xml/dom/parsing-source.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>

#include <xercesc/util/XMLUni.hpp>     // xercesc::fg*
#include <xercesc/util/XMLUniDefs.hpp> // chLatin_L, etc

#include <xsd/cxx/xml/string.hxx>
#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>

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
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource& is,
               error_handler<C>& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          bits::error_handler_proxy<C> ehp (eh);
          return xml::dom::parse (is, ehp, prop, flags);
        }

        template <typename C>
        auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource& is,
               xercesc::DOMErrorHandler& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          // HP aCC cannot handle using namespace xercesc;
          //
          using xercesc::DOMImplementationRegistry;
          using xercesc::DOMImplementationLS;
          using xercesc::DOMImplementation;
          using xercesc::DOMDocument;
          using xercesc::DOMBuilder;
          using xercesc::XMLUni;


          // Instantiate the DOM parser.
          //
          const XMLCh ls_id[] = {xercesc::chLatin_L,
                                 xercesc::chLatin_S,
                                 xercesc::chNull};

          // Get an implementation of the Load-Store (LS) interface.
          //
          DOMImplementation* impl (
            DOMImplementationRegistry::getDOMImplementation (ls_id));

          // Create a DOMBuilder.
          //
          auto_ptr<DOMBuilder> parser (
            impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

          // Discard comment nodes in the document.
          //
          parser->setFeature (XMLUni::fgDOMComments, false);

          // Enable datatype normalization.
          //
          parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created
          //
          parser->setFeature (XMLUni::fgDOMEntities, false);

          // Perform Namespace processing.
          //
          parser->setFeature (XMLUni::fgDOMNamespaces, true);

          // Do not include ignorable whitespace in the DOM tree.
          //
          parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

          if (flags & dont_validate)
          {
            parser->setFeature (XMLUni::fgDOMValidation, false);
            parser->setFeature (XMLUni::fgXercesSchema, false);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
          }
          else
          {
            parser->setFeature (XMLUni::fgDOMValidation, true);
            parser->setFeature (XMLUni::fgXercesSchema, true);

            // This feature checks the schema grammar for additional
            // errors. We most likely do not need it when validating
            // instances (assuming the schema is valid)
            //
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
          }

          // We will release DOM ourselves.
          //
          parser->setFeature (XMLUni::fgXercesUserAdoptsDOMDocument, true);


          // Transfer properies if any.
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          // Set error handler.
          //
          bits::error_handler_proxy<C> ehp (eh);
          parser->setErrorHandler (&ehp);

          auto_ptr<DOMDocument> doc (parser->parse (is));

          if (ehp.failed ())
            doc.reset ();

          return doc;
        }

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               error_handler<C>& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          bits::error_handler_proxy<C> ehp (eh);
          return xml::dom::parse (uri, ehp, prop, flags);
        }

        template <typename C>
        auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               xercesc::DOMErrorHandler& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          // HP aCC cannot handle using namespace xercesc;
          //
          using xercesc::DOMImplementationRegistry;
          using xercesc::DOMImplementationLS;
          using xercesc::DOMImplementation;
          using xercesc::DOMDocument;
          using xercesc::DOMBuilder;
          using xercesc::XMLUni;


          // Instantiate the DOM parser.
          //
          const XMLCh ls_id[] = {xercesc::chLatin_L,
                                 xercesc::chLatin_S,
                                 xercesc::chNull};

          // Get an implementation of the Load-Store (LS) interface.
          //
          DOMImplementation* impl (
            DOMImplementationRegistry::getDOMImplementation (ls_id));

          // Create a DOMBuilder.
          //
          auto_ptr<DOMBuilder> parser (
            impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

          // Discard comment nodes in the document.
          //
          parser->setFeature (XMLUni::fgDOMComments, false);

          // Enable datatype normalization.
          //
          parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created
          //
          parser->setFeature (XMLUni::fgDOMEntities, false);

          // Perform Namespace processing.
          //
          parser->setFeature (XMLUni::fgDOMNamespaces, true);

          // Do not include ignorable whitespace in the DOM tree.
          //
          parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

          if (flags & dont_validate)
          {
            parser->setFeature (XMLUni::fgDOMValidation, false);
            parser->setFeature (XMLUni::fgXercesSchema, false);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
          }
          else
          {
            parser->setFeature (XMLUni::fgDOMValidation, true);
            parser->setFeature (XMLUni::fgXercesSchema, true);

            // This feature checks the schema grammar for additional
            // errors. We most likely do not need it when validating
            // instances (assuming the schema is valid)
            //
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
          }

          // We will release DOM ourselves.
          //
          parser->setFeature (XMLUni::fgXercesUserAdoptsDOMDocument, true);

          // Transfer properies if any.
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          // Set error handler.
          //
          bits::error_handler_proxy<C> ehp (eh);
          parser->setErrorHandler (&ehp);

          auto_ptr<DOMDocument> doc (
            parser->parseURI (string (uri).c_str ()));

          if (ehp.failed ())
            doc.reset ();

          return doc;
        }
      }
    }
  }
}

