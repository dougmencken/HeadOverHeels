// file      : xsd/cxx/tree/text.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xercesc/dom/DOMText.hpp>

#include <xsd/cxx/xml/string.hxx>

#include <xsd/cxx/tree/exceptions.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <typename C>
      std::basic_string<C>
      text_content (const xercesc::DOMElement& e)
      {
        std::basic_string<C> r;

        using xercesc::DOMNode;
        using xercesc::DOMText;

        for (DOMNode* n (e.getFirstChild ());
             n != 0;
             n = n->getNextSibling ())
        {
          switch (n->getNodeType ())
          {
          case DOMNode::TEXT_NODE:
          case DOMNode::CDATA_SECTION_NODE:
            {
              DOMText* t (static_cast<DOMText*> (n));
              r += xml::transcode<C> (t->getData ());
              break;
            }
          case DOMNode::ELEMENT_NODE:
            {
              throw expected_text_content<C> ();
            }
          }
        }

        return r;
      }
    }
  }
}

