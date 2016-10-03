// file      : xsd/cxx/tree/buffer.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_BUFFER_HXX
#define XSD_CXX_TREE_BUFFER_HXX

#include <new>     // operator new/delete
#include <cstddef> // std::size_t
#include <cstring> // std::memcpy, std::memcmp

#include <xsd/cxx/tree/exceptions.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      class buffer_base
      {
      protected:
        virtual
        ~buffer_base ()
        {
          operator delete (data_);
        }

        buffer_base ()
            : data_ (0), size_ (0), capacity_ (0)
        {
        }

      protected:
        char* data_;
        size_t size_;
        size_t capacity_;
      };

      // Note that the template parameter is only used to instantiate
      // exception types. The underlying buffer type is always 'char'
      //
      template<typename C>
      class buffer: protected buffer_base
      {
      public:
        typedef std::size_t size_t;

      public:
        explicit
        buffer (size_t size = 0);
        buffer (size_t size, size_t capacity);
        buffer (const void* data, size_t size);
        buffer (const void* data, size_t size, size_t capacity);

        // If the assume_ownership argument is true, the buffer will
        // assume the ownership of the data and will release the memory
        // by calling operator delete ()
        //
        buffer (void* data,
                size_t size,
                size_t capacity,
                bool assume_ownership);

        buffer (const buffer&);

      public:
        buffer&
        operator= (const buffer&);

      public:
        size_t
        capacity () const
	{
	  return capacity_;
	}

        // Returns true if the underlying buffer has moved
        //
        bool
        capacity (size_t capacity)
        {
          return this->capacity (capacity, true);
        }

      public:
        size_t
        size () const {return size_;}

        // Returns true if the underlying buffer has moved
        //
        bool
        size (size_t size)
        {
          bool r (false);

          if (size > capacity_)
            r = capacity (size);

          size_ = size;

          return r;
        }

      public:
        const char*
        data () const {return data_;}

        char*
        data () {return data_;}

        const char*
        begin () const {return data_;}

        char*
        begin () {return data_;}

        const char*
        end () const {return data_ + size_;}

        char*
        end () {return data_ + size_;}

      public:
        void
        swap (buffer& other);

      private:
        bool
        capacity (size_t capacity, bool copy);
      };

      template <typename C>
      inline bool
      operator== (const buffer<C>& a, const buffer<C>& b)
      {
        return a.size () == b.size () &&
          std::memcmp (a.data (), b.data (), a.size ()) == 0;
      }

      template <typename C>
      inline bool
      operator!= (const buffer<C>& a, const buffer<C>& b)
      {
        return !(a == b);
      }
    }
  }
}

#include <xsd/cxx/tree/buffer.txx>

#endif  // XSD_CXX_TREE_BUFFER_HXX
