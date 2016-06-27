/*!
 * \file Value.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <string>
#include <vector>
#include <initializer_list>
#include <ossia_export.h>

namespace OSSIA
{

class OSSIA_EXPORT Value
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of value */
  enum class Type
  {
    IMPULSE,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    TUPLE,
    GENERIC,
    DESTINATION,  //! \note see Destination structure declaration in Node.h
    BEHAVIOR,     //! \note see Behavior structure declaration in Curve.h
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! clone */
  virtual Value * clone() const = 0;

  /*! destructor */
  virtual ~Value();

#if 0
# pragma mark -
# pragma mark Operators
#endif

  /*! equal operator */
  virtual bool operator== (const Value& v) const = 0;

  /*! different operator */
  virtual bool operator!= (const Value& v) const = 0;

  /*! greater than operator */
  virtual bool operator> (const Value& v) const = 0;

  /*! greater than and equal operator */
  virtual bool operator>= (const Value& v) const = 0;

  /*! less than operator */
  virtual bool operator< (const Value& v) const = 0;

  /*! less than and equal operator */
  virtual bool operator<= (const Value& v) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return m_type;}

protected:

  Type m_type;
};

#if 0
# pragma mark -
# pragma mark Impulse
#endif

/*! \details Impulse value */
struct OSSIA_EXPORT Impulse final : public Value
{
  /*! constructor */
  Impulse();

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Bool
#endif

/*! \details Bool value */
struct OSSIA_EXPORT Bool final : public Value
{
  bool value;

  /*! constructor */
  Bool(bool = false);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

const Bool False{false};
const Bool True{true};

#if 0
# pragma mark -
# pragma mark Int
#endif

/*! \details Int value */
struct OSSIA_EXPORT Int final : public Value
{
  int value;

  /*! constructor
   \param int value */
  Int(int = 0);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Float
#endif

/*! \details Float value */
struct OSSIA_EXPORT Float final : public Value
{
  float value;

  /*! constructor
   \param float value */
  Float(float = 0.);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Char
#endif

/*! \details Char value */
struct OSSIA_EXPORT Char final : public Value
{
  char value;

  /*! constructor
   \param char value */
  Char(char = 0x00);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark String
#endif

/*! \details String value */
struct OSSIA_EXPORT String final : public Value
{
  std::string value;

  /*! constructor
   \param std::string value */
  String(std::string = "");

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Tuple
#endif

/*! \details Tuple value */
struct OSSIA_EXPORT Tuple final : public Value
{
  std::vector<Value*> value;

  /*! constructor for an empty tuple */
  Tuple();
  ~Tuple();

  /*! Mechanism for building a Tuple with a list of
   * values, to remove the need for spurious memory allocations.
   * Usage : Tuple t{Tuple::ValueInit, Int{2}, Float{3.145}, String{"foo"}};
   */
  struct ValueInit {} ;
  template<typename... Args>
  explicit Tuple(ValueInit, Args&&... args)
  {
      value.reserve(sizeof...(args));
      append(std::forward<Args>(args)...);
  }

  void append() { }
  template<typename Arg, typename... Args>
  void append(Arg&& arg, Args&&... args)
  {
      value.push_back(arg.clone());
      append(std::forward<Args>(args)...);
  }

  /*! constructor for one value
   \param const value */
  explicit Tuple(const Value* v);

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<const Value*>);

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<const Value*>&);

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Generic
#endif

/*! \details Generic value */
template<typename T>
struct Generic final : public Value
{
    T value;

    Generic(T v) : value(v)
    {
        m_type = Type::GENERIC;
    }

    Generic* clone() const
    {
        return new Generic(value);
    }

