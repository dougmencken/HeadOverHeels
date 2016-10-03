// file      : xsd/cxx/parser/xml-schema.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2007 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <new>     // operator new/delete
#include <cstring> // std::memcpy, std::memcmp

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      // string_sequence
      //
      template <typename C>
      string_sequence<C>::
      string_sequence ()
      {
      }

      template <typename C>
      string_sequence<C>::
      string_sequence (size_type n, const value_type& x)
          : base (n, x)
      {
      }

      template <typename C>
      template <typename I>
      string_sequence<C>::
      string_sequence (const I& begin, const I& end)
          : base (begin, end)
      {
      }

      template <typename C>
      inline bool
      operator!= (const string_sequence<C>& a, const string_sequence<C>& b)
      {
        return !(a == b);
      }

      // qname
      //
      template <typename C>
      inline qname<C>::
      qname (const std::basic_string<C>& name)
          : name_ (name)
      {
      }

      template <typename C>
      inline qname<C>::
      qname (const std::basic_string<C>& prefix,
             const std::basic_string<C>& name)
          : prefix_ (prefix), name_ (name)
      {
      }

      template <typename C>
      inline const std::basic_string<C>& qname<C>::
      prefix () const
      {
        return prefix_;
      }

      template <typename C>
      inline void qname<C>::
      prefix (const std::basic_string<C>& prefix)
      {
        prefix_ = prefix;
      }

      template <typename C>
      inline const std::basic_string<C>& qname<C>::
      name () const
      {
        return name_;
      }

      template <typename C>
      inline void qname<C>::
      name (const std::basic_string<C>& name)
      {
        name_ = name;
      }

      template <typename C>
      inline bool
      operator== (const qname<C>& a, const qname<C>& b)
      {
        return a.prefix () == b.prefix () && a.name () == b.name ();
      }

      template <typename C>
      inline bool
      operator!= (const qname<C>& a, const qname<C>& b)
      {
        return !(a == b);
      }

      // buffer
      //
      inline buffer::
      ~buffer ()
      {
        operator delete (data_);
      }

      inline buffer::
      buffer (size_t size)
          : data_ (0), size_ (0), capacity_ (0)
      {
        capacity (size);
        size_ = size;
      }

      inline buffer::
      buffer (size_t size, size_t cap)
          : data_ (0), size_ (0), capacity_ (0)
      {
        if (size > cap)
          throw bounds ();

        capacity (cap);
        size_ = size;
      }

      inline buffer::
      buffer (const void* data, size_t size)
          : data_ (0), size_ (0), capacity_ (0)
      {
        capacity (size);
        size_ = size;

        if (size_)
          std::memcpy (data_, data, size_);
      }

      inline buffer::
      buffer (const void* data, size_t size, size_t cap)
          : data_ (0), size_ (0), capacity_ (0)
      {
        if (size > cap)
          throw bounds ();

        capacity (cap);
        size_ = size;

        if (size_)
          std::memcpy (data_, data, size_);
      }

      inline buffer::
      buffer (void* data, size_t size, size_t cap, bool own)
          : data_ (0), size_ (0), capacity_ (0)
      {
        if (size > cap)
          throw bounds ();

        if (own)
        {
          data_ = reinterpret_cast<char*> (data);
          size_ = size;
          capacity_ = cap;
        }
        else
        {
          capacity (cap);
          size_ = size;

          if (size_)
            std::memcpy (data_, data, size_);
        }
      }

      inline buffer::
      buffer (const buffer& other)
          : data_ (0), size_ (0), capacity_ (0)
      {
        capacity (other.capacity_);
        size_ = other.size_;

        if (size_)
          std::memcpy (data_, other.data_, size_);
      }

      inline buffer& buffer::
      operator= (const buffer& other)
      {
        if (this != &other)
        {
          capacity (other.capacity_, false);
          size_ = other.size_;

          if (size_)
            std::memcpy (data_, other.data_, size_);
        }

        return *this;
      }

      inline size_t buffer::
      capacity () const
      {
        return capacity_;
      }

      inline bool buffer::
      capacity (size_t cap)
      {
        return capacity (cap, true);
      }

      inline size_t buffer::
      size () const
      {
        return size_;
      }

      inline bool buffer::
      size (size_t size)
      {
        bool r (false);

        if (size > capacity_)
          r = capacity (size);

        size_ = size;

        return r;
      }

      inline const char* buffer::
      data () const
      {
        return data_;
      }

      inline char* buffer::
      data ()
      {
        return data_;
      }

      inline const char* buffer::
      begin () const
      {
        return data_;
      }

      inline char* buffer::
      begin ()
      {
        return data_;
      }

      inline const char* buffer::
      end () const
      {
        return data_ + size_;
      }

      inline char* buffer::
      end ()
      {
        return data_ + size_;
      }

      inline void buffer::
      swap (buffer& other)
      {
        char* tmp_data (data_);
        size_t tmp_size (size_);
        size_t tmp_capacity (capacity_);

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = tmp_data;
        other.size_ = tmp_size;
        other.capacity_ = tmp_capacity;
      }

      inline bool buffer::
      capacity (size_t capacity, bool copy)
      {
        if (size_ > capacity)
          throw bounds ();

        if (capacity <= capacity_)
        {
          return false; // Do nothing if shrinking is requested.
        }
        else
        {
          char* data (reinterpret_cast<char*> (operator new (capacity)));

          if (copy && size_ > 0)
            std::memcpy (data, data_, size_);

          char* tmp (data_);
          data_ = data;
          capacity_ = capacity;

          operator delete (tmp);

          return true;
        }
      }

      inline bool
      operator== (const buffer& a, const buffer& b)
      {
        return a.size () == b.size () &&
          std::memcmp (a.data (), b.data (), a.size ()) == 0;
      }

      inline bool
      operator!= (const buffer& a, const buffer& b)
      {
        return !(a == b);
      }

      // gday
      //
      template <typename C>
      inline gday<C>::
      gday (unsigned short day)
          : day_ (day)
      {
      }

      template <typename C>
      inline gday<C>::
      gday (unsigned short day, const std::basic_string<C>& zone)
          : day_ (day), zone_ (zone)
      {
      }

      template <typename C>
      inline unsigned short gday<C>::
      day () const
      {
        return day_;
      }

      template <typename C>
      inline void gday<C>::
      day (unsigned short day)
      {
        day_ = day;
      }

      template <typename C>
      inline const std::basic_string<C>& gday<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void gday<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const gday<C>& a, const gday<C>& b)
      {
        return a.day () == b.day () && a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const gday<C>& a, const gday<C>& b)
      {
        return !(a == b);
      }

      // gmonth
      //
      template <typename C>
      inline gmonth<C>::
      gmonth (unsigned short month)
          : month_ (month)
      {
      }

      template <typename C>
      inline gmonth<C>::
      gmonth (unsigned short month, const std::basic_string<C>& zone)
          : month_ (month), zone_ (zone)
      {
      }

      template <typename C>
      inline unsigned short gmonth<C>::
      month () const
      {
        return month_;
      }

      template <typename C>
      inline void gmonth<C>::
      month (unsigned short month)
      {
        month_ = month;
      }

      template <typename C>
      inline const std::basic_string<C>& gmonth<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void gmonth<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const gmonth<C>& a, const gmonth<C>& b)
      {
        return a.month () == b.month () && a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const gmonth<C>& a, const gmonth<C>& b)
      {
        return !(a == b);
      }

      // gyear
      //
      template <typename C>
      inline gyear<C>::
      gyear (int year)
          : year_ (year)
      {
      }

      template <typename C>
      inline gyear<C>::
      gyear (int year, const std::basic_string<C>& zone)
          : year_ (year), zone_ (zone)
      {
      }

      template <typename C>
      inline int gyear<C>::
      year () const
      {
        return year_;
      }

      template <typename C>
      inline void gyear<C>::
      year (int year)
      {
        year_ = year;
      }

      template <typename C>
      inline const std::basic_string<C>& gyear<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void gyear<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const gyear<C>& a, const gyear<C>& b)
      {
        return a.year () == b.year () && a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const gyear<C>& a, const gyear<C>& b)
      {
        return !(a == b);
      }

      // gmonth_day
      //
      template <typename C>
      inline gmonth_day<C>::
      gmonth_day (unsigned short month, unsigned short day)
          : month_ (month), day_ (day)
      {
      }

      template <typename C>
      inline gmonth_day<C>::
      gmonth_day (unsigned short month,
                  unsigned short day,
                  const std::basic_string<C>& zone)
          : month_ (month), day_ (day), zone_ (zone)
      {
      }

      template <typename C>
      inline unsigned short gmonth_day<C>::
      month () const
      {
        return month_;
      }

      template <typename C>
      inline void gmonth_day<C>::
      month (unsigned short month)
      {
        month_ = month;
      }

      template <typename C>
      inline unsigned short gmonth_day<C>::
      day () const
      {
        return day_;
      }

      template <typename C>
      inline void gmonth_day<C>::
      day (unsigned short day)
      {
        day_ = day;
      }

      template <typename C>
      inline const std::basic_string<C>& gmonth_day<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void gmonth_day<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const gmonth_day<C>& a, const gmonth_day<C>& b)
      {
        return a.month () == b.month () &&
          a.day () == b.day () &&
          a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const gmonth_day<C>& a, const gmonth_day<C>& b)
      {
        return !(a == b);
      }

      // gyear_month
      //
      template <typename C>
      inline gyear_month<C>::
      gyear_month (int year, unsigned short month)
          : year_ (year), month_ (month)
      {
      }

      template <typename C>
      inline gyear_month<C>::
      gyear_month (int year, unsigned short month,
                   const std::basic_string<C>& zone)
          : year_ (year), month_ (month), zone_ (zone)
      {
      }

      template <typename C>
      inline int gyear_month<C>::
      year () const
      {
        return year_;
      }

      template <typename C>
      inline void gyear_month<C>::
      year (int year)
      {
        year_ = year;
      }

      template <typename C>
      inline unsigned short gyear_month<C>::
      month () const
      {
        return month_;
      }

      template <typename C>
      inline void gyear_month<C>::
      month (unsigned short month)
      {
        month_ = month;
      }

      template <typename C>
      inline const std::basic_string<C>& gyear_month<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void gyear_month<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const gyear_month<C>& a, const gyear_month<C>& b)
      {
        return a.year () == b.year () &&
          a.month () == b.month () &&
          a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const gyear_month<C>& a, const gyear_month<C>& b)
      {
        return !(a == b);
      }

      // date
      //
      template <typename C>
      inline date<C>::
      date (int year, unsigned short month, unsigned short day)
          : year_ (year), month_ (month), day_ (day)
      {
      }

      template <typename C>
      inline date<C>::
      date (int year, unsigned short month, unsigned short day,
            const std::basic_string<C>& zone)
          : year_ (year), month_ (month), day_ (day), zone_ (zone)
      {
      }

      template <typename C>
      inline int date<C>::
      year () const
      {
        return year_;
      }

      template <typename C>
      inline void date<C>::
      year (int year)
      {
        year_ = year;
      }

      template <typename C>
      inline unsigned short date<C>::
      month () const
      {
        return month_;
      }

      template <typename C>
      inline void date<C>::
      month (unsigned short month)
      {
        month_ = month;
      }

      template <typename C>
      inline unsigned short date<C>::
      day () const
      {
        return day_;
      }

      template <typename C>
      inline void date<C>::
      day (unsigned short day)
      {
        day_ = day;
      }

      template <typename C>
      inline const std::basic_string<C>& date<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void date<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const date<C>& a, const date<C>& b)
      {
        return a.year () == b.year () &&
          a.month () == b.month () &&
          a.day () == b.day () &&
          a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const date<C>& a, const date<C>& b)
      {
        return !(a == b);
      }

      // time
      //
      template <typename C>
      inline time<C>::
      time (unsigned short hours, unsigned short minutes, double seconds)
          : hours_ (hours), minutes_ (minutes), seconds_ (seconds)
      {
      }

      template <typename C>
      inline time<C>::
      time (unsigned short hours, unsigned short minutes, double seconds,
            const std::basic_string<C>& zone)
          : hours_ (hours), minutes_ (minutes), seconds_ (seconds),
            zone_ (zone)
      {
      }

      template <typename C>
      inline unsigned short time<C>::
      hours () const
      {
        return hours_;
      }

      template <typename C>
      inline void time<C>::
      hours (unsigned short hours)
      {
        hours_ = hours;
      }

      template <typename C>
      inline unsigned short time<C>::
      minutes () const
      {
        return minutes_;
      }

      template <typename C>
      inline void time<C>::
      minutes (unsigned short minutes)
      {
        minutes_ = minutes;
      }

      template <typename C>
      inline double time<C>::
      seconds () const
      {
        return seconds_;
      }

      template <typename C>
      inline void time<C>::
      seconds (double seconds)
      {
        seconds_ = seconds;
      }

      template <typename C>
      inline const std::basic_string<C>& time<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void time<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const time<C>& a, const time<C>& b)
      {
        return a.hours () == b.hours () &&
          a.minutes () == b.minutes () &&
          a.seconds () == b.seconds () &&
          a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const time<C>& a, const time<C>& b)
      {
        return !(a == b);
      }

      // date_time
      //
      template <typename C>
      inline date_time<C>::
      date_time (int year, unsigned short month, unsigned short day,
                 unsigned short hours, unsigned short minutes, double seconds)
          : year_ (year), month_ (month), day_ (day),
            hours_ (hours), minutes_ (minutes), seconds_ (seconds)
      {
      }

      template <typename C>
      inline date_time<C>::
      date_time (int year, unsigned short month, unsigned short day,
                 unsigned short hours, unsigned short minutes, double seconds,
                 const std::basic_string<C>& zone)
          : year_ (year), month_ (month), day_ (day),
            hours_ (hours), minutes_ (minutes), seconds_ (seconds),
            zone_ (zone)
      {
      }

      template <typename C>
      inline int date_time<C>::
      year () const
      {
        return year_;
      }

      template <typename C>
      inline void date_time<C>::
      year (int year)
      {
        year_ = year;
      }

      template <typename C>
      inline unsigned short date_time<C>::
      month () const
      {
        return month_;
      }

      template <typename C>
      inline void date_time<C>::
      month (unsigned short month)
      {
        month_ = month;
      }

      template <typename C>
      inline unsigned short date_time<C>::
      day () const
      {
        return day_;
      }

      template <typename C>
      inline void date_time<C>::
      day (unsigned short day)
      {
        day_ = day;
      }

      template <typename C>
      inline unsigned short date_time<C>::
      hours () const
      {
        return hours_;
      }

      template <typename C>
      inline void date_time<C>::
      hours (unsigned short hours)
      {
        hours_ = hours;
      }

      template <typename C>
      inline unsigned short date_time<C>::
      minutes () const
      {
        return minutes_;
      }

      template <typename C>
      inline void date_time<C>::
      minutes (unsigned short minutes)
      {
        minutes_ = minutes;
      }

      template <typename C>
      inline double date_time<C>::
      seconds () const
      {
        return seconds_;
      }

      template <typename C>
      inline void date_time<C>::
      seconds (double seconds)
      {
        seconds_ = seconds;
      }

      template <typename C>
      inline const std::basic_string<C>& date_time<C>::
      zone () const
      {
        return zone_;
      }

      template <typename C>
      inline void date_time<C>::
      zone (const std::basic_string<C>& zone)
      {
        zone_ = zone;
      }

      template <typename C>
      inline bool
      operator== (const date_time<C>& a, const date_time<C>& b)
      {
        return a.year () == b.year () &&
          a.month () == b.month () &&
          a.day () == b.day () &&
          a.hours () == b.hours () &&
          a.minutes () == b.minutes () &&
          a.seconds () == b.seconds () &&
          a.zone () == b.zone ();
      }

      template <typename C>
      inline bool
      operator!= (const date_time<C>& a, const date_time<C>& b)
      {
        return !(a == b);
      }

      // duration
      //
      inline duration::
      duration (bool negative,
                unsigned int years, unsigned int months, unsigned int days,
                unsigned int hours, unsigned int minutes, double seconds)
          : negative_ (negative),
            years_ (years), months_ (months), days_ (days),
            hours_ (hours), minutes_ (minutes), seconds_ (seconds)
      {
      }

      inline bool duration::
      negative () const
      {
        return negative_;
      }

      inline void duration::
      negative (bool negative)
      {
        negative_ = negative;
      }

      inline unsigned int duration::
      years () const
      {
        return years_;
      }

      inline void duration::
      years (unsigned int years)
      {
        years_ = years;
      }

      inline unsigned int duration::
      months () const
      {
        return months_;
      }

      inline void duration::
      months (unsigned int months)
      {
        months_ = months;
      }

      inline unsigned int duration::
      days () const
      {
        return days_;
      }

      inline void duration::
      days (unsigned int days)
      {
        days_ = days;
      }

      inline unsigned int duration::
      hours () const
      {
        return hours_;
      }

      inline void duration::
      hours (unsigned int hours)
      {
        hours_ = hours;
      }

      inline unsigned int duration::
      minutes () const
      {
        return minutes_;
      }

      inline void duration::
      minutes (unsigned int minutes)
      {
        minutes_ = minutes;
      }

      inline double duration::
      seconds () const
      {
        return seconds_;
      }

      inline void duration::
      seconds (double seconds)
      {
        seconds_ = seconds;
      }

      inline bool
      operator== (const duration& a, const duration& b)
      {
        return a.negative () == b.negative () &&
          a.years () == b.years () &&
          a.months () == b.months () &&
          a.days () == b.days () &&
          a.hours () == b.hours () &&
          a.minutes () == b.minutes () &&
          a.seconds () == b.seconds ();
      }

      inline bool
      operator!= (const duration& a, const duration& b)
      {
        return !(a == b);
      }
    }
  }
}

