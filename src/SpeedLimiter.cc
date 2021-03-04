/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, PAL Robotics, S.L.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the PAL Robotics nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/*
 * Author: Enrique Fernández
 * Modified: Carlos Agüero
 */

#include "ignition/math/Helpers.hh"
#include "ignition/math/SpeedLimiter.hh"

using namespace ignition;
using namespace math;

/// \brief Private SpeedLimiter data class.
class ignition::math::SpeedLimiterPrivate
{
  /// \brief Class constructor.
  public: SpeedLimiterPrivate(bool _hasVelocityLimits,
                              bool _hasAccelerationLimits,
                              bool _hasJerkLimits,
                              double _minVelocity,
                              double _maxVelocity,
                              double _minAcceleration,
                              double _maxAcceleration,
                              double _minJerk,
                              double _maxJerk)
    : hasVelocityLimits(_hasVelocityLimits),
      hasAccelerationLimits(_hasAccelerationLimits),
      hasJerkLimits(_hasJerkLimits),
      minVelocity(_minVelocity),
      maxVelocity(_maxVelocity),
      minAcceleration(_minAcceleration),
      maxAcceleration(_maxAcceleration),
      minJerk(_minJerk),
      maxJerk(_maxJerk)
  {
  }

  /// \brief Enable/Disable velocity.
  public: bool hasVelocityLimits{false};

  /// \brief Enable/Disable acceleration.
  public: bool hasAccelerationLimits{false};

  /// \brief Enable/Disable jerk.
  public: bool hasJerkLimits{false};

  /// \brief Minimum velocity limit.
  public: double minVelocity{-std::numeric_limits<double>::infinity()};

  /// \brief Maximum velocity limit.
  public: double maxVelocity{std::numeric_limits<double>::infinity()};

  /// \brief Minimum acceleration limit.
  public: double minAcceleration{-std::numeric_limits<double>::infinity()};

  /// \brief Maximum acceleration limit.
  public: double maxAcceleration{std::numeric_limits<double>::infinity()};

  /// \brief Minimum jerk limit.
  public: double minJerk{-std::numeric_limits<double>::infinity()};

  /// \brief Maximum jerk limit.
  public: double maxJerk{std::numeric_limits<double>::infinity()};
};

//////////////////////////////////////////////////
SpeedLimiter::SpeedLimiter(bool   _hasVelocityLimits,
                           bool   _hasAccelerationLimits,
                           bool   _hasJerkLimits,
                           double _minVelocity,
                           double _maxVelocity,
                           double _minAcceleration,
                           double _maxAcceleration,
                           double _minJerk,
                           double _maxJerk)
  : dataPtr(std::make_unique<SpeedLimiterPrivate>(_hasVelocityLimits,
      _hasAccelerationLimits, _hasJerkLimits, _minVelocity, _maxVelocity,
      _minAcceleration, _maxAcceleration, _minJerk, _maxJerk))
{
}

//////////////////////////////////////////////////
SpeedLimiter::~SpeedLimiter() = default;

//////////////////////////////////////////////////
double SpeedLimiter::Limit(double &_v, double _v0, double _v1, double _dt) const
{
  const double tmp = _v;

  this->LimitJerk(_v, _v0, _v1, _dt);
  this->LimitAcceleration(_v, _v0, _dt);
  this->LimitVelocity(_v);

  if (ignition::math::equal(tmp, 0.0))
    return 1.0;
  else
    return _v / tmp;
}

//////////////////////////////////////////////////
double SpeedLimiter::LimitVelocity(double &_v) const
{
  const double tmp = _v;

  if (this->dataPtr->hasVelocityLimits)
  {
    _v = ignition::math::clamp(
      _v, this->dataPtr->minVelocity, this->dataPtr->maxVelocity);
  }

  if (ignition::math::equal(tmp, 0.0))
    return 1.0;
  else
    return _v / tmp;
}

//////////////////////////////////////////////////
double SpeedLimiter::LimitAcceleration(double &_v, double _v0, double _dt) const
{
  const double tmp = _v;

  if (this->dataPtr->hasAccelerationLimits)
  {
    const double dvMin = this->dataPtr->minAcceleration * _dt;
    const double dvMax = this->dataPtr->maxAcceleration * _dt;

    const double dv = ignition::math::clamp(_v - _v0, dvMin, dvMax);

    _v = _v0 + dv;
  }

  if (ignition::math::equal(tmp, 0.0))
    return 1.0;
  else
    return _v / tmp;
}

//////////////////////////////////////////////////
double SpeedLimiter::LimitJerk(double &_v, double _v0, double _v1, double _dt)
  const
{
  const double tmp = _v;

  if (this->dataPtr->hasJerkLimits)
  {
    const double dv  = _v  - _v0;
    const double dv0 = _v0 - _v1;

    const double dt2 = _dt * _dt;

    const double daMin = this->dataPtr->minJerk * dt2;
    const double daMax = this->dataPtr->maxJerk * dt2;

    const double da = ignition::math::clamp(dv - dv0, daMin, daMax);

    _v = _v0 + dv0 + da;
  }

  if (ignition::math::equal(tmp, 0.0))
    return 1.0;
  else
    return _v / tmp;
}
