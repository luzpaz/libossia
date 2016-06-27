/*!
 * \file CurveSegmentPower.h
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

#include "Editor/CurveSegment.h"

namespace OSSIA
{

template <typename Y>
class CurveSegmentPower : public CurveSegment<Y>
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
  \param std::shared_ptr<CurveAbstract> parent
  \return std::shared_ptr<CurveSegmentPower<Y>> */
  static std::shared_ptr<CurveSegmentPower<Y>> create(std::shared_ptr<CurveAbstract> = nullptr);

  /*! clone */
  virtual std::shared_ptr<CurveSegmentPower<Y>> clone() const = 0;

  /*! destructor */
  virtual ~CurveSegmentPower();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get power value
  \return double */
  virtual double getPower() const = 0;

  /*! set power value
  \param double
  \return CurveSegmentPower */
  virtual CurveSegmentPower<Y> & setPower(double) = 0;

  typename CurveSegment<Y>::Type getType() const override final
  {return CurveSegment<Y>::Type::POWER;}

};
}

#if !defined(APIJamoma_EXPORTS)
extern template class OSSIA_EXPORT OSSIA::CurveSegmentPower<bool>;
extern template class OSSIA_EXPORT OSSIA::CurveSegmentPower<int>;
extern template class OSSIA_EXPORT OSSIA::CurveSegmentPower<float>;
#endif