    bool operator== (const Value& v) const
    {
        switch (v.getType())
        {
        case Value::Type::IMPULSE:
        {
            return true;
        }
        case Value::Type::BOOL:
        {
            auto b = static_cast<const Bool*>(&v);
            return value == b->value;
        }
        case Value::Type::INT:
        {
            auto i = static_cast<const Int*>(&v);
            return value == i->value;
        }
        case Value::Type::FLOAT:
        {
            auto f = static_cast<const Float*>(&v);
            return value == f->value;
        }
        case Value::Type::CHAR:
        {
            auto c = static_cast<const Char*>(&v);
            return value == c->value;
        }
        case Value::Type::STRING:
        {
            return false;
        }
        case Value::Type::TUPLE:
        {
            auto t = static_cast<const Tuple*>(&v);
            if (t->value.size() == 1)
                return *this == *t->value[0];
            else
                return false;
        }
        case Value::Type::GENERIC:
        {
            Generic<T>* g = (Generic<T>*)&v;
            return value == g->value;
        }
        case Value::Type::DESTINATION:
        {
            auto d = static_cast<const Destination*>(&v);
            if (d->value->getAddress())
            {
                const Value* c = d->value->getAddress()->cloneValue(d->index);
                bool result = *this == *c;

                delete c;
                return result;
            }
        }
        case Value::Type::BEHAVIOR:
        {
            return false;
        }
        default:
            return false;
        }
    }

    bool operator!= (const Value& v) const
    {
        return !(*this == v);
    }

    bool operator> (const Value& v) const
    {
        switch (v.getType())
        {
        case Value::Type::IMPULSE:
        {
            return true;
        }
        case Value::Type::BOOL:
        {
            auto b = static_cast<const Bool*>(&v);
            return value == b->value;
        }
        case Value::Type::INT:
        {
            auto i = static_cast<const Int*>(&v);
            return value > i->value;
        }
        case Value::Type::FLOAT:
        {
            auto f = static_cast<const Float*>(&v);
            return value > f->value;
        }
        case Value::Type::CHAR:
        {
            auto c = static_cast<const Char*>(&v);
            return value > c->value;
        }
        case Value::Type::STRING:
        {
            return false;
        }
        case Value::Type::TUPLE:
        {
            auto t = static_cast<const Tuple*>(&v);
            if (t->value.size() == 1)
                return *this > *t->value[0];
            else
                return false;
        }
        case Value::Type::GENERIC:
        {
            Generic<T>* g = (Generic<T>*)&v;
            return value > g->value;
        }
        case Value::Type::DESTINATION:
        {
            auto d = static_cast<const Destination*>(&v);
            if (d->value->getAddress())
            {
                const Value* c = d->value->getAddress()->cloneValue(d->index);
                bool result = *this > *c;

                delete c;
                return result;
            }
        }
        case Value::Type::BEHAVIOR:
        {
            return false;
        }
        default:
            return false;
        }
    }

    bool operator>= (const Value& v) const
    {
        switch (v.getType())
        {
        case Value::Type::IMPULSE:
        {
            return true;
        }
        case Value::Type::BOOL:
        {
            auto b = static_cast<const Bool*>(&v);
            return value >= b->value;
        }
        case Value::Type::INT:
        {
            auto i = static_cast<const Int*>(&v);
            return value >= i->value;
        }
        case Value::Type::FLOAT:
        {
            auto f = static_cast<const Float*>(&v);
            return value >= f->value;
        }
        case Value::Type::CHAR:
        {
            auto c = static_cast<const Char*>(&v);
            return value >= c->value;
        }
        case Value::Type::STRING:
        {
            return false;
        }
        case Value::Type::TUPLE:
        {
            auto t = static_cast<const Tuple*>(&v);
            if (t->value.size() == 1)
                return *this >= *t->value[0];
            else
                return false;
        }
        case Value::Type::GENERIC:
        {
            Generic<T>* g = (Generic<T>*)&v;
            return value >= g->value;
        }
        case Value::Type::DESTINATION:
        {
            auto d = static_cast<const Destination*>(&v);
            if (d->value->getAddress())
            {
                const Value* c = d->value->getAddress()->cloneValue(d->index);
                bool result = *this >= *c;

                delete c;
                return result;
            }
        }
        case Value::Type::BEHAVIOR:
        {
            return false;
        }
        default:
            return false;
        }
    }

    bool operator< (const Value& v) const
    {
        return !(*this >= v);
    }

    bool operator<= (const Value& v) const
    {
        return !(*this > v);
    }
};

}

