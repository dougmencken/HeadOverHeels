// file      : xsd/cxx/parser/validating/xml-schema-pimpl.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <limits>

#include <xsd/cxx/zc-istream.hxx>
#include <xsd/cxx/parser/validating/exceptions.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {
        // Note that most of the types implemented here cannot have
        // whitespaces in the value. As result we don't need to waste
        // time collapsing whitespaces. All we need to do is trim the
        // string representation which can be done without copying.
        //

        // Character table.
        //
        namespace bits
        {
          const unsigned char ncname_mask     = 0x1;
          const unsigned char name_first_mask = 0x2;
          const unsigned char name_mask       = 0x4;

          template <typename C>
          struct char_table
          {
            static C table[0x80];
          };

          template <typename C>
          C char_table<C>::table[0x80] =
          {
            0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0xD0, 0x00, 0x00, 0xD0, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xD8, 0x48, 0x58, 0x48, 0x48, 0x48, 0x40, 0x58, 0x48, 0x48, 0x48, 0x48, 0x48, 0x4D, 0x4D, 0x58,
            0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4D, 0x4E, 0x48, 0x50, 0x48, 0x58, 0x48,
            0x48, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F,
            0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x48, 0x48, 0x40, 0x48, 0x4F,
            0x48, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F,
            0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x48, 0x48, 0x48, 0x48, 0x48
          };
        }

        // any_type
        //

        template <typename C>
        void any_type_pimpl<C>::
        post_any_type ()
        {
        }

        // any_simple_type
        //

        template <typename C>
        void any_simple_type_pimpl<C>::
        post_any_simple_type ()
        {
        }

        // boolean
        //

        template <typename C>
        void boolean_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void boolean_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void boolean_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          if (str == bits::true_<C> () || str == bits::one<C> ())
            value_ = true;
          else if (str == bits::false_<C> () || str == bits::zero<C> ())
            value_ = false;
          else
            throw invalid_value<C> (bits::boolean<C> (), str);
        }

        template <typename C>
        bool boolean_pimpl<C>::
        post_boolean ()
        {
          return value_;
        }

        // byte
        //

        template <typename C>
        void byte_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void byte_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void byte_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          short t;
          zc_istream<C> is (str);

          if (is >> t && is.exhausted () && t >= -128 && t <= 127)
            value_ = static_cast<signed char> (t);
          else
            throw invalid_value<C> (bits::byte<C> (), str);
        }

        template <typename C>
        signed char byte_pimpl<C>::
        post_byte ()
        {
          return value_;
        }

        // unsigned_byte
        //

        template <typename C>
        void unsigned_byte_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void unsigned_byte_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void unsigned_byte_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          unsigned short t;
          zc_istream<C> is (str);

          if (is >> t && is.exhausted () && t <= 255)
            value_ = static_cast<unsigned char> (t);
          else
            throw invalid_value<C> (bits::unsigned_byte<C> (), str);
        }

        template <typename C>
        unsigned char unsigned_byte_pimpl<C>::
        post_unsigned_byte ()
        {
          return value_;
        }

        // short
        //

        template <typename C>
        void short_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void short_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void short_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::short_<C> (), str);
        }

        template <typename C>
        short short_pimpl<C>::
        post_short ()
        {
          return value_;
        }


        // unsigned_short
        //

        template <typename C>
        void unsigned_short_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void unsigned_short_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void unsigned_short_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::unsigned_short<C> (), str);
        }

        template <typename C>
        unsigned short unsigned_short_pimpl<C>::
        post_unsigned_short ()
        {
          return value_;
        }

        // int
        //

        template <typename C>
        void int_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void int_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void int_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::int_<C> (), str);
        }

        template <typename C>
        int int_pimpl<C>::
        post_int ()
        {
          return value_;
        }


        // unsigned_int
        //

        template <typename C>
        void unsigned_int_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void unsigned_int_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void unsigned_int_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::unsigned_int<C> (), str);
        }

        template <typename C>
        unsigned int unsigned_int_pimpl<C>::
        post_unsigned_int ()
        {
          return value_;
        }


        // long
        //
        template <typename C>
        void long_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void long_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void long_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::long_<C> (), str);
        }

        template <typename C>
        long long long_pimpl<C>::
        post_long ()
        {
          return value_;
        }

        // unsigned_long
        //
        template <typename C>
        void unsigned_long_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void unsigned_long_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void unsigned_long_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::unsigned_long<C> (), str);
        }

        template <typename C>
        unsigned long long unsigned_long_pimpl<C>::
        post_unsigned_long ()
        {
          return value_;
        }


        // integer
        //
        template <typename C>
        void integer_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void integer_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void integer_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::integer<C> (), str);
        }

        template <typename C>
        long long integer_pimpl<C>::
        post_integer ()
        {
          return value_;
        }

        // negative_integer
        //
        template <typename C>
        void negative_integer_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void negative_integer_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void negative_integer_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted () && value_ < 0))
            throw invalid_value<C> (bits::negative_integer<C> (), str);
        }

        template <typename C>
        long long negative_integer_pimpl<C>::
        post_negative_integer ()
        {
          return value_;
        }


        // non_positive_integer
        //
        template <typename C>
        void non_positive_integer_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void non_positive_integer_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void non_positive_integer_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted () && value_ <= 0))
            throw invalid_value<C> (bits::non_positive_integer<C> (), str);
        }

        template <typename C>
        long long non_positive_integer_pimpl<C>::
        post_non_positive_integer ()
        {
          return value_;
        }

        // positive_integer
        //
        template <typename C>
        void positive_integer_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void positive_integer_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void positive_integer_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted () && value_ > 0))
            throw invalid_value<C> (bits::positive_integer<C> (), str);
        }

        template <typename C>
        unsigned long long positive_integer_pimpl<C>::
        post_positive_integer ()
        {
          return value_;
        }


        // non_negative_integer
        //
        template <typename C>
        void non_negative_integer_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void non_negative_integer_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void non_negative_integer_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::non_negative_integer<C> (), str);
        }

        template <typename C>
        unsigned long long non_negative_integer_pimpl<C>::
        post_non_negative_integer ()
        {
          return value_;
        }


        // float
        //
        template <typename C>
        void float_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void float_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void float_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          if (str == bits::positive_inf<C> ())
            value_ = std::numeric_limits<float>::infinity ();
          else if (str == bits::negative_inf<C> ())
            value_ = -std::numeric_limits<float>::infinity ();
          else if (str == bits::nan<C> ())
            value_ = std::numeric_limits<float>::quiet_NaN ();
          else
          {
            zc_istream<C> is (str);

            if (!(is >> value_ && is.exhausted ()))
              throw invalid_value<C> (bits::float_<C> (), str);
          }
        }

        template <typename C>
        float float_pimpl<C>::
        post_float ()
        {
          return value_;
        }


        // double
        //
        template <typename C>
        void double_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void double_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void double_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          if (str == bits::positive_inf<C> ())
            value_ = std::numeric_limits<double>::infinity ();
          else if (str == bits::negative_inf<C> ())
            value_ = -std::numeric_limits<double>::infinity ();
          else if (str == bits::nan<C> ())
            value_ = std::numeric_limits<double>::quiet_NaN ();
          else
          {
            zc_istream<C> is (str);

            if (!(is >> value_ && is.exhausted ()))
              throw invalid_value<C> (bits::double_<C> (), str);
          }
        }

        template <typename C>
        double double_pimpl<C>::
        post_double ()
        {
          return value_;
        }

        // decimal
        //
        template <typename C>
        void decimal_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void decimal_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        void decimal_pimpl<C>::
        _post ()
        {
          ro_string<C> str (str_);
          trim (str);

          zc_istream<C> is (str);

          //@@ TODO: now we accept scientific notations and INF/NaN.
          //
          if (!(is >> value_ && is.exhausted ()))
            throw invalid_value<C> (bits::decimal<C> (), str);
        }

        template <typename C>
        double decimal_pimpl<C>::
        post_decimal ()
        {
          return value_;
        }

        // string
        //
        template <typename C>
        void string_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void string_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        std::basic_string<C> string_pimpl<C>::
        post_string ()
        {
          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // normalized_string
        //
        template <typename C>
        void normalized_string_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void normalized_string_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          str_ += s;
        }

        template <typename C>
        std::basic_string<C> normalized_string_pimpl<C>::
        post_normalized_string ()
        {
          typedef typename std::basic_string<C>::size_type size_type;

          size_type size (str_.size ());

          for (size_type i (0); i < size; ++i)
          {
            C& c = str_[i];

            if (c == C (0x0A) || c == C (0x0D) || c == C (0x09))
              c = C (0x20);
          }

          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // token
        //
        template <typename C>
        void token_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void token_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        std::basic_string<C> token_pimpl<C>::
        post_token ()
        {
          typedef typename std::basic_string<C>::size_type size_type;

          size_type size (str_.size ());
          size_type j (0);

          bool subs (false);

          for (size_type i (0); i < size; ++i)
          {
            C c = str_[i];

            if (c == C (0x20) || c == C (0x0A) ||
                c == C (0x0D) || c == C (0x09))
            {
              subs = true;
            }
            else
            {
              if (subs)
              {
                subs = false;
                str_[j++] = C (0x20);
              }

              str_[j++] = c;
            }
          }

          str_.resize (j);

          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // name
        //
        template <typename C>
        void name_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void name_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void name_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // For now we are only checking the US-ASCII characters.
          //

          bool ok (size != 0);

          if (ok)
          {
            unsigned int c (static_cast<unsigned int> (str_[0]));

            ok = c >= 0x80 ||
              (bits::char_table<unsigned char>::table[c] &
               bits::name_first_mask);

            if (ok)
            {
              for (size_type i (1); i < size; ++i)
              {
                c = static_cast<unsigned int> (str_[i]);

                if (c < 0x80 &&
                    !(bits::char_table<unsigned char>::table[c] &
                      bits::name_mask))
                {
                  ok = false;
                  break;
                }
              }
            }
          }

          if (!ok)
            throw invalid_value<C> (bits::name<C> (), tmp);

          str_.resize (size);
        }

        template <typename C>
        std::basic_string<C> name_pimpl<C>::
        post_name ()
        {
          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // nmtoken
        //
        template <typename C>
        void nmtoken_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void nmtoken_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void nmtoken_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // For now we are only checking the US-ASCII characters.
          //

          bool ok (size != 0);

          if (ok)
          {
            for (size_type i (0); i < size; ++i)
            {
              unsigned int c (static_cast<unsigned int> (str_[i]));

              if (c < 0x80 &&
                  !(bits::char_table<unsigned char>::table[c] &
                    bits::name_mask))
              {
                ok = false;
                break;
              }
            }
          }

          if (!ok)
            throw invalid_value<C> (bits::nmtoken<C> (), tmp);

          str_.resize (size);
        }

        template <typename C>
        std::basic_string<C> nmtoken_pimpl<C>::
        post_nmtoken ()
        {
          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // nmtokens
        //
        template <typename C>
        void nmtokens_pimpl<C>::
        _pre ()
        {
          seq_.clear ();
        }

        template <typename C>
        string_sequence<C> nmtokens_pimpl<C>::
        post_nmtokens ()
        {
          string_sequence<C> r;
          r.swap (seq_);
          return r;
        }

        template <typename C>
        void nmtokens_pimpl<C>::
        _xsd_parse_item (const ro_string<C>& s)
        {
          parser_.pre ();
          parser_._pre ();
          parser_._characters (s);
          parser_._post ();
          seq_.push_back (parser_.post_nmtoken ());
        }

        // ncname
        //
        namespace bits
        {
          template <typename C>
          bool
          valid_ncname (const C* s, typename ro_string<C>::size_type size)
          {
            typedef typename ro_string<C>::size_type size_type;

            // For now we are only checking the US-ASCII characters.
            //
            bool ok (size != 0);

            if (ok)
            {
              unsigned int c (static_cast<unsigned int> (s[0]));

              ok = c >= 0x80 ||
                ((bits::char_table<unsigned char>::table[c] &
                  bits::name_first_mask) && c != C (':'));

              if (ok)
              {
                for (size_type i (1); i < size; ++i)
                {
                  c = static_cast<unsigned int> (s[i]);

                  if (c < 0x80 &&
                      !(bits::char_table<unsigned char>::table[c] &
                        bits::ncname_mask))
                  {
                    ok = false;
                    break;
                  }
                }
              }
            }

            return ok;
          }
        }

        template <typename C>
        void ncname_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void ncname_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void ncname_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          if (!bits::valid_ncname (tmp.data (), size))
            throw invalid_value<C> (bits::ncname<C> (), tmp);

          str_.resize (size);
        }

        template <typename C>
        std::basic_string<C> ncname_pimpl<C>::
        post_ncname ()
        {
          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // id
        //
        template <typename C>
        std::basic_string<C> id_pimpl<C>::
        post_id ()
        {
          return this->post_ncname ();
        }

        // idref
        //
        template <typename C>
        std::basic_string<C> idref_pimpl<C>::
        post_idref ()
        {
          return this->post_ncname ();
        }

        // idrefs
        //
        template <typename C>
        void idrefs_pimpl<C>::
        _pre ()
        {
          seq_.clear ();
        }

        template <typename C>
        string_sequence<C> idrefs_pimpl<C>::
        post_idrefs ()
        {
          string_sequence<C> r;
          r.swap (seq_);
          return r;
        }

        template <typename C>
        void idrefs_pimpl<C>::
        _xsd_parse_item (const ro_string<C>& s)
        {
          parser_.pre ();
          parser_._pre ();
          parser_._characters (s);
          parser_._post ();
          seq_.push_back (parser_.post_idref ());
        }

        // language
        //
        template <typename C>
        void language_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void language_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void language_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // language := ALPHA{1,8} *(-(ALPHA | DIGIT){1,8})
          //
          bool ok (true);

          for (size_type tag (0), i (0); ; ++tag)
          {
            size_type n (0);

            for (; i < size && n < 8; ++n, ++i)
            {
              C c (tmp[i]);

              if (!((c >= C ('a') && c <= C ('z')) ||
                    (c >= C ('A') && c <= C ('Z')) ||
                    (tag != 0 && c >= C ('0') && c <= C ('9'))))
                break;
            }

            if (n == 0)
            {
              ok = false;
              break;
            }

            if (i == size)
              break;

            if (tmp[i++] != C ('-'))
            {
              ok = false;
              break;
            }
          }

          if (!ok)
            throw invalid_value<C> (bits::language<C> (), tmp);

          str_.resize (size);
        }

        template <typename C>
        std::basic_string<C> language_pimpl<C>::
        post_language ()
        {
          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // uri
        //
        template <typename C>
        void uri_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void uri_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        std::basic_string<C> uri_pimpl<C>::
        post_uri ()
        {
          // According to Datatypes 3.2.17 and RFC2396 pretty much anything
          // can be a URI and conforming processors do not need to figure
          // out and verify particular URI schemes.
          //
          ro_string<C> tmp (str_);
          str_.resize (trim_right (tmp));

          std::basic_string<C> r;
          r.swap (str_);
          return r;
        }

        // qname
        //
        template <typename C>
        void qname_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void qname_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void qname_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));
          size_type pos (tmp.find (C (':')));

          const C* s (tmp.data ());

          if (pos != ro_string<C>::npos)
          {
            if (!bits::valid_ncname (s, pos) ||
                !bits::valid_ncname (s + pos + 1, size - pos - 1))
              throw invalid_value<C> (bits::qname<C> (), tmp);

            prefix_.assign (s, pos);
            name_.assign (s + pos + 1, size - pos - 1);
          }
          else
          {
            if (!bits::valid_ncname (s, size))
              throw invalid_value<C> (bits::qname<C> (), tmp);

            prefix_.clear ();
            str_.resize (size);
            name_.swap (str_);
          }
        }

        template <typename C>
        qname<C> qname_pimpl<C>::
        post_qname ()
        {
          return prefix_.empty ()
            ? qname<C> (name_)
            : qname<C> (prefix_, name_);
        }

        // base64_binary
        //
        template <typename C>
        void base64_binary_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void base64_binary_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        namespace bits
        {
          template <typename C>
          inline unsigned char
          base64_decode (C c)
          {
            unsigned char r (0xFF);

            if (c >= C('A') && c <= C ('Z'))
              r = static_cast<unsigned char> (c - C ('A'));
            else if (c >= C('a') && c <= C ('z'))
              r = static_cast<unsigned char> (c - C ('a') + 26);
            else if (c >= C('0') && c <= C ('9'))
              r = static_cast<unsigned char> (c - C ('0') + 52);
            else if (c == C ('+'))
              r = 62;
            else if (c == C ('/'))
              r = 63;

            return r;
          }
        }

        template <typename C>
        void base64_binary_pimpl<C>::
        _post ()
        {
          typedef typename std::basic_string<C>::size_type size_type;

          size_type size (str_.size ());
          const C* src (str_.c_str ());

          // Remove all whitespaces.
          //
          {
            size_type j (0);

            bool subs (false);

            for (size_type i (0); i < size; ++i)
            {
              C c = str_[i];

              if (c == C (0x20) || c == C (0x0A) ||
                  c == C (0x0D) || c == C (0x09))
              {
                subs = true;
              }
              else
              {
                if (subs)
                  subs = false;

                str_[j++] = c;
              }
            }

            size = j;
            str_.resize (size);
          }

          // Our length should be a multiple of four.
          //
          if (size == 0 || size % 4 != 0)
            throw invalid_value<C> (bits::base64_binary<C> (), str_);

          size_type quad_count (size / 4);
          size_type capacity (quad_count * 3 + 1);

          buf_.reset (new buffer (capacity, capacity));
          char* dst (buf_->data ());

          size_type si (0), di (0); // Source and destination indexes.

          // Process all quads except the last one.
          //
          unsigned char b1, b2, b3, b4;

          for (size_type q (0); q < quad_count - 1; ++q)
          {
            b1 = bits::base64_decode (src[si++]);
            b2 = bits::base64_decode (src[si++]);
            b3 = bits::base64_decode (src[si++]);
            b4 = bits::base64_decode (src[si++]);

            if (b1 == 0xFF || b2 == 0xFF || b3 == 0xFF || b4 == 0xFF)
              throw invalid_value<C> (bits::base64_binary<C> (), str_);

            dst[di++] = (b1 << 2) | (b2 >> 4);
            dst[di++] = (b2 << 4) | (b3 >> 2);
            dst[di++] = (b3 << 6) | b4;
          }

          // Process the last quad. The first two octets are always there.
          //
          b1 = bits::base64_decode (src[si++]);
          b2 = bits::base64_decode (src[si++]);

          if (b1 == 0xFF || b2 == 0xFF)
            throw invalid_value<C> (bits::base64_binary<C> (), str_);

          C e3 (src[si++]);
          C e4 (src[si++]);

          if (e4 == C ('='))
          {
            if (e3 == C ('='))
            {
              // Two pads. Last 4 bits in b2 should be zero.
              //
              if ((b2 & 0x0F) != 0)
                throw invalid_value<C> (bits::base64_binary<C> (), str_);

              dst[di++] = (b1 << 2) | (b2 >> 4);
            }
            else
            {
              // One pad. Last 2 bits in b3 should be zero.
              //
              b3 = bits::base64_decode (e3);

              if (b3 == 0xFF || (b3 & 0x03) != 0)
                throw invalid_value<C> (bits::base64_binary<C> (), str_);

              dst[di++] = (b1 << 2) | (b2 >> 4);
              dst[di++] = (b2 << 4) | (b3 >> 2);
            }
          }
          else
          {
            // No pads.
            //
            b3 = bits::base64_decode (e3);
            b4 = bits::base64_decode (e4);

            if (b3 == 0xFF || b4 == 0xFF)
              throw invalid_value<C> (bits::base64_binary<C> (), str_);

            dst[di++] = (b1 << 2) | (b2 >> 4);
            dst[di++] = (b2 << 4) | (b3 >> 2);
            dst[di++] = (b3 << 6) | b4;
          }

          // Set the real size.
          //
          buf_->size (di);
        }

        template <typename C>
        std::auto_ptr<buffer> base64_binary_pimpl<C>::
        post_base64_binary ()
        {
          return buf_;
        }

        // hex_binary
        //
        template <typename C>
        void hex_binary_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void hex_binary_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        namespace bits
        {
          template <typename C>
          inline unsigned char
          hex_decode (C c)
          {
            unsigned char r (0xFF);

            if (c >= C('0') && c <= C ('9'))
              r = static_cast<unsigned char> (c - C ('0'));
            else if (c >= C ('A') && c <= C ('F'))
              r = static_cast<unsigned char> (10 + (c - C ('A')));
            else if (c >= C ('a') && c <= C ('f'))
              r = static_cast<unsigned char> (10 + (c - C ('a')));

            return r;
          }
        }

        template <typename C>
        void hex_binary_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          if (size % 2 != 0)
            throw invalid_value<C> (bits::hex_binary<C> (), tmp);

          buffer::size_t n (size / 2);
          buf_.reset (new buffer (n));

          if (n != 0)
          {
            const C* src (tmp.data ());
            char* dst (buf_->data ());
            buffer::size_t i (0);

            for (; i < n; ++i)
            {
              unsigned char h (bits::hex_decode (src[2 * i]));
              unsigned char l (bits::hex_decode (src[2 * i + 1]));

              if (h == 0xFF || l == 0xFF)
                break;

              dst[i] = (h << 4) | l;
            }

            if (i != n)
              throw invalid_value<C> (bits::hex_binary<C> (), tmp);
          }
        }

        template <typename C>
        std::auto_ptr<buffer> hex_binary_pimpl<C>::
        post_hex_binary ()
        {
          return buf_;
        }

        // gday
        //
        template <typename C>
        void gday_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void gday_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        namespace bits
        {
          // Datatypes 3.2.7.3
          //
          template <typename C>
          bool
          parse_tz (const C* s,
                    typename std::basic_string<C>::size_type n,
                    std::basic_string<C>& r)
          {
            // time_zone := Z|(+|-)HH:MM
            //
            if (s[0] == C ('Z'))
            {
              if (n != 1)
                return false;
            }
            else
            {
              if (n != 6 ||
                  (s[0] != C ('-') && s[0] != C ('+')) ||
                  s[3] != C (':'))
                return false;

              unsigned short hh;
              ro_string<C> hhs (s + 1, 2);
              zc_istream<C> hhi (hhs);

              if (!(hhi >> hh && hhi.exhausted () && hh <= 14))
                return false;

              unsigned short mm;
              ro_string<C> mms (s + 4, 2);
              zc_istream<C> mmi (mms);

              if (!(mmi >> mm && mmi.exhausted () && mm <= 59))
                return false;

              if (hh == 14 && mm != 0)
                return false;
            }

            r.assign (s, n);
            return true;
          }
        }

        template <typename C>
        void gday_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // gday := ---DD[Z|(+|-)HH::MM]
          //
          if (size < 5 ||
              tmp[0] != C ('-') || tmp[1] != C ('-') || tmp[2] != C ('-'))
            throw invalid_value<C> (bits::gday<C> (), tmp);

          ro_string<C> day_fragment (tmp.data () + 3, 2);
          zc_istream<C> is (day_fragment);

          if (!(is >> day_ && is.exhausted () && day_ > 0 && day_ <= 31))
            throw invalid_value<C> (bits::gday<C> (), tmp);

          if (size > 5)
          {
            if (!bits::parse_tz (tmp.data () + 5, size - 5, zone_))
              throw invalid_value<C> (bits::gday<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        gday<C> gday_pimpl<C>::
        post_gday ()
        {
          return zone_.empty () ? gday<C> (day_) : gday<C> (day_, zone_);
        }

        // gmonth
        //
        template <typename C>
        void gmonth_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void gmonth_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void gmonth_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // gmonth := --MM[Z|(+|-)HH::MM]
          //
          if (size < 4 || tmp[0] != C ('-') || tmp[1] != C ('-'))
            throw invalid_value<C> (bits::gmonth<C> (), tmp);

          ro_string<C> month_fragment (tmp.data () + 2, 2);
          zc_istream<C> is (month_fragment);

          if (!(is >> month_ && is.exhausted () && month_ > 0 && month_ <= 12))
            throw invalid_value<C> (bits::gmonth<C> (), tmp);

          if (size > 4)
          {
            if (!bits::parse_tz (tmp.data () + 4, size - 4, zone_))
              throw invalid_value<C> (bits::gmonth<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        gmonth<C> gmonth_pimpl<C>::
        post_gmonth ()
        {
          return zone_.empty ()
            ? gmonth<C> (month_)
            : gmonth<C> (month_, zone_);
        }

        // gyear
        //
        template <typename C>
        void gyear_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void gyear_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void gyear_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // gyear := [-]CCYY[N]*[Z|(+|-)HH::MM]
          //

          if (size < 4 || (tmp[0] == C ('-') && size < 5))
            throw invalid_value<C> (bits::gyear<C> (), tmp);

          // Find the end of the year token.
          //
          size_type pos (tmp[0] == C ('-') ? 5 : 4);
          for (; pos < size; ++pos)
          {
            C c (tmp[pos]);

            if (c == C ('Z') || c == C ('+') || c == C ('-'))
              break;
          }

          ro_string<C> year_fragment (tmp.data (), pos);
          zc_istream<C> is (year_fragment);

          if (!(is >> year_ && is.exhausted () && year_ != 0))
            throw invalid_value<C> (bits::gyear<C> (), tmp);

          if (pos < size)
          {
            if (!bits::parse_tz (tmp.data () + pos, size - pos, zone_))
              throw invalid_value<C> (bits::gyear<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        gyear<C> gyear_pimpl<C>::
        post_gyear ()
        {
          return zone_.empty () ? gyear<C> (year_) : gyear<C> (year_, zone_);
        }

        // gmonth_day
        //
        template <typename C>
        void gmonth_day_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void gmonth_day_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void gmonth_day_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // gmonth_day := --MM-DD[Z|(+|-)HH::MM]
          //
          if (size < 7 ||
              tmp[0] != C ('-') || tmp[1] != C ('-') || tmp[4] != C ('-'))
            throw invalid_value<C> (bits::gmonth_day<C> (), tmp);

          ro_string<C> month_fragment (tmp.data () + 2, 2);
          zc_istream<C> mis (month_fragment);

          if (!(mis >> month_ && mis.exhausted () &&
                month_ > 0 && month_ <= 12))
            throw invalid_value<C> (bits::gmonth_day<C> (), tmp);

          ro_string<C> day_fragment (tmp.data () + 5, 2);
          zc_istream<C> dis (day_fragment);

          if (!(dis >> day_ && dis.exhausted () && day_ > 0 && day_ <= 31))
            throw invalid_value<C> (bits::gmonth_day<C> (), tmp);

          if (size > 7)
          {
            if (!bits::parse_tz (tmp.data () + 7, size - 7, zone_))
              throw invalid_value<C> (bits::gmonth_day<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        gmonth_day<C> gmonth_day_pimpl<C>::
        post_gmonth_day ()
        {
          return zone_.empty ()
            ? gmonth_day<C> (month_, day_)
            : gmonth_day<C> (month_, day_, zone_);
        }

        // gyear_month
        //
        template <typename C>
        void gyear_month_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void gyear_month_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void gyear_month_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // gyear_month := [-]CCYY[N]*-MM[Z|(+|-)HH::MM]
          //

          if (size < 7 || (tmp[0] == C ('-') && size < 8))
            throw invalid_value<C> (bits::gyear_month<C> (), tmp);

          // Find the end of the year token.
          //
          size_type pos (tmp.find (C ('-'), tmp[0] == C ('-') ? 5 : 4));

          if (pos == ro_string<C>::npos || (size - pos - 1) < 2)
            throw invalid_value<C> (bits::gyear_month<C> (), tmp);

          ro_string<C> year_fragment (tmp.data (), pos);
          zc_istream<C> yis (year_fragment);

          if (!(yis >> year_ && yis.exhausted () && year_ != 0))
            throw invalid_value<C> (bits::gyear_month<C> (), tmp);

          ro_string<C> month_fragment (tmp.data () + pos + 1, 2);
          zc_istream<C> mis (month_fragment);

          if (!(mis >> month_ && mis.exhausted () &&
                month_ > 0 && month_ <= 12))
            throw invalid_value<C> (bits::gyear_month<C> (), tmp);

          pos += 3;

          if (pos < size)
          {
            if (!bits::parse_tz (tmp.data () + pos, size - pos, zone_))
              throw invalid_value<C> (bits::gyear_month<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        gyear_month<C> gyear_month_pimpl<C>::
        post_gyear_month ()
        {
          return zone_.empty ()
            ? gyear_month<C> (year_, month_)
            : gyear_month<C> (year_, month_, zone_);
        }

        // date
        //
        template <typename C>
        void date_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void date_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void date_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // date := [-]CCYY[N]*-MM-DD[Z|(+|-)HH::MM]
          //

          if (size < 10 || (tmp[0] == C ('-') && size < 11))
            throw invalid_value<C> (bits::date<C> (), tmp);

          // Find the end of the year token.
          //
          size_type pos (tmp.find (C ('-'), tmp[0] == C ('-') ? 5 : 4));

          if (pos == ro_string<C>::npos
              || (size - pos - 1) < 5
              || tmp[pos + 3] != C ('-'))
            throw invalid_value<C> (bits::date<C> (), tmp);

          ro_string<C> year_fragment (tmp.data (), pos);
          zc_istream<C> yis (year_fragment);

          if (!(yis >> year_ && yis.exhausted () && year_ != 0))
            throw invalid_value<C> (bits::date<C> (), tmp);

          ro_string<C> month_fragment (tmp.data () + pos + 1, 2);
          zc_istream<C> mis (month_fragment);

          if (!(mis >> month_ && mis.exhausted () &&
                month_ > 0 && month_ <= 12))
            throw invalid_value<C> (bits::date<C> (), tmp);

          ro_string<C> day_fragment (tmp.data () + pos + 4, 2);
          zc_istream<C> dis (day_fragment);

          if (!(dis >> day_ && dis.exhausted () && day_ > 0 && day_ <= 31))
            throw invalid_value<C> (bits::date<C> (), tmp);

          pos += 6;

          if (pos < size)
          {
            if (!bits::parse_tz (tmp.data () + pos, size - pos, zone_))
              throw invalid_value<C> (bits::date<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        date<C> date_pimpl<C>::
        post_date ()
        {
          return zone_.empty ()
            ? date<C> (year_, month_, day_)
            : date<C> (year_, month_, day_, zone_);
        }

        // time
        //
        template <typename C>
        void time_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void time_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void time_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // time := HH:MM:SS[.S+][Z|(+|-)HH::MM]
          //

          if (size < 8 || tmp[2] != C (':') || tmp[5] != C (':'))
            throw invalid_value<C> (bits::time<C> (), tmp);

          ro_string<C> hour_fragment (tmp.data (), 2);
          zc_istream<C> his (hour_fragment);

          if (!(his >> hours_ && his.exhausted () && hours_ <= 24))
            throw invalid_value<C> (bits::time<C> (), tmp);

          ro_string<C> minutes_fragment (tmp.data () + 3, 2);
          zc_istream<C> mis (minutes_fragment);

          if (!(mis >> minutes_ && mis.exhausted () && minutes_ <= 59))
            throw invalid_value<C> (bits::time<C> (), tmp);

          // Find the end of the seconds fragment.
          //
          size_type pos (8);
          for (; pos < size; ++pos)
          {
            C c (tmp[pos]);

            if (c == C ('Z') || c == C ('+') || c == C ('-'))
              break;
          }

          // At least one digit should should follow the fraction point.
          //
          if ((pos - 6) == 3)
            throw invalid_value<C> (bits::time<C> (), tmp);

          ro_string<C> seconds_fragment (tmp.data () + 6, pos - 6);
          zc_istream<C> sis (seconds_fragment);

          if (!(sis >> seconds_ && sis.exhausted () && seconds_ < 60.0))
            throw invalid_value<C> (bits::time<C> (), tmp);

          if (hours_ == 24 && (minutes_ != 0 || seconds_ != 0.0))
            throw invalid_value<C> (bits::time<C> (), tmp);

          if (pos < size)
          {
            if (!bits::parse_tz (tmp.data () + pos, size - pos, zone_))
              throw invalid_value<C> (bits::gyear<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        time<C> time_pimpl<C>::
        post_time ()
        {
          return zone_.empty ()
            ? time<C> (hours_, minutes_, seconds_)
            : time<C> (hours_, minutes_, seconds_, zone_);
        }


        // date_time
        //
        template <typename C>
        void date_time_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void date_time_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        template <typename C>
        void date_time_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          // date_time := [-]CCYY[N]*-MM-DDTHH:MM:SS[.S+][Z|(+|-)HH::MM]
          //

          if (size < 19 || (tmp[0] == C ('-') && size < 20))
            throw invalid_value<C> (bits::date_time<C> (), tmp);

          // Find the end of the year token.
          //
          size_type pos (tmp.find (C ('-'), tmp[0] == C ('-') ? 5 : 4));

          if (pos == ro_string<C>::npos || (size - pos - 1) < 14
              || tmp[pos + 3] != C ('-') || tmp[pos + 6] != C ('T')
              || tmp[pos + 9] != C (':') || tmp[pos + 12] != C (':'))
            throw invalid_value<C> (bits::date_time<C> (), tmp);

          ro_string<C> year_fragment (tmp.data (), pos);
          zc_istream<C> yis (year_fragment);

          if (!(yis >> year_ && yis.exhausted () && year_ != 0))
            throw invalid_value<C> (bits::date_time<C> (), tmp);

          ro_string<C> month_fragment (tmp.data () + pos + 1, 2);
          zc_istream<C> mis (month_fragment);

          if (!(mis >> month_ && mis.exhausted () &&
                month_ > 0 && month_ <= 12))
            throw invalid_value<C> (bits::date_time<C> (), tmp);

          ro_string<C> day_fragment (tmp.data () + pos + 4, 2);
          zc_istream<C> dis (day_fragment);

          if (!(dis >> day_ && dis.exhausted () && day_ > 0 && day_ <= 31))
            throw invalid_value<C> (bits::date_time<C> (), tmp);

          pos += 7; // Point to the first H.

          ro_string<C> hour_fragment (tmp.data () + pos, 2);
          zc_istream<C> his (hour_fragment);

          if (!(his >> hours_ && his.exhausted () && hours_ <= 24))
            throw invalid_value<C> (bits::time<C> (), tmp);

          ro_string<C> minutes_fragment (tmp.data () + pos + 3, 2);
          zc_istream<C> min_is (minutes_fragment);

          if (!(min_is >> minutes_ && min_is.exhausted () && minutes_ <= 59))
            throw invalid_value<C> (bits::time<C> (), tmp);

          // Find the end of the seconds fragment.
          //
          pos += 6; // Point to the first S.

          size_type sec_end (pos + 2);
          for (; sec_end < size; ++sec_end)
          {
            C c (tmp[sec_end]);

            if (c == C ('Z') || c == C ('+') || c == C ('-'))
              break;
          }

          // At least one digit should should follow the fraction point.
          //
          if ((sec_end - pos) == 3)
            throw invalid_value<C> (bits::time<C> (), tmp);

          ro_string<C> seconds_fragment (tmp.data () + pos, sec_end - pos);
          zc_istream<C> sis (seconds_fragment);

          if (!(sis >> seconds_ && sis.exhausted () && seconds_ < 60.0))
            throw invalid_value<C> (bits::time<C> (), tmp);

          if (hours_ == 24 && (minutes_ != 0 || seconds_ != 0.0))
            throw invalid_value<C> (bits::time<C> (), tmp);

          if (sec_end < size)
          {
            if (!bits::parse_tz (tmp.data () + sec_end, size - sec_end, zone_))
              throw invalid_value<C> (bits::gyear<C> (), tmp);
          }
          else
            zone_.clear ();
        }

        template <typename C>
        date_time<C> date_time_pimpl<C>::
        post_date_time ()
        {
          return zone_.empty ()
            ? date_time<C> (year_, month_, day_, hours_, minutes_, seconds_)
            : date_time<C> (year_, month_, day_, hours_, minutes_, seconds_,
                            zone_);
        }

        // duration
        //
        template <typename C>
        void duration_pimpl<C>::
        _pre ()
        {
          str_.clear ();
        }

        template <typename C>
        void duration_pimpl<C>::
        _characters (const ro_string<C>& s)
        {
          if (str_.size () == 0)
          {
            ro_string<C> tmp (s.data (), s.size ());

            if (trim_left (tmp) != 0)
              str_ += tmp;
          }
          else
            str_ += s;
        }

        namespace bits
        {
          template <typename C>
          inline typename ro_string<C>::size_type
          find_delim (const C* s,
                      typename ro_string<C>::size_type pos,
                      typename ro_string<C>::size_type size)
          {
            const C* p (s + pos);
            for (; p < (s + size); ++p)
            {
              if (*p == C ('Y') || *p == C ('D') || *p == C ('M') ||
                  *p == C ('H') || *p == C ('M') || *p == C ('S') ||
                  *p == C ('T'))
                break;
            }

            return p - s;
          }
        }

        template <typename C>
        void duration_pimpl<C>::
        _post ()
        {
          typedef typename ro_string<C>::size_type size_type;

          ro_string<C> tmp (str_);
          size_type size (trim_right (tmp));

          negative_ = false;
          years_ = 0;
          months_ = 0;
          days_ = 0;
          hours_ = 0;
          minutes_ = 0;
          seconds_ = 0.0;

          // duration := [-]P[nY][nM][nD][TnHnMnS[.S+]]
          //
          const C* s (tmp.data ());

          if (size < 3 || (s[0] == C ('-') && size < 4))
            throw invalid_value<C> (bits::duration<C> (), tmp);

          size_type pos (0);

          if (s[0] == C ('-'))
          {
            negative_ = true;
            pos++;
          }

          if (s[pos++] != C ('P'))
            throw invalid_value<C> (bits::duration<C> (), tmp);

          size_type del (bits::find_delim (s, pos, size));

          // Duration should contain at least one component.
          //
          if (del == size)
            throw invalid_value<C> (bits::duration<C> (), tmp);

          if (s[del] == C ('Y'))
          {
            ro_string<C> fragment (s + pos, del - pos);
            zc_istream<C> is (fragment);

            if (!(is >> years_ && is.exhausted ()))
              throw invalid_value<C> (bits::duration<C> (), tmp);

            pos = del + 1;
            del = bits::find_delim (s, pos, size);
          }

          if (del != size && s[del] == C ('M'))
          {
            ro_string<C> fragment (s + pos, del - pos);
            zc_istream<C> is (fragment);

            if (!(is >> months_ && is.exhausted ()))
              throw invalid_value<C> (bits::duration<C> (), tmp);

            pos = del + 1;
            del = bits::find_delim (s, pos, size);
          }

          if (del != size && s[del] == C ('D'))
          {
            ro_string<C> fragment (s + pos, del - pos);
            zc_istream<C> is (fragment);

            if (!(is >> days_ && is.exhausted ()))
              throw invalid_value<C> (bits::duration<C> (), tmp);

            pos = del + 1;
            del = bits::find_delim (s, pos, size);
          }

          if (del != size && s[del] == C ('T'))
          {
            pos = del + 1;
            del = bits::find_delim (s, pos, size);

            // At least one time component should be present.
            //
            if (del == size)
              throw invalid_value<C> (bits::duration<C> (), tmp);

            if (s[del] == C ('H'))
            {
              ro_string<C> fragment (s + pos, del - pos);
              zc_istream<C> is (fragment);

              if (!(is >> hours_ && is.exhausted ()))
                throw invalid_value<C> (bits::duration<C> (), tmp);

              pos = del + 1;
              del = bits::find_delim (s, pos, size);
            }

            if (del != size && s[del] == C ('M'))
            {
              ro_string<C> fragment (s + pos, del - pos);
              zc_istream<C> is (fragment);

              if (!(is >> minutes_ && is.exhausted ()))
                throw invalid_value<C> (bits::duration<C> (), tmp);

              pos = del + 1;
              del = bits::find_delim (s, pos, size);
            }

            if (del != size && s[del] == C ('S'))
            {
              ro_string<C> fragment (s + pos, del - pos);
              zc_istream<C> is (fragment);

              if (!(is >> seconds_ && is.exhausted () && seconds_ >= 0.0))
                throw invalid_value<C> (bits::duration<C> (), tmp);

              pos = del + 1;
            }
          }

          // Something did not patch or appeared in the wrong order.
          //
          if (pos != size)
            throw invalid_value<C> (bits::duration<C> (), tmp);
        }

        template <typename C>
        duration duration_pimpl<C>::
        post_duration ()
        {
          return duration (
            negative_, years_, months_, days_, hours_, minutes_, seconds_);
        }
      }
    }
  }
}

